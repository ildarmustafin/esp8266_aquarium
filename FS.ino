void initFileSystem(void) {
  LittleFS.begin();
  configSetup = readFile("configSetup.json", 4096);
  configChart = readFile("configChart.json", 4096);
  if (configSetup == "Failed") {
    Serial.printf("\nERROR OPENING SPIFFS\n");
    printLCD(2, 0, " ERROR OPENING  ", "     SPIFFS     ");
  } else {
    readJsonValues();
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/index.htm", "text/html");
  });
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", debug_page, processor);
  });
  server.serveStatic("/debug", LittleFS, "/debug.htm");
  server.serveStatic("/bootstrap.min.css", LittleFS, "/bootstrap.min.css", "max-age=2592000");
  server.serveStatic("/style.css", LittleFS, "/style.css", "max-age=2592000");
  server.serveStatic("/chartist.min.js", LittleFS, "/chartist.min.js", "max-age=2592000");
  server.serveStatic("/functions.js", LittleFS, "/functions.js", "max-age=2592000");
  server.serveStatic("/css_sprites.png", LittleFS, "/css_sprites.png", "max-age=2592000");
  server.serveStatic("/RUS.json", LittleFS, "/RUS.json", "max-age=2592000");
  server.serveStatic("/ENG.json", LittleFS, "/ENG.json", "max-age=2592000");
  server.serveStatic("/favicon.ico", LittleFS, "/favicon.ico", "max-age=2592000");
  server.serveStatic("/configSetup.json", LittleFS, "/configSetup.json");
  server.serveStatic("/configChart.json", LittleFS, "/configChart.json");

  server.on("/lang", HTTP_GET, [] (AsyncWebServerRequest * request) {
    defLang = request->arg("defaultLang");
    jsonWrite(configSetup, "defaultLang", defLang);
    saveConfigSetup();
    request->send(200, "text/plain", "");
  });
  server.on("/save_schedule", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("SAVE SCHEDULE: OK");
    byte selIndex = request->arg("selIndex").toInt();
    for (int b = 0; b <= 6; b++) {
      for (int c = 0; c <= 3; c++) {
        schedule[selIndex][b][c] = request->arg("schedule[" + String(selIndex) + "][" + String(b) + "][" + String(c) + "]");
        jsonWrite(configSetup, "schedule", selIndex, b, c, schedule[selIndex][b][c]);
      }
    }
    saveConfigSetup();
    printLCD(1, 0, "", " SAVE SCHEDULE  ");
    request->send(200, "text/plain", "");
  });
  server.on("/save_date", HTTP_GET, [](AsyncWebServerRequest * request) {
    String date_man = request->arg("input[3][0]");
    String time_man = request->arg("input[3][1]");
    sscanf(date_man.c_str(), "%i-%d-%d", &years, &months, &days);
    sscanf(time_man.c_str(), "%d:%d:%d", &hours, &minutes, &seconds);
    jsonWrite(configSetup, "input", 3, 0, date_man);
    jsonWrite(configSetup, "input", 3, 1, time_man);
    saveConfigSetup();
    rtc.adjust(DateTime(years, months, days, hours, minutes, seconds));
    Serial.printf("Manual button pressed \nEntered time: %02d.%02d.%02i | %02d:%02d:%02d\n", days, months, years, hours, minutes, seconds);
    printLCD(1, 0, "", " MANUAL PRESSED ");
    request->send(200, "text/plain", "");
  });
  server.on("/auto_sync", HTTP_GET, [](AsyncWebServerRequest * request) {
    timeZone = request->arg("input[3][2]").toInt();
    ntpServerName = request->arg("input[3][3]").c_str();
    jsonWrite(configSetup, "input", 3, 2, timeZone);
    jsonWrite(configSetup, "input", 3, 3, String(ntpServerName));
    saveConfigSetup();
    updateTimeNTP();
    if (!isSyncOK) {
      printLCD(2, 0, "  NO INTERNET   ", "   CONNECTION   ");
      request->send(404, "text/plain", "NO INTERNET CONNECTION...");
    } else {
      rtc.adjust(DateTime(years, months, days, hours, minutes, seconds));
      Serial.printf("Autosync button pressed \nNTP time now: %02d.%02d.%02i | %02d:%02d:%02d\n", days, months, years, hours, minutes, seconds);
      printLCD(1, 0, "", "AUTOSYNC PRESSED");
      request->send(200, "text/plain", "");
    }
  });
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("SAVE TRANSMITTERS: OK");
    temp_koef = request->arg("input[1][0]").toFloat();
    max_day_percent   = request->arg("input[1][1]").toInt();
    max_night_percent = request->arg("input[1][2]").toInt();
    fan_start = request->arg("input[1][3]").toFloat();
    fan_stop  = request->arg("input[1][4]").toFloat();
    ten_start = request->arg("input[1][5]").toFloat();
    ten_stop  = request->arg("input[1][6]").toFloat();
    max_day   = max_day_percent * 1024 / 100;   // x = 100 * 1009 / 100 = 100
    max_night = max_night_percent * 1024 / 100; // x = 0 * 1009 / 100 = 0
    jsonWrite(configSetup, "input", 1, 0, temp_koef);
    jsonWrite(configSetup, "input", 1, 1, max_day_percent);
    jsonWrite(configSetup, "input", 1, 2, max_night_percent);
    jsonWrite(configSetup, "input", 1, 3, fan_start);
    jsonWrite(configSetup, "input", 1, 4, fan_stop);
    jsonWrite(configSetup, "input", 1, 5, ten_start);
    jsonWrite(configSetup, "input", 1, 6, ten_stop);
    saveConfigSetup();
    printLCD(1, 0, "", "  SAVE OPTIONS  ");
    request->send(200, "text/plain", "");
  });
  server.on("/chartDays", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("SAVE CHART DAYS: OK");
    int chartDays = request->arg("input[0][0]").toInt();
    jsonWrite(configSetup, "input", 0, 0, chartDays);
    saveConfigSetup();
    request->send(200, "text/plain", "");
  });
  server.on("/s_mode", HTTP_GET, [](AsyncWebServerRequest * request) {
    s_mode_led = request->arg("s_mode[0]").toInt();
    s_mode_r1  = request->arg("s_mode[1]").toInt();
    s_mode_r2  = request->arg("s_mode[2]").toInt();
    Serial.println("SAVE S_MODE: OK");
    jsonWrite(configSetup, "s_mode", 0, s_mode_led);
    jsonWrite(configSetup, "s_mode", 1, s_mode_r1);
    jsonWrite(configSetup, "s_mode", 2, s_mode_r2);
    saveConfigSetup();
    request->send(200, "text/plain", "");
  });
  server.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("SAVE MQTT: OK");
    mqttClient.disconnect();
    mqtt_server = request->arg("input[2][0]").c_str();
    mqtt_port = request->arg("input[2][1]");
    mqtt_ID   = request->arg("input[2][2]");
    mqtt_user = request->arg("input[2][3]");
    mqtt_password = request->arg("input[2][4]");
    mqtt_topic = request->arg("input[2][5]");
    mqtt_server_ip.fromString(mqtt_server);
    if (mqtt_server == "" || mqtt_ID == "" || mqtt_port == "") {
      mqttClient.disconnect();
      timer_mqtt.detach();
      mqtt_working = 0;
    } else {
      mqttClient.setServer(mqtt_server_ip, mqtt_port.toInt());
      mqttClient.connect();
      timer_mqtt.attach(1, send_values_by_mqtt);
    }
    jsonWrite(configSetup, "input", 2, 0, mqtt_server);
    jsonWrite(configSetup, "input", 2, 1, mqtt_port);
    jsonWrite(configSetup, "input", 2, 2, mqtt_ID);
    jsonWrite(configSetup, "input", 2, 3, mqtt_user);
    jsonWrite(configSetup, "input", 2, 4, mqtt_password);
    jsonWrite(configSetup, "input", 2, 5, mqtt_topic);
    saveConfigSetup();
    request->send(200, "text/plain", "");
  });
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest * request) {
    String restart = request->arg("device");
    if (restart == "1") {
      wifi_working = 2;
      request->send(200, "text/plain", "");
      restart_once.once(0.5, restart_esp);
    }
  });
  server.on("/ssid", HTTP_POST, [](AsyncWebServerRequest * request) {
    ssidAP     = request->arg("input[3][4]");
    passwordAP = request->arg("input[3][5]");
    ssid       = request->arg("input[3][6]");
    password   = request->arg("input[3][7]");
    ip_str     = request->arg("input[3][8]").c_str();
    ip.fromString(ip_str);
    ip_gw_str  = request->arg("input[3][9]").c_str();
    ip_gw.fromString(ip_gw_str);
    port       = request->arg("input[3][10]").toInt();
    WiFi.scanNetworksAsync(prinScanResult);
    jsonWrite(configSetup, "input", 3, 4, ssidAP);     //ssidAP
    jsonWrite(configSetup, "input", 3, 5, passwordAP); //ssidAP_PASS
    jsonWrite(configSetup, "input", 3, 6, ssid);       //ssid
    jsonWrite(configSetup, "input", 3, 7, password);   //ssid_PASS
    jsonWrite(configSetup, "input", 3, 8, ip_str);     //ip
    jsonWrite(configSetup, "input", 3, 9, ip_gw_str);  //ip_gateway
    jsonWrite(configSetup, "input", 3, 10, port);      //web_port
    saveConfigSetup();
    request->send(200, "text/plain", "");
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      Serial.printf("GET");
    else if (request->method() == HTTP_POST)
      Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength()) {
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for (i = 0; i < headers; i++) {
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }
    int params = request->params();
    for (i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) {
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    request->send(404);
  });

  /*
    server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404);
    });
  */
  
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (request->hasParam("lastModified", true)) {
      AsyncWebParameter* p = request->getParam("lastModified", true);
      //p->name().c_str(), p->value().c_str(), p->size()
      int modif = (cmd == U_FLASH) ? 0 : 1;
      jsonWrite(configSetup, "lastModified", modif, p->value().c_str());
      saveConfigSetup();
    }
    request->send_P(200, "text/html", "UPDATE SUCCESS");
  }, handle_update_progress_cb);

  server.begin();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.addHandler(&events);
}

