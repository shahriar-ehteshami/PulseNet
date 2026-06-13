// #pragma once

// #include "types.h"

// class SensorManager
// {
// public:

//     bool begin();

//     bool read(PPGData& data);

//     bool isConnected();

//     float readTemperature();
// };


#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"

class SensorManager
{
public:

    bool begin();

    bool healthCheck();

    bool fingerDetected();

    long getIR();

    long getRed();

    float getTemperature();

private:

    bool checkI2CDevice();

    bool verifyPartID();

    bool initializeSensor();

    MAX30105 particleSensor;
};