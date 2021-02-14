void readJsonValues() {
  jsonWrite(configSetup, "ver", 0, ver.fw);
  saveConfig("/configSetup.json", configSetup);
  DynamicJsonDocument root(4096);
  deserializeJson(root, configSetup);
  strcpy(ver.fs,          root["ver"][1].as<const char*>()); //7
  strcpy(defLang,         root["defaultLang"].as<const char*>()); //4
  timeZone              = root["input"][3][2].as<signed int>();
  strcpy(wifi.ssidAP,     root["input"][3][3].as<const char*>()); //32
  strcpy(wifi.passwordAP, root["input"][3][4].as<const char*>()); //32
  strcpy(wifi.ssid,       root["input"][3][5].as<const char*>()); //32
  strcpy(wifi.password,   root["input"][3][6].as<const char*>()); //32
  strcpy(wifi.ip_str,     root["input"][3][7].as<const char*>()); //16
  strcpy(wifi.ip_gw_str,  root["input"][3][8].as<const char*>()); //16

  strcpy(mqtt.server,     root["input"][2][0].as<const char*>()); //16
  mqtt.port             = root["input"][2][1].as<signed int>();
  strcpy(mqtt.ID,         root["input"][2][2].as<const char*>()); //12
  strcpy(mqtt.user,       root["input"][2][3].as<const char*>()); //12
  strcpy(mqtt.password,   root["input"][2][4].as<const char*>()); //12
  strcpy(mqtt.topic,      root["input"][2][5].as<const char*>()); //12

  if (!strcmp(mqtt.server, "")) strcpy(mqtt.server, "192.168.4.1");
  if (mqtt.port == 0) mqtt.port = 1883;
  if (!strcmp(mqtt.ID, "")) strcpy(mqtt.ID, "esp_aqua");
  if (!strcmp(mqtt.topic, "")) strcpy(mqtt.topic, "all");

  option.temp_koef  = root["input"][1][0].as<float>();
  option.max_d_perc = root["input"][1][1].as<signed int>();
  option.max_n_perc = root["input"][1][2].as<signed int>();
  option.fan_start  = root["input"][1][3].as<float>();
  option.fan_stop   = root["input"][1][4].as<float>();
  option.ten_stop   = root["input"][1][5].as<float>();
  option.ten_start  = root["input"][1][6].as<float>();
  bitWrite(aux_bf, 1, root["input"][1][7].as<signed int>());

  s_mode_led        = root["s_mode"][0].as<signed int>();
  s_mode_r1         = root["s_mode"][1].as<signed int>();
  s_mode_r2         = root["s_mode"][2].as<signed int>();

  mqtt.server_ip.fromString(mqtt.server);
  wifi.ip.fromString(wifi.ip_str);
  wifi.ip_gw.fromString(wifi.ip_gw_str);
  max_day    = option.max_d_perc * 1024 / 100;
  max_night  = option.max_n_perc * 1024 / 100;
  mqttClient.disconnect();
  mqttClient.setServer(mqtt.server_ip, mqtt.port);
  mqttClient.connect();
  for (int a = 0; a <= 1; a++) {
    for (int b = 0; b <= 6; b++) {
      for (int c = 0; c <= 3; c++) {
        schedule[a][b][c] = root["schedule"][a][b][c].as<String>();
      }
    }
  }
}
void updateTimeNTP() {
  ntp.now = time(nullptr);
  struct tm * tm = localtime(&ntp.now);
  if (tm->tm_year == 70) {
    bitClear(aux_bf, 4);
  } else {
    bitSet(aux_bf, 4);
    ntp.day  = tm->tm_mday;
    ntp.month = tm->tm_mon + 1;
    ntp.year  = tm->tm_year + 1900;
    ntp.hour = tm->tm_hour;
    ntp.min = tm->tm_min;
    ntp.sec = tm->tm_sec;
  }
}
void restart_esp() {
  DEBUG_PRINT("RESTARTING ESP8266. PLEASE WAIT...\n");
  printLCD(2, "   RESTARTING   ", "PLEASE WAIT...  ");
  delay(2000);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  ESP.restart();
}
void digitalOutput(byte pin, byte bitNumber) {
  if (bitRead(aux_bf, 1)) {
    digitalWrite(pin, !bitRead(bf, bitNumber));
  } else {
    digitalWrite(pin, bitRead(bf, bitNumber));
  }
}
void led_schedule() {
  if (schedule[0][ntp.week][0] != "" || schedule[0][ntp.week][1] != "" || schedule[0][ntp.week][2] != "" || schedule[0][ntp.week][3] != "") {
    sscanf(schedule[0][ntp.week][0].c_str(), "%d:%d", &hour_0[ntp.week], &min_0[ntp.week]);
    sscanf(schedule[0][ntp.week][1].c_str(), "%d:%d", &hour_1[ntp.week], &min_1[ntp.week]);
    sscanf(schedule[0][ntp.week][2].c_str(), "%d:%d", &hour_2[ntp.week], &min_2[ntp.week]);
    sscanf(schedule[0][ntp.week][3].c_str(), "%d:%d", &hour_3[ntp.week], &min_3[ntp.week]);
    sec_0   = hour_0[ntp.week] * 3600 + min_0[ntp.week] * 60;  // сек ОТ Рассвет
    sec_1   = hour_1[ntp.week] * 3600 + min_1[ntp.week] * 60;  // сек ДО Рассвет
    sec_2   = hour_2[ntp.week] * 3600 + min_2[ntp.week] * 60;  // сек ОТ Закат
    sec_3   = hour_3[ntp.week] * 3600 + min_3[ntp.week] * 60;  // сек ДО Закат
    switch (s_mode_led) {
      case 0:
        if (sec_now < sec_0 || sec_now > sec_3) { //до рассвета и после заката
          led_bright = option.max_n_perc;
          LED_value = max_night;
        } else if (sec_now >= sec_0 && sec_now <= sec_1) {    //Рассвет
          LED_value = map(sec_now, sec_0, sec_1, max_night, max_day);
          led_bright = map(sec_now, sec_0, sec_1, option.max_n_perc, option.max_d_perc);
        } else if (sec_now >= sec_2 && sec_now <= sec_3) {   //Закат
          LED_value = map(sec_now, sec_2, sec_3, max_day, max_night);
          led_bright = map(sec_now, sec_2, sec_3, option.max_d_perc, option.max_n_perc);
        } else if (sec_now > sec_1 && sec_now < sec_2) { //после рассвета и до заката
          led_bright = option.max_d_perc;
          LED_value = max_day;
        }
        analogWrite(LEDPIN, LED_value);
        break;
      case 1:
        led_bright = 100;
        analogWrite(LEDPIN, 1024);
        break;
      case 2:
        led_bright = 0;
        analogWrite(LEDPIN, 0);
        break;
    }
  }
}
void rele_function(byte s_m, byte n) {
  switch (s_m) {
    case 0:
      bf = (n == 3) ? (sec_now < sec_0 || sec_now > sec_1) ? bitClear(bf, n) : (sec_now >= sec_0 && sec_now <= sec_1) ? bitSet(bf, n) : bf :
           (sec_now < sec_2 || sec_now > sec_3) ? bitClear(bf, n) : (sec_now >= sec_2 && sec_now <= sec_3) ? bitSet(bf, n) : bf;
      break;
    case 1:
      bitSet(bf, n);
      break;
    case 2:
      bitClear(bf, n);
      break;
  }
}
void relay_schedule() {
  if (schedule[1][ntp.week][0] != "" || schedule[1][ntp.week][1] != "" || schedule[1][ntp.week][2] != "" || schedule[1][ntp.week][3] != "") {
    sscanf(schedule[1][ntp.week][0].c_str(), "%d:%d", &hour_0[ntp.week], &min_0[ntp.week]);
    sscanf(schedule[1][ntp.week][1].c_str(), "%d:%d", &hour_1[ntp.week], &min_1[ntp.week]);
    sscanf(schedule[1][ntp.week][2].c_str(), "%d:%d", &hour_2[ntp.week], &min_2[ntp.week]);
    sscanf(schedule[1][ntp.week][3].c_str(), "%d:%d", &hour_3[ntp.week], &min_3[ntp.week]);
    sec_0 = hour_0[ntp.week] * 3600 + min_0[ntp.week] * 60;
    sec_1 = hour_1[ntp.week] * 3600 + min_1[ntp.week] * 60;
    sec_2 = hour_2[ntp.week] * 3600 + min_2[ntp.week] * 60;
    sec_3 = hour_3[ntp.week] * 3600 + min_3[ntp.week] * 60;

    rele_function(s_mode_r1, 3);
    rele_function(s_mode_r2, 2);
    digitalOutput(RELAY1PIN, 3);
    digitalOutput(RELAY2PIN, 2);
    bitWrite(bf_rel, 0, bitRead(bf, 3));
    bitWrite(bf_rel, 1, bitRead(bf, 2));
  }
}
void update_chart_values() {
  int ostatok = rtc_now.hour() % 2;
  if (!ostatok && !bitRead(aux_bf, 2)) {
    int point = rtc_now.hour() / 2;
    configChart  = readFile("/configChart.json", 2048);
    jsonWrite(configChart, "days", rtc_now.day(), point, option.tempC);
    saveConfig("/configChart.json", configChart);
    configChart = "";
    bitSet(aux_bf, 2);
  } else if (ostatok) {
    bitClear(aux_bf, 2);
  }
}
void temp_fan_regulation(float t) {
  bf = (t >= option.fan_start) ? bitSet(bf, 5) : bf;
  bf = (t <= option.fan_stop) ? bitClear(bf, 5) : bf;
  digitalOutput(FANPIN, 5);
}
void temp_ten_regulation(float t) {
  bf = (t <= option.ten_start) ? bitSet(bf, 4) : bf;
  bf = (t >= option.ten_stop) ? bitClear(bf, 4) : bf;
  digitalOutput(TENPIN, 4);
}
void initOTA() {
  ArduinoOTA.setPort(8266);
  ArduinoOTA.onStart([]() {
    isUpdating = true;
    DEBUG_PRINT("Start OTA ");
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_PRINT("\nEnd OTA");
    isUpdating = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    isUpdating = true;
    perc = progress * 100 / total;
    initBar2();
    snprintf(line1, 17, "%-8i (%3i %%)", progress, perc);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    fillBar2(0, 1, 16, perc);
    DEBUG_PRINT("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    isUpdating = false;  
    DEBUG_PRINT("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) DEBUG_PRINT("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DEBUG_PRINT("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DEBUG_PRINT("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINT("Receive Failed");
    else if (error == OTA_END_ERROR) DEBUG_PRINT("End Failed");
  });
  ArduinoOTA.begin();
}