void handle_update_progress_cb(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    cmd = (filename.indexOf("spiffs") > -1 || filename.indexOf("littlefs") > -1) ? U_FS : U_FLASH;
    const char * text_cmd = (cmd == U_FS) ? "FILESYSTEM" : "FIRMWARE";
    content_len = request->contentLength();
    size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Serial.printf("\nUPDATE TYPE: %s\nFILENAME:    %s\n--UPLOAD_FILE_START--\n", text_cmd, filename.c_str());
    timer_datetime.detach();
    timer_websocket.detach();
    timer_websocket.attach(1, send_values_by_websocket);
    timer_mqtt.detach();
    mqttClient.disconnect();

    Update.runAsync(true);
    if (!Update.begin((cmd == U_FS) ? fsSize : maxSketchSpace, cmd)) {
      Update.printError(Serial);
      return request->send(400, "text/plain", "OTA could not begin");
    }
  }
  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  } else {
    wifi_working = 2;
    perc = Update.progress() * 100 / content_len;
    initBar2();
    sprintf(line1, "%-8i (%3i %%)", Update.progress(), perc);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    fillBar2(0, 1, 16, perc);
    Serial.printf("Writing: %i / %i bytes (%i %%) \n", Update.progress(), content_len, perc);
  }
  if (final) {
    if (!Update.end(true)) {
      Update.printError(Serial);
    } else {
      Serial.printf("--UPLOAD_FILE_END-- \nUPDATE SUCCESS: %u\n", content_len);
      Serial.flush();
      perc = 100;
      restart_once.once(0.5, restart_esp);
    }
  }
}

