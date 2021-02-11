const char *sendJson() {
  strcpy(configLive, "");
  StaticJsonDocument<512> root;
  root["now"] = rtc_now.timestamp();
  root["t"] = option.tempC;
  root["led"] = led_bright;
  root["rssi"] =  wifi.rssi;
  root["bf"] = bf;
  root["clws"] = ws_working;
  serializeJson(root, configLive);
  return configLive;
}
void connectToMqtt() {
  mqttClient.connect();
}
void onMqttConnect(bool sessionPresent) {
  bitSet(bf, 1);
  if (!bitRead(aux_bf, 3) && bitRead(bf, 1)) {
    //DEBUG_PRINT("Установлено MQTT-соединение!: %i\n", bitRead(bf, 1));
    bitSet(aux_bf, 3);
  }
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  bitClear(bf, 1);
  if (bitRead(aux_bf, 3) && !bitRead(bf, 1)) {
    //DEBUG_PRINT("MQTT-соединение разорвано!: %i\n", bitRead(bf, 1));
    bitClear(aux_bf, 3);
  }
  if (WiFi.isConnected()) mqttReconnectTimer.once(2, connectToMqtt);
}
void send_mqtt() {
  strcpy(configLive, sendJson());
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt.topic, 0, true, configLive);
  }
}
void send_ws() {
  strcpy(configLive, sendJson());
  if (ws_working) ws.textAll(configLive);
}
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  byte id = client->id();
  if (type == WS_EVT_CONNECT) {
    ws_working++;
    //DEBUG_PRINT("Установлено Websocket-соединение с ID = %i\n", id);
  } else if (type == WS_EVT_DISCONNECT) {
    //DEBUG_PRINT("Разорвано Websocket-соединение с ID = %i\n", id);
    ws.close(id);
    if (ws_working != 0) ws_working--;
  }
}
