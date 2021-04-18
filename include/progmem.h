#pragma once
#ifndef PROGMEM_H
#define PROGMEM_H
#include <Arduino.h>

static const char successResponse[] PROGMEM =
  "<META http-equiv=\"refresh\" content=\"20;URL=/\">Update Success! Rebooting...Wait for 20 sec\n";

static const char upload_spiffs[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><HTML><HEAD><TITLE>ESP_UPDATE</TITLE></HEAD><BODY><form id="update"method="POST"action="/update"enctype="multipart/form-data"><input type="file"name="Update"><input type="submit"value="Update"><br><label id="d"></label></form></BODY><SCRIPT>if(!!window.EventSource){var source=new EventSource('/events');source.addEventListener('ota',function(e){console.log("[SSE] OTA:",e.data);document.getElementById("d").innerHTML="Download: "+e.data+" %";},false);}</SCRIPT></HTML>)rawliteral";
#endif