String processor(const String & var) {
  char chID[15];
  char fchID[15];
  sprintf(chID, "%08X", ESP.getChipId());
  sprintf(fchID, "%08X", ESP.getFlashChipId());
  if (var == "GETFREEHEAP") return String(ESP.getFreeHeap()).c_str();
  if (var == "GETCOREVER") return String(ESP.getCoreVersion()).c_str();
  if (var == "GETBOOTVER") return String(system_get_boot_version()).c_str();
  if (var == "GETCPUFREQMHZ") return String(ESP.getCpuFreqMHz()).c_str();
  if (var == "GETSDKVER") return String(ESP.getSdkVersion()).c_str();
  if (var == "GETSKETCHMD5") return String(ESP.getSketchMD5()).c_str();
  if (var == "GETCHIPID") return chID;
  if (var == "GETFLASHCHIPID") return fchID;
  if (var == "GETFLAHCHIPREALSIZE") return String(ESP.getFlashChipRealSize()).c_str();
  if (var == "GETFLAHCHIPSIZE") return String(ESP.getFlashChipSize()).c_str();
  if (var == "GETFREESKETCHSPACE") return String(ESP.getFreeSketchSpace()).c_str();
  if (var == "GETSKETCHSIZE") return String(ESP.getSketchSize()).c_str();
  if (var == "GETFLASHCHIPSPEED") return String(ESP.getFlashChipSpeed() / 1000000).c_str();
  if (var == "GETCYCLECOUNT") return String(ESP.getCycleCount()).c_str();
  return String();
}
