#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"

#define SDA_PIN 8
#define SCL_PIN 9

#define MAX30102_ADDRESS 0x57
#define EXPECTED_PART_ID 0x15

#define RETRY_INTERVAL_MS 10000
#define HEALTH_CHECK_INTERVAL_MS 5000

MAX30105 particleSensor;

enum class SystemState
{
    BOOTING,
    FAULT,
    READY
};

SystemState systemState = SystemState::BOOTING;

unsigned long lastRetryTime = 0;
unsigned long lastHealthCheckTime = 0;

// ======================================================
// Utility
// ======================================================

void sendAlert(const char *message)
{
    Serial.print("[ALERT] ");
    Serial.println(message);
}

void sendInfo(const char *message)
{
    Serial.print("[INFO] ");
    Serial.println(message);
}

// ======================================================
// Hardware Checks
// ======================================================

bool checkI2CDevice()
{
    Wire.beginTransmission(MAX30102_ADDRESS);
    return (Wire.endTransmission() == 0);
}

bool verifyPartID()
{
    Wire.beginTransmission(MAX30102_ADDRESS);
    Wire.write(0xFF);

    if (Wire.endTransmission(false) != 0)
        return false;

    Wire.requestFrom(MAX30102_ADDRESS, 1);

    if (!Wire.available())
        return false;

    uint8_t partID = Wire.read();

    Serial.printf(
        "[INFO] MAX30102 Part ID = 0x%02X\n",
        partID);

    return (partID == EXPECTED_PART_ID);
}

bool initializeSensor()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
        return false;

    particleSensor.setup(
        60,     // LED Brightness
        4,      // Sample Average
        2,      // RED + IR
        100,    // Sample Rate
        411,    // Pulse Width
        16384); // ADC Range

    return true;
}

bool checkTemperature()
{
    float temp = particleSensor.readTemperature();

    Serial.printf(
        "[INFO] Temperature = %.2f C\n",
        temp);

    if (isnan(temp))
        return false;

    if (temp < -20 || temp > 100)
        return false;

    return true;
}

// ======================================================
// Complete Startup Diagnostic
// ======================================================

bool runDiagnostics()
{
    sendInfo("Running diagnostics...");

    if (!checkI2CDevice())
    {
        sendAlert("MAX30102 not detected on I2C bus");
        return false;
    }

    sendInfo("MAX30102 detected");

    if (!verifyPartID())
    {
        sendAlert("Invalid Part ID");
        return false;
    }

    if (!initializeSensor())
    {
        sendAlert("Sensor initialization failed");
        return false;
    }

    sendInfo("Sensor initialized");

    if (!checkTemperature())
    {
        sendAlert("Temperature check failed");
        return false;
    }

    sendInfo("Diagnostics passed");

    return true;
}

// ======================================================
// Runtime Health Monitoring
// ======================================================

bool runtimeHealthCheck()
{
    if (!checkI2CDevice())
    {
        sendAlert("Sensor disappeared from I2C bus");
        return false;
    }

    long ir = particleSensor.getIR();

    if (ir < 0)
    {
        sendAlert("Invalid IR reading");
        return false;
    }

    return true;
}

// ======================================================
// Setup
// ======================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("=================================");
    Serial.println("Pulse Oximeter Node");
    Serial.println("Firmware v1.0");
    Serial.println("=================================");

    Wire.begin(SDA_PIN, SCL_PIN);

    if (runDiagnostics())
    {
        systemState = SystemState::READY;
        sendInfo("System READY");
    }
    else
    {
        systemState = SystemState::FAULT;
        lastRetryTime = millis();
    }
}

// ======================================================
// Loop
// ======================================================

void loop()
{
    switch (systemState)
    {
    case SystemState::FAULT:
    {
        if (millis() - lastRetryTime >= RETRY_INTERVAL_MS)
        {
            sendInfo("Retrying diagnostics...");

            if (runDiagnostics())
            {
                sendInfo("Recovery successful");
                systemState = SystemState::READY;
            }

            lastRetryTime = millis();
        }

        break;
    }

    case SystemState::READY:
    {
        if (millis() - lastHealthCheckTime >= HEALTH_CHECK_INTERVAL_MS)
        {
            if (!runtimeHealthCheck())
            {
                sendAlert("Health check failed");
                systemState = SystemState::FAULT;
                lastRetryTime = millis();
                break;
            }

            lastHealthCheckTime = millis();
        }

        long irValue = particleSensor.getIR();
        long redValue = particleSensor.getRed();

        Serial.print(irValue);
        Serial.print(",");
        Serial.println(redValue);

        break;
    }

    default:
        break;
    }
}