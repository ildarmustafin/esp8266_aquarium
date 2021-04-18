#pragma once
#ifndef JSON_H
#define JSON_H

#include <Arduino.h>
#include "GLOBAL.h"

String jsonWrite(String &json, String name, String volume);
String jsonWrite(String &json, String name, uint8_t a, String volume);
String jsonWrite(String &json, String name, uint8_t a, uint8_t b, String volume);
String jsonWrite(String &json, String name, uint8_t a, uint8_t b, uint8_t c, String volume);
String jsonWrite(String &json, String name, byte a, float volume);
String jsonWrite(String &json, String name, byte a, byte b, float volume);
void saveConfig(String fileName, String strings);
String readFile(String fileName, size_t len);

#endif
