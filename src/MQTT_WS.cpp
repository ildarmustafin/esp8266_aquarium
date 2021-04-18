#include "MQTT_WS.h"

int8_t sendOnChange(const char *key, const char *keyMQTT, int8_t state, int8_t state_old)
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
  Serial.print(F("[MQTT] MQTT CONNECTED!: "));
  Serial.println(state.mqtt);  
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  state.mqtt = 0;
  oldState.mqtt = sendOnChange("mqtt", "", state.mqtt, oldState.mqtt);
  if (strcmp(mqtt.server, ""))
  {
    Serial.print(F("[MQTT] MQTT DISCONNECTED!:"));
    Serial.println(state.mqtt);
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
    Serial.print(F("[WS] WS CONNECTED. ID = "));
    Serial.println(id);    
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.print(F("[WS] WS DISCONNECTED. ID = "));
    Serial.println(id);
    ws.close(id);
    if (state.ws != 0 && !isUpdating)
    {
      state.ws--;
      events.send(String(state.ws).c_str(), "ws", millis());
    }
  }
}
