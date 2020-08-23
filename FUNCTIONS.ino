void check_connection() {
  if (triesCon-- && WiFi.status() != WL_CONNECTED && wifi_working)  {
    Serial.printf("LOST CONNECTION...WAITING FOR: %i\n", triesCon);
    lcd.createChar(1, znak_wifi_tc);
    if (!triesCon) {
      StartAPMode();
    }
  } else {
    lcd.createChar(1, znak_wifi_sta);
    triesCon = 21;
  }

  if (CheckDelay-- && !wifi_working) {
    Serial.printf("RECONNECT AFTER: %i\n", CheckDelay);
    if (!CheckDelay) {
      int rt = WiFi.scanNetworks();
      Serial.println("SCANNING DONE...");
      for (int i = 0; i < rt; ++i) {
        if (rt != 0 && WiFi.SSID(i) == _ssid) {
          Serial.printf("\nFOUND \"%s\"\nCONNECTING...PLEASE WAIT...\n", _ssid.c_str());
          delay(100);
          initSTA();
        }
      }
      CheckDelay = n;
    }
  } else {
    CheckDelay = n;
  }
}

void measure_datetime() {
  if (Terr >= 1000) Terr = 0;
  rssi =  WiFi.RSSI();
  DateTime now = rtc.now();
  isRTCconnected = now.isValid();
  bitWrite(bitFlags, 6, isRTCconnected);
  if (isRTCconnected == 0) {
    Terr++;
    sprintf(logWrite, "[--DATE ERROR--] temp:%2.1fC\n", temp_filtered);
    Serial.printf("[--DATE ERROR--] temp:%2.1fC\n", temp_filtered);
    configLog = logWrite;
    saveLog();
    ds_hour = 0;
    ds_min = 0;
    led_bright = 0;
    relay1_working = 0;
    relay2_working = 0;
  } else {
    dt_now = now.timestamp();
    ds_day = now.day();
    ds_month = now.month();
    ds_year = now.year();
    ds_hour = now.hour();
    ds_min = now.minute();
    ds_sec = now.second();
    nedelya = dotw[now.dayOfTheWeek()].toInt();
    led_schedule();
    relay_schedule();
  }
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  temp_filtered = round(tempC * 10) / 10.0;
  if (temp_filtered != -127.0 || temp_filtered != 85.0) {
    sprintf(line1, "%02i:%02i %s %2.1f\2C",  ds_hour, ds_min, daysOfTheWeek[nedelya], temp_filtered);
    temp_fan_regulation(temp_filtered);
    temp_ten_regulation(temp_filtered);
  } else {
    sprintf(line1, "%02i:%02i %s --ERR--", ds_hour, ds_min, daysOfTheWeek[nedelya]);
    Terr++;
    sprintf(logWrite, "[%s] temp:%2.1fC | led_bright:%i\n", dt_now.c_str(), temp_filtered, led_bright);
    Serial.printf("[%s] temp:%2.1fC | led_bright:%i\n", dt_now.c_str(), temp_filtered, led_bright);
    configLog = logWrite;
    saveLog();
  }
  sprintf(line2_1, "\1%i", rssi);
  sprintf(line2_2, "\3%i", led_bright);
  printLCD(2, 0, 0, convertValue(line1, 16), convertValue(line2_1, 5), 0);
  printLCD(1, 0, 6, "", convertValue(line2_2, 4), 0);

  updateZnak(4, fan_working,    13, 1);
  updateZnak(5, ten_working,    14, 1);
  updateZnak(6, ws_working,     15, 1);
  updateZnak(7, relay1_working, 11, 1);
  updateZnak(8, relay2_working, 12, 1);
  bitWrite(bitFlags, 1, mqtt_working);
  bitWrite(bitFlags, 2, relay2_working);
  bitWrite(bitFlags, 3, relay1_working);
  bitWrite(bitFlags, 4, ten_working);
  bitWrite(bitFlags, 5, fan_working);
  update_chart_values();
}

