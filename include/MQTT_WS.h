#pragma once
#include <Arduino.h>
#include "GLOBAL.h"

int8_t sendOnChange(const char *key, const char *keyMQTT, int8_t state, int8_t state_old);
const char *sendJson();
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

