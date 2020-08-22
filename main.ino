// ------------- read JSON STRING -------------
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
// ------------- read JSON INTEGER -------------
int jsonReadToInt(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>().toInt();
}

int jsonReadToInt(String &json, String name, int a) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a].as<String>().toInt();
}

int jsonReadToInt(String &json, String name, int a, int b) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b].as<String>().toInt();
}
// ------------- read JSON FLOAT -------------
float jsonReadToFloat(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>().toFloat();
}

float jsonReadToFloat(String &json, String name, int a) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a].as<String>().toFloat();
}

float jsonReadToFloat(String &json, String name, int a, int b) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name][a][b].as<String>().toFloat();
}
// ------------- Запись значения json String -------------
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
// ------------- Запись значения json Integer -------------
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
// ------------- Запись значения json float -------------
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
void saveConfigSetup() {
  writeFile("configSetup.json", configSetup );
}

void saveConfigChart() {
  writeFile("configChart.json", configChart);
}

void saveLog() {
  appendFile("log.txt", configLog);
}
// ------------- Добавление строки в файл
String appendFile(String fileName, String strings ) {
  File configFile = LittleFS.open("/" + fileName, "a+");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  //strings.printTo(configFile);
  configFile.close();
  return "Write success";
}
// ------------- Запись строки в файл
String writeFile(String fileName, String strings ) {
  File configFile = LittleFS.open("/" + fileName, "w");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  //strings.printTo(configFile);
  configFile.close();
  return "Write success";
}

// ------------- Чтение файла в строку
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
