#pragma once
#include <Arduino.h>
#include "GLOBAL.h"

void readJsonValues();
void updateTimeNTP();
void restart_esp();
void led_schedule();
byte rele_function(long s0, long s1, byte s_m);
void relay_schedule();
void update_chart_values();
void temp_fan_regulation(float t);
void temp_ten_regulation(float t);
void initOTA();

