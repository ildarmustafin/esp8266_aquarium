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
  server.serveStatic("/index.htm", LittleFS, "/index.htm", "max-age=2592000");
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
      request->send(404, "text/plain", "");
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
    max_day   = max_day_percent * maxPWM / 100;   // x = 100 * 1009 / 100 = 100
    max_night = max_night_percent * maxPWM / 100; // x = 0 * 1009 / 100 = 0
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
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
    char suc[500];
    sprintf(suc, "<META http-equiv=\"refresh\" content=\"1;URL=/info\"><label>ChipId: %s</label><br/><label>FlashChipSize: %s bytes</label><br/><label>FreeSketchSpace: %s bytes</label><br/><label>FreeHeap: %s bytes</label><br/>",
            String(ESP.getChipId()).c_str(),
            String(ESP.getFlashChipSize()).c_str(),
            String(ESP.getFreeSketchSpace()).c_str(),
            String(ESP.getFreeHeap()).c_str());
    request->send_P(200, "text/html", suc);
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", successResponse);
  }, handleDoUpdate);

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404);
  });
  server.begin();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.addHandler(&events);
}

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    Serial.printf("--UPLOAD_FILE_START-- \nUpdate filename: %s \n", filename.c_str());
    content_len = request->contentLength();
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
    Update.runAsync(true);
    wifi_working = 2;
    timer_mqtt.detach();
    timer_datetime.detach();
    timer_websocket.detach();
    if (!Update.begin(content_len, cmd)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  } else {
    int perc = map(Update.progress(), 0, Update.size(), 0, 100);
    initBar2();
    sprintf(line1, "%-8i (%3i %%)", Update.progress(), perc);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    fillBar2(0, 1, 16, perc);
    Serial.printf("Writing: %i bytes (%i %%) \n", Update.progress(), perc);
  }
  if (final) {
    if (!Update.end(true)) {
      Update.printError(Serial);
    } else {
      Serial.printf("--UPLOAD_FILE_END-- \nUpdate Success: %u\n", Update.size());
      Serial.flush();
      restart_once.once(0.5, restart_esp);
    }
  }
}
