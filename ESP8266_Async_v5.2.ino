#include "include_define.h"
#include "ruswords.h"

void setup() {
  Serial.begin(74880);
  analogWriteFreq(100);
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(TENPIN, OUTPUT);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  analogWrite(LEDPIN, 0);
  initLCD();
  initFileSystem();
  digitalWrite(FANPIN, chk_inverse);
  digitalWrite(TENPIN, chk_inverse);
  digitalWrite(RELAY1PIN, chk_inverse);
  digitalWrite(RELAY2PIN, chk_inverse);

  sprintf(line2, "FW:%s  FS:%s", fwver.c_str(), fsver.c_str());
  printLCD(2, 0, "  ESP8266_AQUA  ", line2);
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.onEvent(WiFiEvent);

  initSTA();
  configTime(timeZone * 3600, 0, ntpServerName, "time.nist.gov");
  sensors.begin();
  rtc.begin();
  timer_datetime.attach(1, measure_datetime);
  timer_websocket.attach(0.5, send_values_by_websocket);

  if (!mqtt_server.isEmpty() && !mqtt_ID.isEmpty() && !mqtt_port.isEmpty()) {
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
  ws.cleanupClients();
}
