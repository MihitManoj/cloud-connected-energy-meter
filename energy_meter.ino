
#include "AdafruitIO_WiFi.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Preferences.h>

// ---------------- Wi-Fi and Adafruit IO ----------------

#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASS       "YOUR_WIFI_PASSWORD"

#define IO_USERNAME     "YOUR_ADAFRUIT_USERNAME"
#define IO_KEY          "YOUR_ADAFRUIT_IO_KEY"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// ---------------- Hardware ----------------

Adafruit_INA219 ina219;
Preferences preferences;

#define MOTOR_PIN 5

// ---------------- Adafruit IO Feeds ----------------

AdafruitIO_Feed *voltageFeed =
    io.feed("voltage");

AdafruitIO_Feed *currentFeed =
    io.feed("current");

AdafruitIO_Feed *powerFeed =
    io.feed("power");

AdafruitIO_Feed *energyFeed =
    io.feed("energy");

AdafruitIO_Feed *motorFeed =
    io.feed("motor-switch");

AdafruitIO_Feed *energyResetFeed =
    io.feed("energy_reset");

// ---------------- Energy Variables ----------------

float voltage = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;

double energy_mWh = 0.0;

bool motorState = false;

unsigned long previousSampleTime = 0;
unsigned long lastPublishTime = 0;
unsigned long lastEnergyPublishTime = 0;
unsigned long lastStorageTime = 0;

const unsigned long SAMPLE_INTERVAL = 1000;
const unsigned long PUBLISH_INTERVAL = 10000;
const unsigned long ENERGY_PUBLISH_INTERVAL = 60000;
const unsigned long STORAGE_INTERVAL = 60000;

// ---------------- Motor Switch Callback ----------------

void handleMotorMessage(AdafruitIO_Data *data) {

  motorState = data->toPinLevel();

  digitalWrite(MOTOR_PIN, motorState ? HIGH : LOW);

  Serial.print("Motor state: ");
  Serial.println(motorState ? "ON" : "OFF");

  // Immediately publish the latest sensor readings
  voltageFeed->save(voltage);
  currentFeed->save(current_mA);
  powerFeed->save(power_mW);
}

// ---------------- Energy Reset Callback ----------------

void handleEnergyReset(AdafruitIO_Data *data) {

  if (data->toInt() == 1) {

    energy_mWh = 0.0;

    preferences.putDouble("energy", energy_mWh);

    energyFeed->save(energy_mWh);

    Serial.println("Energy counter reset!");

    // Reset the command feed
    energyResetFeed->save(0);
  }
}

// ---------------- Sensor Reading ----------------

void readSensorData() {

  float busVoltage = ina219.getBusVoltage_V();
  float shuntVoltage = ina219.getShuntVoltage_mV();

  // Actual load voltage
  voltage = busVoltage + (shuntVoltage / 1000.0);

  current_mA = ina219.getCurrent_mA();

  power_mW = voltage * current_mA;

  if (current_mA < 0) {
    power_mW = 0;
  }

  Serial.println("-------------------------");

  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current_mA);
  Serial.println(" mA");

  Serial.print("Power: ");
  Serial.print(power_mW);
  Serial.println(" mW");

  Serial.print("Energy: ");
  Serial.print(energy_mWh);
  Serial.println(" mWh");
}

// ---------------- Energy Calculation ----------------

void calculateEnergy(unsigned long elapsedTime) {

  float elapsedSeconds = elapsedTime / 1000.0;

  energy_mWh += (power_mW * elapsedSeconds) / 3600.0;
}

// ---------------- Publish Sensor Data ----------------

void publishSensorData() {

  voltageFeed->save(voltage);
  currentFeed->save(current_mA);
  powerFeed->save(power_mW);

  Serial.println("Sensor data published to Adafruit IO");
}

// ---------------- Setup ----------------

void setup() {

  Serial.begin(115200);

  pinMode(MOTOR_PIN, OUTPUT);

  // Initially keep motor OFF
  digitalWrite(MOTOR_PIN, LOW);

  Wire.begin(21, 22);

  if (!ina219.begin()) {
    Serial.println("INA219 sensor not detected!");
    while (1) {
      delay(1000);
    }
  }

  // Restore previously stored energy
  preferences.begin("energy-meter", false);

  energy_mWh = preferences.getDouble("energy", 0.0);

  Serial.print("Restored energy: ");
  Serial.println(energy_mWh);

  // Register Adafruit IO callbacks
  motorFeed->onMessage(handleMotorMessage);

  energyResetFeed->onMessage(handleEnergyReset);

  // Connect to Adafruit IO
  Serial.println("Connecting to Adafruit IO...");

  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Connected to Adafruit IO!");

  // Publish restored energy value
  energyFeed->save(energy_mWh);

  previousSampleTime = millis();
  lastPublishTime = millis();
  lastEnergyPublishTime = millis();
  lastStorageTime = millis();
}

// ---------------- Main Loop ----------------

void loop() {

  // Maintain cloud connection and process incoming messages
  io.run();

  unsigned long currentTime = millis();

  // Sample sensor and calculate energy every second
  if (currentTime - previousSampleTime >= SAMPLE_INTERVAL) {

    unsigned long elapsedTime =
        currentTime - previousSampleTime;

    previousSampleTime = currentTime;

    readSensorData();

    calculateEnergy(elapsedTime);
  }

  // Publish voltage, current and power every 10 seconds
  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL) {

    lastPublishTime = currentTime;

    publishSensorData();
  }

  // Publish accumulated energy every 60 seconds
  if (currentTime - lastEnergyPublishTime >= ENERGY_PUBLISH_INTERVAL) {

    lastEnergyPublishTime = currentTime;

    energyFeed->save(energy_mWh);

    Serial.println("Energy data published");
  }

  // Save accumulated energy to ESP32 flash every 60 seconds
  if (currentTime - lastStorageTime >= STORAGE_INTERVAL) {

    lastStorageTime = currentTime;

    preferences.putDouble("energy", energy_mWh);

    Serial.println("Energy saved to Preferences");
  }
}