void readJsonValues() {
  jsonWrite(configSetup, "ver", ver);
  saveConfigSetup();
  temp_koef = jsonReadToFloat(configSetup, "input", 1, 0);
  max_day_percent   = jsonReadToInt(configSetup,   "input", 1, 1);
  max_night_percent = jsonReadToInt(configSetup,   "input", 1, 2);
  fan_start = jsonReadToFloat(configSetup, "input", 1, 3);
  fan_stop  = jsonReadToFloat(configSetup, "input", 1, 4);
  ten_stop  = jsonReadToFloat(configSetup, "input", 1, 5);
  ten_start = jsonReadToFloat(configSetup, "input", 1, 6);
  mqtt_server   = jsonReadToStr(configSetup, "input", 2, 0);
  mqtt_port     = jsonReadToInt(configSetup, "input", 2, 1);
  mqtt_ID       = jsonReadToStr(configSetup, "input", 2, 2);
  mqtt_user     = jsonReadToStr(configSetup, "input", 2, 3);
  mqtt_password = jsonReadToStr(configSetup, "input", 2, 4);
  mqtt_topic    = jsonReadToStr(configSetup, "input", 2, 5);

  timeZone      = jsonReadToInt(configSetup, "input", 3, 2);
  ntpServerName = jsonReadToStr(configSetup, "input", 3, 3).c_str();
  _ssidAP       = jsonReadToStr(configSetup, "input", 3, 4);
  _passwordAP   = jsonReadToStr(configSetup, "input", 3, 5);
  _ssid         = jsonReadToStr(configSetup, "input", 3, 6);
  _password     = jsonReadToStr(configSetup, "input", 3, 7);
  ip.fromString(jsonReadToStr(configSetup,   "input", 3, 8).c_str());
  ip_gw.fromString(jsonReadToStr(configSetup, "input", 3, 9).c_str());
  port          = jsonReadToInt(configSetup, "input", 3, 10);
  CheckDelay    = jsonReadToInt(configSetup, "input", 3, 11);
  n = CheckDelay;

  s_mode_led = jsonReadToInt(configSetup, "s_mode", 0);
  s_mode_r1  = jsonReadToInt(configSetup, "s_mode", 1);
  s_mode_r2  = jsonReadToInt(configSetup, "s_mode", 2);
  max_day   = max_day_percent * maxPWM / 100;   // x = 100 * 1009 / 100 = 100
  max_night = max_night_percent * maxPWM / 100; // x = 0 * 1009 / 100 = 0
  for (int a = 0; a <= 1; a++) {
    for (int b = 0; b <= 6; b++) {
      for (int c = 0; c <= 3; c++) {
        schedule[a][b][c] = jsonReadToStr(configSetup, "schedule", a, b, c);
      }
    }
  }
}

void led_schedule() {
  if (schedule[0][nedelya][0] != "" || schedule[0][nedelya][1] != "" || schedule[0][nedelya][2] != "" || schedule[0][nedelya][3] != "") {
    sscanf(schedule[0][nedelya][0].c_str(), "%d:%d", &hour_0[nedelya], &min_0[nedelya]);
    sscanf(schedule[0][nedelya][1].c_str(), "%d:%d", &hour_1[nedelya], &min_1[nedelya]);
    sscanf(schedule[0][nedelya][2].c_str(), "%d:%d", &hour_2[nedelya], &min_2[nedelya]);
    sscanf(schedule[0][nedelya][3].c_str(), "%d:%d", &hour_3[nedelya], &min_3[nedelya]);
    sec_0   = hour_0[nedelya] * 3600 + min_0[nedelya] * 60;  // сек ОТ Рассвет
    sec_1   = hour_1[nedelya] * 3600 + min_1[nedelya] * 60;  // сек ДО Рассвет
    sec_2   = hour_2[nedelya] * 3600 + min_2[nedelya] * 60;  // сек ОТ Закат
    sec_3   = hour_3[nedelya] * 3600 + min_3[nedelya] * 60;  // сек ДО Закат
    sec_now = ds_hour * 3600 + ds_min * 60 + ds_sec;
    //=============================================================================================================
    switch (s_mode_led) {
      case 0:
        if (sec_now < sec_0 || sec_now > sec_3) { //до рассвета и после заката
          led_bright = max_night_percent;
          LED_value = max_night;
        } else if (sec_now >= sec_0 && sec_now <= sec_1) {    //Рассвет
          LED_value = map(sec_now, sec_0, sec_1, max_night, max_day);
          led_bright = map(sec_now, sec_0, sec_1, max_night_percent, max_day_percent);
        } else if (sec_now >= sec_2 && sec_now <= sec_3) {   //Закат
          LED_value = map(sec_now, sec_2, sec_3, max_day, max_night);
          led_bright = map(sec_now, sec_2, sec_3, max_day_percent, max_night_percent);
        } else if (sec_now > sec_1 && sec_now < sec_2) { //после рассвета и до заката
          led_bright = max_day_percent;
          LED_value = max_day;
        }
        analogWrite(LEDPIN, LED_value);
        break;
      case 1:
        led_bright = 100;
        digitalWrite(LEDPIN, HIGH);
        break;
      case 2:
        led_bright = 0;
        digitalWrite(LEDPIN, LOW);
        break;
    }
  }
}

