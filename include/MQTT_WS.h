#pragma once
#include <Arduino.h>
#include "GLOBAL.h"

byte sendOnChange(const char *key, const char *keyMQTT, byte state, byte state_old);
const char *sendJson();
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

