#include "measurement_manager.h"
#include "config.h"
#include "diagnostics.h"

MeasurementManager::MeasurementManager(
    SensorManager& sensorRef)
    : sensor(sensorRef)
{
}

void MeasurementManager::begin()
{
    state = MeasurementState::WAIT_FOR_FINGER;

    sendInfo("Waiting for finger");
}

void MeasurementManager::update()
{
    switch(state)
    {
        case MeasurementState::WAIT_FOR_FINGER:

            if(sensor.fingerDetected())
                startMeasurement();

            break;

        case MeasurementState::MEASURING:

            collectSamples();

            break;

        case MeasurementState::PROCESSING:

            processMeasurement();

            break;

        case MeasurementState::REPORT:

            reportMeasurement();

            break;

        case MeasurementState::IDLE:

            if(millis() - idleStartTime >= IDLE_TIME_MS)
            {
                state = MeasurementState::WAIT_FOR_FINGER;
                sendInfo("Waiting for finger");
            }

            break;
    }
}

void MeasurementManager::startMeasurement()
{
    sampleCount = 0;

    measurementStartTime = millis();

    sendInfo("Finger detected");

    state = MeasurementState::MEASURING;
}

void MeasurementManager::collectSamples()
{
    if(!sensor.fingerDetected())
    {
        sendAlert("Finger removed");

        state = MeasurementState::WAIT_FOR_FINGER;

        return;
    }

    if(sampleCount < MAX_SAMPLES)
    {
        irBuffer[sampleCount] = sensor.getIR();
        redBuffer[sampleCount] = sensor.getRed();

        sampleCount++;
    }

    if(millis() - measurementStartTime >=
       MEASUREMENT_TIME_MS)
    {
        state = MeasurementState::PROCESSING;
    }

    delay(10);
}

float MeasurementManager::calculateSQI()
{
    if(sampleCount == 0)
        return 0;

    uint32_t minVal = irBuffer[0];
    uint32_t maxVal = irBuffer[0];

    uint64_t sum = 0;

    for(uint16_t i=0;i<sampleCount;i++)
    {
        if(irBuffer[i] < minVal)
            minVal = irBuffer[i];

        if(irBuffer[i] > maxVal)
            maxVal = irBuffer[i];

        sum += irBuffer[i];
    }

    float mean =
        (float)sum / sampleCount;

    return (float)(maxVal - minVal) / mean;
}

void MeasurementManager::processMeasurement()
{
    result.valid = true;

    result.sampleCount = sampleCount;

    result.temperature =
        sensor.getTemperature();

    result.sqi =
        calculateSQI();

    state = MeasurementState::REPORT;
}

void MeasurementManager::reportMeasurement()
{
    Serial.println();
    Serial.println("========== RESULT ==========");

    Serial.print("Samples: ");
    Serial.println(result.sampleCount);

    Serial.print("Temperature: ");
    Serial.println(result.temperature);

    Serial.print("SQI: ");
    Serial.println(result.sqi, 4);

    Serial.println("============================");
    Serial.println();

    idleStartTime = millis();

    state = MeasurementState::IDLE;
}