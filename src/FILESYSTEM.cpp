#include "FILESYSTEM.h"

extern void initFileSystem(void)
{
  LittleFS.begin();
  if (!LittleFS.exists("/configSetup.json"))
  {
    Serial.println(F("[FS] ERROR OPENING LittleFS"));
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", upload_spiffs);
    });
    printLCD(2, PSTR(" ERROR OPENING  "), PSTR("     SPIFFS     "));
  }
  else
  {
    configSetup = readFile("/configSetup.json", 1024);
    readJsonValues();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/index.htm", "text/html");
    });
  }
  server.on("/new", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", upload_spiffs);
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
  server.on("/lang", HTTP_GET, [](AsyncWebServerRequest *request) {
    strcpy(defLang, request->arg("defaultLang").c_str());
    jsonWrite(configSetup, "defaultLang", defLang);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "OK");
    Serial.println(F("[GET] LANG CHANGE: OK"));
  });
  server.on("/save_schedule", HTTP_GET, [](AsyncWebServerRequest *request) {
    byte selIndex = request->arg("selIndex").toInt();
    for (int a = 0; a <= 6; a++)
    {
      for (int b = 0; b <= 3; b++)
      {
        if (!selIndex)
        {
          strcpy(sched_led[a][b], request->arg("schedule[0][" + String(a) + "][" + String(b) + "]").c_str());
          jsonWrite(configSetup, "schedule", 0, a, b, sched_led[a][b]);
        }
        else
        {
          strcpy(sched_rel[a][b], request->arg("schedule[1][" + String(a) + "][" + String(b) + "]").c_str());
          jsonWrite(configSetup, "schedule", 1, a, b, sched_rel[a][b]);
        }
      }
    }
    saveConfig("/configSetup.json", configSetup);
    printLCD(1, "", " SAVE SCHEDULE  ");
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE SCHEDULE: OK"));
  });
  server.on("/save_date", HTTP_GET, [](AsyncWebServerRequest *request) {
    String date_man = request->arg("input[3][0]");
    String time_man = request->arg("input[3][1]");
    jsonWrite(configSetup, "input", 3, 0, date_man);
    jsonWrite(configSetup, "input", 3, 1, time_man);
    saveConfig("/configSetup.json", configSetup);
    if (state.valNow)
    {
      sscanf(date_man.c_str(), "%i-%d-%d", &ntp.year, &ntp.month, &ntp.day);
      sscanf(time_man.c_str(), "%d:%d:%d", &ntp.hour, &ntp.min, &ntp.sec);
      rtc.adjust(DateTime(ntp.year, ntp.month, ntp.day, ntp.hour, ntp.min, ntp.sec));
      printLCD(1, "", " MANUAL PRESSED ");
      events.send(rtc.now().timestamp().c_str(), "now", millis());
      request->send(200, "text/plain", "[GET] OK");
      Serial.println(F("[GET] SET MANUAL RTC"));
      Serial.print(F("[GET] ENTERED TIME: "));
      Serial.printf("%02d.%02d.%04i %02d:%02d:%02d", ntp.day, ntp.month, ntp.year, ntp.hour, ntp.min, ntp.sec);
    }
    else
    {
      printLCD(2, "  NO INTERNET   ", "   CONNECTION   ");
      request->send(400, "text/plain", "[GET] ERROR");
      Serial.print(F("[GET] No internet. now.isValid: "));
      Serial.println(state.valNow);
    }
  });
  server.on("/auto_sync", HTTP_GET, [](AsyncWebServerRequest *request) {
    timeZone = request->arg("input[3][2]").toInt();
    jsonWrite(configSetup, "input", 3, 2, timeZone);
    saveConfig("/configSetup.json", configSetup);
    updateTimeNTP();
    if (!state.valSync || !state.valNow)
    {
      printLCD(2, "  NO INTERNET   ", "   CONNECTION   ");
      request->send(400, "text/plain", "[GET] NO INTERNET CONNECTION");
      Serial.print(F("[GET] No internet. isSyncOK: "));
      Serial.println(state.valSync);
    }
    else
    {
      rtc.adjust(DateTime(ntp.year, ntp.month, ntp.day, ntp.hour, ntp.min, ntp.sec));
      printLCD(1, "", "AUTOSYNC PRESSED");
      events.send(rtc.now().timestamp().c_str(), "now", millis());
      request->send(200, "text/plain", "[GET] OK");
      Serial.print(F("[GET] Internet OK. isSyncOK: "));
      Serial.println(state.valSync);
      Serial.println(F("[GET] SET AUTOSYNC RTC"));
      Serial.print(F("[GET] NTP TIME NOW: "));
      Serial.printf("%02d.%02d.%04i %02d:%02d:%02d", ntp.day, ntp.month, ntp.year, ntp.hour, ntp.min, ntp.sec);
    }
  });
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request) {
    option.temp_koef = request->arg("input[1][0]").toFloat();
    option.max_d_perc = request->arg("input[1][1]").toInt();
    option.max_n_perc = request->arg("input[1][2]").toInt();

    fan_reg.setpoint = request->arg("input[1][3]").toFloat();   // установка (ставим на 40 градусов)
    fan_reg.hysteresis = request->arg("input[1][4]").toFloat(); // ширина гистерезиса
    fan_reg.k = request->arg("input[1][5]").toFloat();
    ten_reg.setpoint = request->arg("input[1][6]").toFloat();   // установка (ставим на 40 градусов)
    ten_reg.hysteresis = request->arg("input[1][7]").toFloat(); // ширина гистерезиса
    ten_reg.k = request->arg("input[1][8]").toFloat();
    option.delLed = request->arg("input[1][9]").toInt();
    state.inv = request->arg("input[1][10]").toInt();
    max_day = option.max_d_perc * 1024 / 100;   // x = 100 * 1009 / 100 = 100
    max_night = option.max_n_perc * 1024 / 100; // x = 0 * 1009 / 100 = 0
    (option.delLed) ? lcd.noBacklight() : lcd.backlight();
    jsonWrite(configSetup, "input", 1, 0, option.temp_koef);
    jsonWrite(configSetup, "input", 1, 1, option.max_d_perc);
    jsonWrite(configSetup, "input", 1, 2, option.max_n_perc);
    jsonWrite(configSetup, "input", 1, 3, fan_reg.setpoint);
    jsonWrite(configSetup, "input", 1, 4, fan_reg.hysteresis);
    jsonWrite(configSetup, "input", 1, 5, fan_reg.k);
    jsonWrite(configSetup, "input", 1, 6, ten_reg.setpoint);
    jsonWrite(configSetup, "input", 1, 7, ten_reg.hysteresis);
    jsonWrite(configSetup, "input", 1, 8, ten_reg.k);
    jsonWrite(configSetup, "input", 1, 9, option.delLed);
    jsonWrite(configSetup, "input", 1, 10, state.inv);
    saveConfig("/configSetup.json", configSetup);
    printLCD(1, "", "  SAVE OPTIONS  ");
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE TRANSMITTERS: OK"));
  });
  server.on("/chartDays", HTTP_GET, [](AsyncWebServerRequest *request) {
    byte chartDays = request->arg("input[0][0]").toInt();
    jsonWrite(configSetup, "input", 0, 0, chartDays);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE CHART DAYS: OK"));
  });
  server.on("/s_mode", HTTP_GET, [](AsyncWebServerRequest *request) {
    s_mode_led = request->arg("s_mode[0]").toInt();
    s_mode_r1 = request->arg("s_mode[1]").toInt();
    s_mode_r2 = request->arg("s_mode[2]").toInt();
    jsonWrite(configSetup, "s_mode", 0, s_mode_led);
    jsonWrite(configSetup, "s_mode", 1, s_mode_r1);
    jsonWrite(configSetup, "s_mode", 2, s_mode_r2);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE S_MODE: OK"));
  });
  server.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest *request) {
    mqttClient.disconnect();
    strcpy(mqtt.server, request->arg("input[2][0]").c_str());
    strcpy(mqtt.ID, request->arg("input[2][2]").c_str());
    strcpy(mqtt.user, request->arg("input[2][3]").c_str());
    strcpy(mqtt.password, request->arg("input[2][4]").c_str());
    strcpy(mqtt.t_all, request->arg("input[2][5]").c_str());
    strcpy(mqtt.t_temp, request->arg("input[2][6]").c_str());
    strcpy(mqtt.t_led, request->arg("input[2][7]").c_str());
    strcpy(mqtt.t_fan, request->arg("input[2][8]").c_str());
    strcpy(mqtt.t_ten, request->arg("input[2][9]").c_str());
    strcpy(mqtt.t_rel1, request->arg("input[2][10]").c_str());
    strcpy(mqtt.t_rel2, request->arg("input[2][11]").c_str());
    strcpy(mqtt.t_date, request->arg("input[2][12]").c_str());
    mqtt.port = request->arg("input[2][1]").toInt();
    mqtt.server_ip.fromString(mqtt.server);
    if (strcmp(mqtt.server, ""))
    {
      mqttClient.setServer(mqtt.server_ip, mqtt.port);
      mqttClient.connect();
    }
    else
    {
      mqttClient.disconnect();
    }
    jsonWrite(configSetup, "input", 2, 0, mqtt.server);
    jsonWrite(configSetup, "input", 2, 1, mqtt.port);
    jsonWrite(configSetup, "input", 2, 2, mqtt.ID);
    jsonWrite(configSetup, "input", 2, 3, mqtt.user);
    jsonWrite(configSetup, "input", 2, 4, mqtt.password);
    jsonWrite(configSetup, "input", 2, 5, mqtt.t_all);
    jsonWrite(configSetup, "input", 2, 6, mqtt.t_temp);
    jsonWrite(configSetup, "input", 2, 7, mqtt.t_led);
    jsonWrite(configSetup, "input", 2, 8, mqtt.t_fan);
    jsonWrite(configSetup, "input", 2, 9, mqtt.t_ten);
    jsonWrite(configSetup, "input", 2, 10, mqtt.t_rel1);
    jsonWrite(configSetup, "input", 2, 11, mqtt.t_rel2);
    jsonWrite(configSetup, "input", 2, 12, mqtt.t_date);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE MQTT: OK"));
  });
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    String restart = request->arg("device");
    if (restart == "1")
    {
      Serial.println(F("[GET] RESTART: OK"));
      request->send(200, "text/plain", "[GET] OK");
      timer_once.once(0.5, restart_esp);
    }
  });
  server.on("/ssid", HTTP_POST, [](AsyncWebServerRequest *request) {
    strcpy(wifi.ssidAP, request->arg("input[3][3]").c_str());     //32
    strcpy(wifi.passwordAP, request->arg("input[3][4]").c_str()); //32
    strcpy(wifi.ssid, request->arg("input[3][5]").c_str());       //32
    strcpy(wifi.password, request->arg("input[3][6]").c_str());   //32
    strcpy(wifi.ip_str, request->arg("input[3][7]").c_str());     //16
    strcpy(wifi.ip_gw_str, request->arg("input[3][8]").c_str());  //16
    wifi.port = request->arg("input[3][9]").toInt();
    wifi.ip.fromString(wifi.ip_str);
    wifi.ip_gw.fromString(wifi.ip_gw_str);
    WiFi.scanNetworksAsync(printScanResult);
    jsonWrite(configSetup, "input", 3, 3, wifi.ssidAP);
    jsonWrite(configSetup, "input", 3, 4, wifi.passwordAP);
    jsonWrite(configSetup, "input", 3, 5, wifi.ssid);
    jsonWrite(configSetup, "input", 3, 6, wifi.password);
    jsonWrite(configSetup, "input", 3, 7, wifi.ip_str);
    jsonWrite(configSetup, "input", 3, 8, wifi.ip_gw_str);
    jsonWrite(configSetup, "input", 3, 9, wifi.port);
    saveConfig("/configSetup.json", configSetup);
    request->send(200, "text/plain", "[GET] OK");
    Serial.println(F("[GET] SAVE WIFI: OK"));
  });
  
  server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
    static byte cmd;
    if (!index)
    {
      //detachInterrupt(BUTPIN);
      isUpdating = true;
      mqttClient.disconnect();
      wifi_search.detach();
      content_len = request->contentLength();
      cmd = (filename.indexOf("spiffs") > -1 || filename.indexOf("littlefs") > -1) ? 100 : 0; //U_FS : U_FLASH;
      fsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
      Serial.print(F("[UPDATE] UPDATE TYPE: "));
      Serial.println((cmd) ? F("FILESYSTEM") : F("FIRMWARE"));
      Serial.print(F("[UPDATE] FILENAME: "));
      Serial.println(filename.c_str());
      Serial.println(F("--UPLOAD_FILE_START--"));
      Update.runAsync(true);
      if (!Update.begin((cmd) ? fsSize : content_len, cmd))
      {
        Update.printError(Serial);
        isUpdating = false;
      }
    }
    if (Update.write(data, len) != len)
    {
      Update.printError(Serial);
      isUpdating = false;
    }
    else
    {
      isUpdating = true;
      lcd.backlight();
      state.perc = Update.progress() * 100 / Update.size();
      initBar2();
      snprintf(line1, 17, "%-8i (%3i %%)", Update.progress(), state.perc);
      lcd.setCursor(0, 0);
      lcd.print(line1);
      fillBar2(0, 1, 16, state.perc);
      Serial.print(F("[UPDATE] Writing: "));
      Serial.print(Update.progress());
      Serial.print(F(" / "));
      Serial.print(Update.size());
      Serial.print(F(" bytes ("));
      Serial.print(state.perc);
      Serial.println(F(" %)"));
    }
    if (final)
    {
      //isUpdating = false;
      //ota_time.detach();
      if (Update.end(true))
      {
        Serial.println(F("--UPLOAD_FILE_END--"));
        Serial.print(F("[UPDATE] UPDATE SUCCESS: "));
        Serial.println((cmd) ? fsSize : content_len);
        Serial.println(F("RESTARTING ESP8266. PLEASE WAIT..."));
        printLCD(2, "   RESTARTING   ", "PLEASE WAIT...  ");
        delay(1000);
        request->send_P(200, "text/html", successResponse);
        timer_once.once(0.5, restart_esp);
      }
      else
      {
        Update.printError(Serial);
      }
    }
  });
  
/*
  server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char *)data);
    if (final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char *)data);
    if (index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
*/
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });
  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId())
      Serial.print(F("[SSE] reconnect! ID: "));
    Serial.println(client->lastId());
    if (!isUpdating)
    {
      //strcpy(configLive, sendJson());
      client->send(sendJson(), "ALL", millis());
    }
    else
    {
      client->send(String(state.perc).c_str(), "ota", millis());
    }
  });
  //events.setAuthentication("admin", "admin");

  server.addHandler(&events);
  server.addHandler(&ws);
  ws.onEvent(onWsEvent);
  server.begin();
}
