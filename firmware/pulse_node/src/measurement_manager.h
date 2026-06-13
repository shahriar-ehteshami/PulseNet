#pragma once

#include "sensor_manager.h"
#include "types.h"

class MeasurementManager
{
public:

    MeasurementManager(SensorManager& sensorRef);

    void begin();

    void update();

private:

    void startMeasurement();

    void collectSamples();

    void processMeasurement();

    void reportMeasurement();

    float calculateSQI();

    SensorManager& sensor;

    MeasurementState state;

    static const uint16_t MAX_SAMPLES = 1000;

    uint32_t irBuffer[MAX_SAMPLES];

    uint32_t redBuffer[MAX_SAMPLES];

    uint16_t sampleCount;

    unsigned long measurementStartTime;

    unsigned long idleStartTime;

    MeasurementResult result;
};