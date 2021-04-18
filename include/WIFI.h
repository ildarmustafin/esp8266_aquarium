#pragma once
#include <Arduino.h>
#include "GLOBAL.h"

void search_wifi();
void WiFi_initAP_STA();
void WiFi_initSTA();
void printScanResult(int networksFound);
void onStationConnected(const WiFiEventStationModeConnected &evt);
void onStationDisconnected(const WiFiEventStationModeDisconnected &evt);
void onStationGotIp(const WiFiEventStationModeGotIP &evt);
void onProbeRequestReceive(const WiFiEventSoftAPModeProbeRequestReceived &evt);