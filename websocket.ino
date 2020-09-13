void send_values_by_websocket() {
  jsonWrite(jsonLive, "now", dt_now);
  jsonWrite(jsonLive, "temp", temp_filtered);
  jsonWrite(jsonLive, "led", led_bright);
  jsonWrite(jsonLive, "rssi", rssi);
  jsonWrite(jsonLive, "bitFlags", bitFlags);
  jsonWrite(jsonLive, "Terr", Terr);
  jsonWrite(jsonLive, "ws", ws_working);
  if (ws_working != 0) webSocket.broadcastTXT(jsonLive);
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("Websocket-соединение с %s разорвано!\n", wsIP[num].c_str());
      if (ws_working != 0) ws_working--;
      break;
    case WStype_CONNECTED:
      wsIP[num] = webSocket.remoteIP(num).toString();
      ws_working++;
      Serial.printf("Установлено Websocket-соединение с %s!\n", wsIP[num].c_str());
      break;
  }
}
