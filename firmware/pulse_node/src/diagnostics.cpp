#include <Arduino.h>
#include "diagnostics.h"

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