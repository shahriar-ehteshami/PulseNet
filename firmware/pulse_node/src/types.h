
// struct PPGData
// {
//     uint32_t timestamp;

//     long ir;

//     long red;

//     float temperature;
// };


#pragma once

#include <Arduino.h>

enum class SystemState
{
    BOOTING,
    FAULT,
    READY
};

enum class MeasurementState
{
    WAIT_FOR_FINGER,
    MEASURING,
    PROCESSING,
    REPORT,
    IDLE
};

struct MeasurementResult
{
    bool valid = false;

    float temperature = 0;

    uint32_t sampleCount = 0;

    float sqi = 0;
};