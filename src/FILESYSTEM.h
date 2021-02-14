void initFileSystem(void) {
  LittleFS.begin();
  if (!LittleFS.exists("/configSetup.json")) {
    DEBUG_PRINT("\nERROR OPENING LittleFS\n");
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send_P(200, "text/html", upload_spiffs);
    });
    printLCD(2, " ERROR OPENING  ", "     SPIFFS     ");
  } else {
    configSetup  = readFile("/configSetup.json", 1024);
    readJsonValues();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/index.htm", "text/html");
    });
  }
  server.on("/new", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", upload_spiffs);
  });
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(LittleFS, "/info.htm", "text/html");
  });
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
    strcpy(defLang, request->arg("defaultLang").c_str()); //4
    jsonWrite(configSetup, "defaultLang", defLang);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("LANG CHANGE: OK\n");
  });
  server.on("/save_schedule", HTTP_GET, [](AsyncWebServerRequest * request) {
    byte selIndex = request->arg("selIndex").toInt();
    for (int b = 0; b <= 6; b++) {
      for (int c = 0; c <= 3; c++) {
        schedule[selIndex][b][c] = request->arg("schedule[" + String(selIndex) + "][" + String(b) + "][" + String(c) + "]");
        jsonWrite(configSetup, "schedule", selIndex, b, c, schedule[selIndex][b][c]);
      }
    }
    saveConfig("/configSetup.json", configSetup);
    printLCD(1, "", " SAVE SCHEDULE  ");
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE SCHEDULE: OK\n");
  });
  server.on("/save_date", HTTP_GET, [](AsyncWebServerRequest * request) {
    String date_man = request->arg("input[3][0]");
    String time_man = request->arg("input[3][1]");
    jsonWrite(configSetup, "input", 3, 0, date_man);
    jsonWrite(configSetup, "input", 3, 1, time_man);
    saveConfig("/configSetup.json", configSetup);
    if (bitRead(bf, 0)) {
      sscanf(date_man.c_str(), "%i-%d-%d", &ntp.year, &ntp.month, &ntp.day);
      sscanf(time_man.c_str(), "%d:%d:%d", &ntp.hour, &ntp.min, &ntp.sec);
      rtc.adjust(DateTime(ntp.year, ntp.month, ntp.day, ntp.hour, ntp.min, ntp.sec));
      printLCD(1, "", " MANUAL PRESSED ");
      request->send(200, "text/plain", "OK");
      DEBUG_PRINT("Manual button pressed \nEntered time: %02d.%02d.%02i | %02d:%02d:%02d\n", ntp.day, ntp.month, ntp.year, ntp.hour, ntp.min, ntp.sec);
    } else {
      printLCD(2, "  NO INTERNET   ", "   CONNECTION   ");
      request->send(400, "text/plain", "ERROR");
      DEBUG_PRINT("No internet. now.isValid: %i\n", bitRead(bf, 0));
    }
  });
  server.on("/auto_sync", HTTP_GET, [](AsyncWebServerRequest * request) {
    timeZone = request->arg("input[3][2]").toInt();
    jsonWrite(configSetup, "input", 3, 2, timeZone);
    saveConfig("/configSetup.json", configSetup);
    updateTimeNTP();
    if (!bitRead(aux_bf, 4) || !bitRead(bf, 0)) {
      printLCD(2, "  NO INTERNET   ", "   CONNECTION   ");
      request->send(400, "text/plain", "NO INTERNET CONNECTION");
      DEBUG_PRINT("No internet. isSyncOK: %i\n", bitRead(aux_bf, 4));
    } else {
      rtc.adjust(DateTime(ntp.year, ntp.month, ntp.day, ntp.hour, ntp.min, ntp.sec));
      printLCD(1, "", "AUTOSYNC PRESSED");
      request->send(200, "text/plain", "OK");
      DEBUG_PRINT("Internet OK. isSyncOK: %i\n", bitRead(aux_bf, 4));
      DEBUG_PRINT("Autosync button pressed \nNTP time now: %02d.%02d.%02i | %02d:%02d:%02d\n", ntp.day, ntp.month, ntp.year, ntp.hour, ntp.min, ntp.sec);
    }
  });
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest * request) {
    option.temp_koef  = request->arg("input[1][0]").toFloat();
    option.max_d_perc = request->arg("input[1][1]").toInt();
    option.max_n_perc = request->arg("input[1][2]").toInt();
    option.fan_start  = request->arg("input[1][3]").toFloat();
    option.fan_stop   = request->arg("input[1][4]").toFloat();
    option.ten_start  = request->arg("input[1][5]").toFloat();
    option.ten_stop   = request->arg("input[1][6]").toFloat();
    bitWrite(aux_bf, 1, request->arg("input[1][7]").toInt());
    max_day   = option.max_d_perc * 1024 / 100;   // x = 100 * 1009 / 100 = 100
    max_night = option.max_n_perc * 1024 / 100; // x = 0 * 1009 / 100 = 0
    jsonWrite(configSetup, "input", 1, 0, option.temp_koef);
    jsonWrite(configSetup, "input", 1, 1, option.max_d_perc);
    jsonWrite(configSetup, "input", 1, 2, option.max_n_perc);
    jsonWrite(configSetup, "input", 1, 3, option.fan_start);
    jsonWrite(configSetup, "input", 1, 4, option.fan_stop);
    jsonWrite(configSetup, "input", 1, 5, option.ten_start);
    jsonWrite(configSetup, "input", 1, 6, option.ten_stop);
    jsonWrite(configSetup, "input", 1, 7, bitRead(aux_bf, 1));
    saveConfig("/configSetup.json", configSetup);
    printLCD(1, "", "  SAVE OPTIONS  ");
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE TRANSMITTERS: OK\n");
  });
  server.on("/chartDays", HTTP_GET, [](AsyncWebServerRequest * request) {
    byte chartDays = request->arg("input[0][0]").toInt();
    jsonWrite(configSetup, "input", 0, 0, chartDays);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE CHART DAYS: OK\n");
  });
  server.on("/s_mode", HTTP_GET, [](AsyncWebServerRequest * request) {
    s_mode_led = request->arg("s_mode[0]").toInt();
    s_mode_r1  = request->arg("s_mode[1]").toInt();
    s_mode_r2  = request->arg("s_mode[2]").toInt();
    jsonWrite(configSetup, "s_mode", 0, s_mode_led);
    jsonWrite(configSetup, "s_mode", 1, s_mode_r1);
    jsonWrite(configSetup, "s_mode", 2, s_mode_r2);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE S_MODE: OK\n");
  });
  server.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest * request) {
    mqttClient.disconnect();
    strcpy(mqtt.server,   request->arg("input[2][0]").c_str()); //16
    strcpy(mqtt.ID,       request->arg("input[2][2]").c_str()); //12
    strcpy(mqtt.user,     request->arg("input[2][3]").c_str()); //12
    strcpy(mqtt.password, request->arg("input[2][4]").c_str()); //12
    strcpy(mqtt.topic,    request->arg("input[2][5]").c_str()); //12
    mqtt.port = request->arg("input[2][1]").toInt();
    mqtt.server_ip.fromString(mqtt.server);
    if (strcmp(mqtt.server, "") || mqtt.port != 0 || strcmp(mqtt.topic, "") || strcmp(mqtt.ID, "")) {
      mqttClient.setServer(mqtt.server_ip, mqtt.port);
      //mqttClient.connect();
      //timer_mqtt.attach(1, send_mqtt);
    } else {
      //mqttClient.disconnect();
      //timer_mqtt.detach();
      bitClear(bf, 1);
    }
    jsonWrite(configSetup, "input", 2, 0, mqtt.server);
    jsonWrite(configSetup, "input", 2, 1, mqtt.port);
    jsonWrite(configSetup, "input", 2, 2, mqtt.ID);
    jsonWrite(configSetup, "input", 2, 3, mqtt.user);
    jsonWrite(configSetup, "input", 2, 4, mqtt.password);
    jsonWrite(configSetup, "input", 2, 5, mqtt.topic);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE MQTT: OK\n");
  });
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest * request) {
    String restart = request->arg("device");
    if (restart == "1") {
      DEBUG_PRINT("RESTART: OK\n");
      request->send(200, "text/plain", "OK");
      restart_once.once(0.5, restart_esp);
    }
  });
  server.on("/ssid", HTTP_POST, [](AsyncWebServerRequest * request) {
    strcpy(wifi.ssidAP,     request->arg("input[3][3]").c_str()); //32
    strcpy(wifi.passwordAP, request->arg("input[3][4]").c_str()); //32
    strcpy(wifi.ssid,       request->arg("input[3][5]").c_str()); //32
    strcpy(wifi.password,   request->arg("input[3][6]").c_str()); //32
    strcpy(wifi.ip_str,     request->arg("input[3][7]").c_str()); //16
    strcpy(wifi.ip_gw_str,  request->arg("input[3][8]").c_str()); //16
    wifi.port = request->arg("input[3][9]").toInt();
    wifi.ip.fromString(wifi.ip_str);
    wifi.ip_gw.fromString(wifi.ip_gw_str);
    WiFi.scanNetworksAsync(printScanResult);
    jsonWrite(configSetup, "input", 3, 3, wifi.ssidAP);     //ssidAP
    jsonWrite(configSetup, "input", 3, 4, wifi.passwordAP); //ssidAP_PASS
    jsonWrite(configSetup, "input", 3, 5, wifi.ssid);       //ssid
    jsonWrite(configSetup, "input", 3, 6, wifi.password);   //ssid_PASS
    jsonWrite(configSetup, "input", 3, 7, wifi.ip_str);     //ip
    jsonWrite(configSetup, "input", 3, 8, wifi.ip_gw_str);  //ip_gateway
    jsonWrite(configSetup, "input", 3, 9, wifi.port);      //web_port
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    DEBUG_PRINT("SAVE WIFI: OK\n");
  });

  server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
    static byte cmd;
    if (!index)
    {
      isUpdating = true;
      timer_datetime.detach();
      timer_websocket.detach();
      timer_mqtt.detach();
      mqttClient.disconnect();
      content_len = request->contentLength();
      cmd = (filename.indexOf("spiffs") > -1 || filename.indexOf("littlefs") > -1) ? 100 : 0; //U_FS : U_FLASH;
      fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
      DEBUG_PRINT("\nUPDATE TYPE: %s\nFILENAME:    %s\n--UPLOAD_FILE_START--\n", (cmd) ? "FILESYSTEM" : "FIRMWARE", filename.c_str());
      Update.runAsync(true);
      if (!Update.begin((cmd) ? fsSize : content_len, cmd))
      {
        //Update.printError(Serial);
        isUpdating = false;
      }
    }
    if (Update.write(data, len) != len)
    {
      //Update.printError(Serial);
      isUpdating = false;
    }
    else
    {
      isUpdating = true;
      perc = Update.progress() * 100 / Update.size();
      initBar2();
      snprintf(line1, 17, "%-8i (%3i %%)", Update.progress(), perc);
      lcd.setCursor(0, 0);
      lcd.print(line1);
      fillBar2(0, 1, 16, perc);
      DEBUG_PRINT("Writing: %i / %i bytes (%i %%)\n", Update.progress(), Update.size(), perc);
    }
    if (final)
    {
      request->send_P(200, "text/html", successResponse);
      if (!Update.end(true))
      {
        //Update.printError(Serial);
      }
      else
      {
        DEBUG_PRINT("--UPLOAD_FILE_END-- \nUPDATE SUCCESS: %u\n", (cmd) ? fsSize : content_len);
        restart_once.once(0.5, restart_esp);
      }
      isUpdating = false;
    }
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404);
  });

  server.begin();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
}
