#include "include_define.h"
#include "rtc.h"
#include "ruswords.h"

void setup() {
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args,BYTE);
#endif
  Serial.begin(74880);
  analogWriteFreq(100);
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(TENPIN, OUTPUT);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  analogWrite(LEDPIN, 0);
  digitalWrite(FANPIN, LOW);
  digitalWrite(TENPIN, LOW);
  digitalWrite(RELAY1PIN, LOW);
  digitalWrite(RELAY2PIN, LOW);
  initLCD();
  sprintf(line2, "    VER:%s    ", ver);
  printLCD(2, 0, "  ESP8266_AQUA  ", line2);
  initFileSystem();
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.onEvent(WiFiEvent);
  initSTA();
  configTime(timeZone * 3600, 0, ntpServerName, "time.nist.gov");
  sensors.begin();
  rtc.begin();
  timer_datetime.attach(1, measure_datetime);
  timer_websocket.attach(0.5, send_values_by_websocket);

  if (mqtt_server != "" && mqtt_ID != "" && mqtt_port != "") {
    mqttClient.setServer(mqtt_server_ip, mqtt_port.toInt());
    mqttClient.connect();
    timer_mqtt.attach(1, send_values_by_mqtt);
  } else {
    timer_mqtt.detach();
  }
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
}

void loop() {
  yield();
  ws.cleanupClients(10);
}
