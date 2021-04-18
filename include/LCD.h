#pragma once
#include <Arduino.h>
#include "GLOBAL.h"

void printLCD(byte colEmpty, const char *line1, const char *line2);
void updateZnak(byte printB, bool value, byte col, byte row);
void updateZnak(String printB, bool value, byte col, byte row);
void initLCD();
void initBar2();
void fillBar2(byte start_pos, byte row, byte bar_length, byte fill_percent);
void line_show();

