#include <Wire.h>
#include <Adafruit_MCP3008.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <LCD_I2C.h>

// WiFi credentials
const char* ssid = "Project";
const char* password = "12345678";

WiFiClient client;
Adafruit_MCP3008 adc;

LCD_I2C lcd(0x27);

const int relayPin = D4;

const int VOLTAGE_SENSOR_CHANNEL = 0;
const int CURRENT_SENSOR_CHANNEL = 1;
const int SAMPLES = 1000;

// Calibration constants
const float AC_VOLTAGE_CALIB_INTERCEPT = -0.04;
const float AC_VOLTAGE_CALIB_SLOPE = 0.0405;

void setup() {
  Serial.begin(9600);
  adc.begin();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Initially off

  lcd.begin(16, 2);
  lcd.backlight();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);
}

void loop() {
  // Read control value from ThingSpeak channel
  int controlValue = ThingSpeak.readIntField(596974, 1, "NRLP8LEVYXY21JMC");
  
  // Control relay based on the received value
  digitalWrite(relayPin, (controlValue == 1) ? HIGH : LOW);

  // Read AC voltage and current
  float acVoltageRMS = calculateACVoltageRMS(VOLTAGE_SENSOR_CHANNEL);
  float acCurrentRMS = calculateACCurrentRMS(CURRENT_SENSOR_CHANNEL);

  // Print readings to serial monitor
  Serial.print("AC Voltage RMS: ");
  Serial.println(acVoltageRMS);
  Serial.print("AC Current RMS: ");
  Serial.println(acCurrentRMS);

  // Display readings on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AC Voltage: ");
  lcd.print(acVoltageRMS);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("AC Current: ");
  lcd.print(acCurrentRMS);
  lcd.print("mA");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, acVoltageRMS);
  ThingSpeak.setField(2, acCurrentRMS);

  int xuk = ThingSpeak.writeFields(1013258, "QNRWO798ZZV2OEIL");

  delay(15000); // 15 seconds delay between updates
}

float calculateACVoltageRMS(int channel) {
  float sumSquared = 0;
  for (int i = 0; i < SAMPLES; i++) {
    int adcValue = adc.readADC(channel);
    float voltage = (adcValue * 3.3) / 1024.0; // Convert ADC value to voltage
    sumSquared += voltage * voltage;
    delayMicroseconds(100); // Sampling frequency
  }
  float rms = sqrt(sumSquared / SAMPLES);
  return rms;
}

float calculateACCurrentRMS(int channel) {
  // Implement current RMS calculation
}