void relay_schedule() {
  if (schedule[1][nedelya][0] != "" || schedule[1][nedelya][1] != "" || schedule[1][nedelya][2] != "" || schedule[1][nedelya][3] != "") {
    sscanf(schedule[1][nedelya][0].c_str(), "%d:%d", &hour_0[nedelya], &min_0[nedelya]);
    sscanf(schedule[1][nedelya][1].c_str(), "%d:%d", &hour_1[nedelya], &min_1[nedelya]);
    sscanf(schedule[1][nedelya][2].c_str(), "%d:%d", &hour_2[nedelya], &min_2[nedelya]);
    sscanf(schedule[1][nedelya][3].c_str(), "%d:%d", &hour_3[nedelya], &min_3[nedelya]);
    sec_0 = hour_0[nedelya] * 60 * 60 + min_0[nedelya] * 60;
    sec_1 = hour_1[nedelya] * 60 * 60 + min_1[nedelya] * 60;
    sec_2 = hour_2[nedelya] * 60 * 60 + min_2[nedelya] * 60;
    sec_3 = hour_3[nedelya] * 60 * 60 + min_3[nedelya] * 60;
    sec_now  = ds_hour * 60 * 60 + ds_min * 60 + ds_sec;
    switch (s_mode_r1) {
      case 0:
        if (sec_now < sec_0 || sec_now > sec_1) {
          relay1_working = 0;
        }
        if (sec_now >= sec_0 && sec_now <= sec_1) {
          relay1_working = 1;
        }
        break;
      case 1:
        relay1_working = 1;
        break;
      case 2:
        relay1_working = 0;
        break;
    }
    digitalWrite(RELAY1PIN, relay1_working);

    switch (s_mode_r2) {
      case 0:
        if (sec_now < sec_2 || sec_now > sec_3) {
          relay2_working = 0;
        }
        if (sec_now >= sec_2 && sec_now <= sec_3) {
          relay2_working = 1;
        }
        break;
      case 1:
        relay2_working = 1;
        break;
      case 2:
        relay2_working = 0;
        break;
    }
    digitalWrite(RELAY2PIN, relay2_working);
  }
}

void temp_fan_regulation(float t) {
  if (t != 0) {
    if (t >= fan_start) {
      fan_working = 1;
    } else if (t <= fan_stop) {
      fan_working = 0;
    }
  } else {
    fan_working = 0;
  }
  digitalWrite(FANPIN, fan_working);
}

void temp_ten_regulation(float t) {
  if (t != 0) {
    if (t <= ten_start) {
      ten_working = 1;
    } else if (t >= ten_stop) {
      ten_working = 0;
    }
  } else {
    ten_working = 0;
  }
  digitalWrite(TENPIN, ten_working);
}
