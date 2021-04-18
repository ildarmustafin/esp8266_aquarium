#include "JSON.h"

String jsonWrite(String &json, String name, String volume)
{
  DynamicJsonDocument root(2560); //2478
  deserializeJson(root, json);
  root[name] = volume;
  json = "";
  serializeJson(root, json);
  //Serial.println(root.memoryUsage());
  return json;
}
String jsonWrite(String &json, String name, uint8_t a, String volume)
{ //ver
  DynamicJsonDocument root(2560); //2478
  deserializeJson(root, json);
  root[name][a] = volume;
  json = "";
  serializeJson(root, json);
  //serializeJson(root, Serial);
  //Serial.println(root.memoryUsage());
  return json;
}
String jsonWrite(String &json, String name, uint8_t a, uint8_t b, String volume)
{
  DynamicJsonDocument root(2560);
  deserializeJson(root, json);
  root[name][a][b] = volume;
  json = "";
  serializeJson(root, json);
  //Serial.println(root.memoryUsage());
  return json;
}
String jsonWrite(String &json, String name, uint8_t a, uint8_t b, uint8_t c, String volume)
{
  DynamicJsonDocument root(2560);
  deserializeJson(root, json);
  root[name][a][b][c] = volume;
  json = "";
  serializeJson(root, json);
  //Serial.println(root.memoryUsage());
  return json;
}

String jsonWrite(String &json, String name, byte a, float volume)
{
  DynamicJsonDocument root(2560);
  deserializeJson(root, json);
  root[name][a] = volume;
  json = "";
  serializeJson(root, json);
  //Serial.println(root.memoryUsage());
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, float volume)
{
  DynamicJsonDocument root(6656);
  deserializeJson(root, json);
  root[name][a][b] = volume;
  json = "";
  serializeJson(root, json);
  //Serial.println(root.memoryUsage());
  return json;
}
void saveConfig(String fileName, String strings)
{
  File configFile = LittleFS.open(fileName, "w");
  configFile.print(strings);
  configFile.close();
}
String readFile(String fileName, size_t len)
{
  File configFile = LittleFS.open(fileName, "r");
  Serial.print(F("File: "));
  Serial.print(fileName);
  Serial.print(F(" size: "));
  Serial.println(configFile.size());
  String cSetup = configFile.readString();
  configFile.close();
  return cSetup;
}
