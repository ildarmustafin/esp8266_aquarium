void send_values_by_mqtt() {
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(), 0, true, jsonLive.c_str());
  } else {
    mqttClient.connect();
  }
}
void onMqttConnect(bool sessionPresent) {
  bitSet(bf, 1);
  if (!mqttFlag &&  bitRead(bf, 1)) {
    Serial.printf("Установлено MQTT-соединение!\n");
    mqttFlag = 1;
  }
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  bitClear(bf, 1);
  if (mqttFlag && ! bitRead(bf, 1)) {
    Serial.printf("MQTT-соединение разорвано!\n");
    mqttFlag = 0;
  }
}

void send_values_by_websocket() {
  jsonWrite(jsonLive, "now", dt_now);
  jsonWrite(jsonLive, "t", tempC);
  jsonWrite(jsonLive, "led", led_bright);
  jsonWrite(jsonLive, "rssi", WiFi.RSSI());
  jsonWrite(jsonLive, "bf", bf);
  jsonWrite(jsonLive, "clws", ws_working);
  jsonWrite(jsonLive, "perc", perc);
  if (ws_working) ws.textAll(jsonLive);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  byte id = client->id();
  if (type == WS_EVT_CONNECT) {
    ws_working++;
    Serial.printf("Установлено Websocket-соединение с ID = %i\n", id);
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Разорвано Websocket-соединение с ID = %i\n", id);
    ws.close(id);
    if (ws_working != 0) ws_working--;
  }
}
