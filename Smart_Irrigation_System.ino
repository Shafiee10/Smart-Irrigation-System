#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ThingSpeak.h>
#include <ESPAsyncWebServer.h>

// HTML code for the web interface
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Smart Irrigation System</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap');
    body { font-family: 'Roboto', sans-serif; text-align: center; background-color: #121212; color: #e0e0e0; margin: 0; padding: 0; }
    h1 { color: #bb86fc; margin: 20px 0; font-weight: 500; }
    .widget { display: flex; justify-content: space-around; flex-wrap: wrap; padding: 20px; max-width: 900px; margin: auto; }
    .sensor, .status { font-size: 1.2rem; margin: 20px; padding: 20px; background: #1e1e1e; border-radius: 10px; box-shadow: 0 0 15px rgba(0, 0, 0, 0.5); width: 200px; text-align: center; transition: transform 0.3s ease-in-out, box-shadow 0.3s ease-in-out; }
    .sensor:hover, .status:hover { transform: translateY(-10px); box-shadow: 0 0 25px rgba(0, 0, 0, 0.7); }
    .status { display: flex; justify-content: center; align-items: center; background-color: #00c853; color: #fff; }
    .status.on { background-color: #b00020; }
    .centered { display: flex; justify-content: center; }
  </style>
</head>
<body>
  <h1>Smart Irrigation System</h1>
  <div class="widget">
    <div class="sensor" id="moisture-widget">
      <h2>Soil Moisture</h2>
      <p><span id="moisture">0</span>%</p>
    </div>
    <div class="sensor" id="temperature-widget">
      <h2>Temperature</h2>
      <p><span id="temperature">0.0</span>°C</p>
    </div>
    <div class="sensor" id="humidity-widget">
      <h2>Humidity</h2>
      <p><span id="humidity">0.0</span>%</p>
    </div>
  </div>
  <div class="centered">
    <div class="status" id="pump-status">
      Pump Status: ON
    </div>
  </div>
  <script>
    setInterval(function() {
      fetch('/data').then(response => response.json()).then(data => {
        document.getElementById("moisture").innerHTML = data.moisture;
        document.getElementById("temperature").innerHTML = data.temperature;
        document.getElementById("humidity").innerHTML = data.humidity;
        const pumpStatus = document.getElementById("pump-status");
        if (data.pumpStatus) {
          pumpStatus.classList.add('on');
          pumpStatus.innerHTML = 'Pump Status: OFF';
        } else {
          pumpStatus.classList.remove('on');
          pumpStatus.innerHTML = 'Pump Status: ON';
        }
      });
    }, 2000);
  </script>
</body>
</html>
)rawliteral";

// Soil Moisture Sensor
const int soilMoisturePin = 35;

// DHT11 Sensor
#define DHTPIN 12 // Pin where the data line is connected
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Relay Pin
const int relayPin = 15;

//Fan Pin
const int fanPin1 = 2;
const int fanPin2 = 4;
const int fanEN = 5;

//Heater Pin

const int heater = 18;
// Wi-Fi credentials
char ssid[] = "ANI";
char pass[] = "Anirsharma";

// ThingSpeak credentials
unsigned long myChannelNumber = 2751970;
const char * myWriteAPIKey = "4MQL8MY8XM9X6009";

// Adjusted threshold for inverse mapping
const int moistureThreshold = 40; // Moisture percentage threshold for inverse mapping
const float tempHighThreshold = 30.0; // High temperature in Celsius
const float humidityLowThreshold = 40.0; // Low humidity percentage

WiFiClient client;
AsyncWebServer server(80);

// Variables to store sensor values
float temperature = 0.0;
float humidity = 0.0;
int moisturePercent = 0;
bool pumpStatus = false; // Track the status of the pump
unsigned long lastThingSpeakUpdate = 0; // Track the last ThingSpeak update time

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initial setup for relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure the pump is off initially

  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(fanEN, OUTPUT);
  pinMode(heater, OUTPUT);

  digitalWrite(fanEN, HIGH);
  digitalWrite(fanPin1, HIGH);
  digitalWrite(fanPin2, HIGH);
  digitalWrite(heater, LOW);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Route to get sensor data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temperature\":" + String(temperature) + ",";
    json += "\"humidity\":" + String(humidity) + ",";
    json += "\"moisture\":" + String(moisturePercent) + ",";
    json += "\"pumpStatus\":" + String(digitalRead(relayPin) == HIGH ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  // Start server
  server.begin();
}

void loop() {
  // Read the analog value from the soil moisture sensor
  int sensorValue = analogRead(soilMoisturePin);

  // Convert the analog value to an inverse percentage
  moisturePercent = map(sensorValue, 4095, 0, 0, 100);

  // Read humidity and temperature values from DHT11
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Control the water pump based on thresholds
  if (moisturePercent > moistureThreshold) {
    digitalWrite(relayPin, HIGH); // Turn on the water pump
    pumpStatus = true;
  } else if (temperature > tempHighThreshold && humidity < humidityLowThreshold) {
    digitalWrite(relayPin, HIGH); // Turn on the water pump
    pumpStatus = true;
  } else {
    digitalWrite(relayPin, LOW); // Turn off the water pump
    pumpStatus = false;
  }

  if (temperature > 26)
  {
    digitalWrite(fanPin1, HIGH);
    digitalWrite(fanPin2, LOW);
  }
  else if (temperature < 24)
  {
    digitalWrite(fanPin1, HIGH);
    digitalWrite(fanPin1, HIGH);
    digitalWrite(heater, HIGH);
  }
  else
  {
    digitalWrite(fanPin1, HIGH);
    digitalWrite(fanPin1, HIGH);
    digitalWrite(heater, LOW);
  }

  // Print values to the Serial Monitor
  Serial.print("Soil Moisture (Inverted): ");
  Serial.print(moisturePercent);
  Serial.println("%");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("C");

  // Upload data to ThingSpeak every 15 seconds
  if (millis() - lastThingSpeakUpdate >= 15000) {
    ThingSpeak.setField(1, moisturePercent);
    ThingSpeak.setField(2, temperature);
    ThingSpeak.setField(3, humidity);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code: " + String(x));
    }

    lastThingSpeakUpdate = millis();
  }

  delay(2000); // Delay before the next loop iteration
}