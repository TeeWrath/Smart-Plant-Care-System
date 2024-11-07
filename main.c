#include <DHT.h> // Include the DHT library
#define DHTPIN A2 // Analog Pin connected to DHT11 sensor
#define DHTTYPE DHT11 // Define DHT11 sensor type
#define BUZZER_PIN 4 // Digital Pin connected to the buzzer (Digital)
#define RAIN_SENSOR_PIN A0 // Analog Pin connected to rain sensor (Analog)
#define LIGHT_SENSOR_PIN A1 // Analog Pin connected to the IR sensor (Analog)
#define FIRE_SENSOR_PIN 5 // Digital Pin connected to fire sensor (Digital)
// Thresholds for temperature, humidity, and light, and rain
float tempThreshold = 30.0; // Temperature above 30°C requires watering
float humidityThreshold = 40.0; // Humidity below 40% requires watering
int lightThreshold = 300; // Example light threshold for adequate sunlight (adjust as needed)
const int rainSensorThreshold = 500; // Threshold for rain sensor value to consider it as rain
// Rain tracking variables
unsigned long lastRainTime = 0; // Last time it rained (timestamp)
unsigned long rainDuration = 0; // Duration of last rain in milliseconds
const unsigned long minRainDuration = 15 * 60 * 1000; // Minimum rain duration (15 minutes)
const unsigned long alertInterval = 6 * 60 * 60 * 1000; // 6 hours in milliseconds
DHT dht(DHTPIN, DHTTYPE); // Create an instance of the DHT sensor
// Function to initialize sensors
void initSensors() {
dht.begin(); // Initialize the DHT sensor
pinMode(LIGHT_SENSOR_PIN, INPUT); // Set the light sensor pin as an input (Analog)
}
// Function to turn off sensors
void turnOffSensors() {
Serial.println("Turning off all sensors.");
}
// Function to activate buzzer
void activateBuzzer() {
digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer to signal an alert
}
// Function to deactivate buzzer
void deactivateBuzzer() {
digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
}
void setup() {
Serial.begin(9600); // Initialize serial communication at 9600 baud
initSensors(); // Initialize all sensors
pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as an output (Digital)
pinMode(FIRE_SENSOR_PIN, INPUT); // Set the fire sensor pin as an input (Digital)
}
void loop() {
// Read temperature and humidity from the DHT11 sensor
float temp = dht.readTemperature();
float humidity = dht.readHumidity();
// Read rain sensor value (analog value)
int rainSensorValue = analogRead(RAIN_SENSOR_PIN); // Read analog value from rain sensor
// Read light level from the IR sensor
int lightLevel = analogRead(LIGHT_SENSOR_PIN);
// Read fire sensor value (0 if fire is detected, 1 if no fire)
int fireDetected = digitalRead(FIRE_SENSOR_PIN); // Read digital value from fire sensor
// Check if reading from DHT sensor failed
if (isnan(temp) || isnan(humidity)) {
Serial.println("Failed to read from DHT sensor!");
return;
}
// Display temperature, humidity, rain status, fire status, and light level on the Serial Monitor
Serial.print("Temperature: ");
Serial.print(temp);
Serial.print(" °C, Humidity: ");
Serial.print(humidity);
Serial.print(" %, Rain Sensor Value: ");
Serial.print(rainSensorValue);
Serial.print(", Fire Detected: ");
Serial.print(fireDetected == 0 ? "Yes" : "No");
Serial.print(", Light Level: ");
Serial.println(lightLevel);
// Check current time
unsigned long currentMillis = millis();
// Decision logic for rain detection
if (rainSensorValue > rainSensorThreshold) {
if (lastRainTime == 0) {
lastRainTime = currentMillis; // Start tracking the rain time
}
rainDuration = currentMillis - lastRainTime;
} else {
lastRainTime = 0;
rainDuration = 0;
}
// Decision logic for watering
bool needWatering = false;
if (temp > tempThreshold) {
Serial.println("High temperature detected.");
needWatering = true;
}
if (humidity < humidityThreshold) {
Serial.println("Low humidity detected.");
needWatering = true;
}
if (rainDuration >= minRainDuration) {
Serial.println("Sufficient rain detected. Adjusting watering decision.");
needWatering = false;
}
if (lightLevel < lightThreshold) {
Serial.println("Low light level detected. Adjusting watering decision.");
needWatering = false;
}
if (fireDetected == 0) {
Serial.println("Fire detected! Activating alarm!");
activateBuzzer();
turnOffSensors();
} else {
deactivateBuzzer();
if (!needWatering) {
Serial.println("Watering not required!");
// activateBuzzer();
}
if (needWatering && (currentMillis - lastRainTime >= alertInterval)) {
Serial.println("Watering required!");
activateBuzzer(); 9
}
}
delay(2000); // Delay for 2 seconds before the next reading
}
