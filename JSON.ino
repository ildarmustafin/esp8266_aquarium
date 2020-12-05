String jsonReadToStr(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>();
}
String jsonReadToStr(String &json, String name, int a) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a].as<String>();
}
String jsonReadToStr(String &json, String name, int a, int b) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b].as<String>();
}
String jsonReadToStr(String &json, String name, int a, int b, int c) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b][c].as<String>();
}
int jsonReadToInt(String &json, String name, int a) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a].as<signed int>();
}
int jsonReadToInt(String &json, String name, int a, int b) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b].as<signed int>();  
}
int jsonReadToLong(String &json, String name, int a) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a].as<unsigned long>();
}
float jsonReadToFloat(String &json, String name, int a, int b) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b].as<float>();
  //return root[name][a][b].as<String>().toFloat();
}
String jsonWrite(String &json, String name, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int b, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a][b] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int b, int c, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a][b][c] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int b, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a][b] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, float volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, float volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int b, float volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a][b] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, int b, int c, float volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a][b][c] = volume;
  json = "";
  root.printTo(json);
  return json;
}
String jsonWrite(String &json, String name, int a, long long volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name][a] = volume;
  json = "";
  root.printTo(json);
  return json;
}
void saveConfigSetup() {
  writeFile("configSetup.json", configSetup );
}
void saveConfigChart() {
  writeFile("configChart.json", configChart);
}
String appendFile(String fileName, String strings ) {
  File configFile = LittleFS.open("/" + fileName, "a+");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  configFile.close();
  return "Write success";
}
String writeFile(String fileName, String strings ) {
  File configFile = LittleFS.open("/" + fileName, "w");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  configFile.close();
  return "Write success";
}
String readFile(String fileName, size_t len ) {
  File configFile = LittleFS.open("/" + fileName, "r");
  if (!configFile) {
    return "Failed";
  }
  size_t size = configFile.size();
  if (size > len) {
    configFile.close();
    return "Large";
  }
  String temp = configFile.readString();
  configFile.close();
  return temp;
}
