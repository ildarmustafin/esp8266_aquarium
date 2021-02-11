String jsonWrite(String &json, String name, String volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, String volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, String volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a][b] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, byte c, String volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a][b][c] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}

String jsonWrite(String &json, String name, int volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, int volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, int volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a][b] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}

String jsonWrite(String &json, String name, float volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, float volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, float volume) {
  DynamicJsonDocument root(6656);
  deserializeJson(root, json);
  root[name][a][b] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
String jsonWrite(String &json, String name, byte a, byte b, byte c, float volume) {
  DynamicJsonDocument root(3072);
  deserializeJson(root, json);
  root[name][a][b][c] = volume;
  json = "";
  serializeJson(root, json);
  return json;
}
void saveConfig(String fileName, String strings) {
  File configFile = LittleFS.open(fileName, "w");
  configFile.print(strings);
  configFile.close();
}
String readFile(String fileName, size_t len) {
  File configFile = LittleFS.open(fileName, "r");
  //DEBUG_PRINT("\nFile: %s | size: %i\n", fileName.c_str(), configFile.size());
  String cSetup = configFile.readString();
  configFile.close();
  return cSetup;
}

