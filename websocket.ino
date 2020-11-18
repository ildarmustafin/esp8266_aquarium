void send_values_by_websocket() {
  jsonWrite(jsonLive, "now", dt_now);
  jsonWrite(jsonLive, "t", tempC);
  jsonWrite(jsonLive, "led", led_bright);
  jsonWrite(jsonLive, "rssi", WiFi.RSSI());
  jsonWrite(jsonLive, "bf", bf);
  jsonWrite(jsonLive, "clws", ws_working);
  if (ws_working) ws.textAll(jsonLive);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  id = client->id();
  if (type == WS_EVT_CONNECT) {
    ws_working++;
    Serial.printf("Установлено Websocket-соединение с ID = %i\n", id);
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Разорвано Websocket-соединение с ID = %i\n", id);
    ws.close(id);
    if (ws_working != 0) ws_working--;
  }
}
