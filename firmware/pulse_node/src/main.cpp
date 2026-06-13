#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "types.h"

#include "sensor_manager.h"
#include "measurement_manager.h"

SensorManager sensor;
MeasurementManager measurement(sensor);

SystemState systemState = SystemState::BOOTING;

unsigned long lastRetryTime = 0;
unsigned long lastHealthCheckTime = 0;

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Wire.begin(SDA_PIN, SCL_PIN);

    if(sensor.begin())
    {
        measurement.begin();

        systemState = SystemState::READY;
    }
    else
    {
        systemState = SystemState::FAULT;
    }
}

void loop()
{
    switch(systemState)
    {
        case SystemState::FAULT:

            if(millis() - lastRetryTime >=
               RETRY_INTERVAL_MS)
            {
                if(sensor.begin())
                {
                    measurement.begin();

                    systemState =
                        SystemState::READY;
                }

                lastRetryTime = millis();
            }

            break;

        case SystemState::READY:

            if(millis() - lastHealthCheckTime >=
               HEALTH_CHECK_INTERVAL_MS)
            {
                if(!sensor.healthCheck())
                {
                    systemState =
                        SystemState::FAULT;
                }

                lastHealthCheckTime = millis();
            }

            // Serial.print(sensor.getIR());
            // Serial.print(",");
            // Serial.println(sensor.getRed());

            measurement.update();

            break;

        default:
            break;
    }
}