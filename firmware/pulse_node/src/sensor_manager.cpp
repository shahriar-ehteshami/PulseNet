#include "sensor_manager.h"
#include "config.h"
#include "diagnostics.h"

bool SensorManager::checkI2CDevice()
{
    Wire.beginTransmission(MAX30102_ADDRESS);
    return (Wire.endTransmission() == 0);
}

bool SensorManager::verifyPartID()
{
    Wire.beginTransmission(MAX30102_ADDRESS);
    Wire.write(0xFF);

    if (Wire.endTransmission(false) != 0)
        return false;

    Wire.requestFrom(MAX30102_ADDRESS, 1);

    if (!Wire.available())
        return false;

    uint8_t partID = Wire.read();

    Serial.printf("Part ID = 0x%02X\n", partID);

    return partID == EXPECTED_PART_ID;
}

bool SensorManager::initializeSensor()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
        return false;

    particleSensor.setup(
        60,
        4,
        2,
        SENSOR_SAMPLE_RATE,
        411,
        16384);

    return true;
}

bool SensorManager::begin()
{
    if (!checkI2CDevice())
        return false;

    if (!verifyPartID())
        return false;

    if (!initializeSensor())
        return false;

    return true;
}

bool SensorManager::healthCheck()
{
    return checkI2CDevice();
}

bool SensorManager::fingerDetected()
{
    return getIR() > FINGER_THRESHOLD;
}

long SensorManager::getIR()
{
    return particleSensor.getIR();
}

long SensorManager::getRed()
{
    return particleSensor.getRed();
}

float SensorManager::getTemperature()
{
    return particleSensor.readTemperature();
}