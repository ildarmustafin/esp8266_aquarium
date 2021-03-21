byte sendOnChange(const char *key, const char *keyMQTT, int state, int state_old)
{
  if (state != state_old && !isUpdating)
  {
    events.send(String(state).c_str(), key, millis());
    if (mqttClient.connected() && strcmp(keyMQTT, ""))
      mqttClient.publish(keyMQTT, 0, true, String(state).c_str());
  }
  return state;
}
const char *sendJson()
{
  strcpy(configLive, "");
  bitWrite(state.bf, 0, state.valNow);
  bitWrite(state.bf, 1, state.mqtt);
  bitWrite(state.bf, 2, state.rel2);
  bitWrite(state.bf, 3, state.rel1);
  bitWrite(state.bf, 4, state.ten);
  bitWrite(state.bf, 5, state.fan);
  bitWrite(state.bf, 6, state.ws);
  StaticJsonDocument<512> root;
  root["now"] = rtc.now().timestamp();
  root["t"] = option.tempC;
  root["led"] = state.led;
  root["rssi"] = wifi.rssi;
  root["bf"] = state.bf;
  root["heap"] = state.heapMin;
  serializeJson(root, configLive);
  return configLive;
}
void connectToMqtt()
{
  if (!isUpdating)
    mqttClient.connect();
}
void onMqttConnect(bool sessionPresent)
{
  state.mqtt = 1;
  mqttReconnectTimer.detach();
  oldState.mqtt = sendOnChange("mqtt", "", state.mqtt, oldState.mqtt);
  mqttClient.publish(mqtt.t_all, 0, true, sendJson());
  SERIAL_PRINT("[MQTT] Установлено MQTT-соединение!: %i\n", state.mqtt);
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  state.mqtt = 0;
  oldState.mqtt = sendOnChange("mqtt", "", state.mqtt, oldState.mqtt);
  if (strcmp(mqtt.server, ""))
  {
    SERIAL_PRINT("[MQTT] MQTT-соединение разорвано!: %i\n", state.mqtt);
    mqttReconnectTimer.attach(3, connectToMqtt);
  }
}
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  byte id = client->id();
  if (type == WS_EVT_CONNECT)
  {
    state.ws++;
    if (!isUpdating)
      events.send(String(state.ws).c_str(), "ws", millis());
    SERIAL_PRINT("[WS] Установлено Websocket-соединение с ID = %i\n", id);
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    SERIAL_PRINT("[WS] Разорвано Websocket-соединение с ID = %i\n", id);
    ws.close(id);
    if (state.ws != 0 && !isUpdating)
    {
      state.ws--;
      events.send(String(state.ws).c_str(), "ws", millis());
    }
  }
}
