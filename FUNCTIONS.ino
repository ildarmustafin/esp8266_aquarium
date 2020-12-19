/* bf pins
  7  6  5  4  3  2  1  0
  |  |  |  |  |  |  |  |
  |  |  |  |  |  |  |  now_is_Valid
  |  |  |  |  |  |  MQTT_connected
  |  |  |  |  |  RELAY_2
  |  |  |  |  RELAY_1
  |  |  |  TEN
  |  |  FAN
  |  EMPTY
  EMPTY
*/

void readDateTime() {
  s_mode_r1  = jsonReadToInt(configSetup, "s_mode", 1);
  s_mode_r2  = jsonReadToInt(configSetup, "s_mode", 2);
  DateTime now = rtc.now();
  nowisValid = now.isValid();
  if (!nowisValid) {
    if (nowisValid != old_valid) {
      Serial.printf("[%s]: %2.1fC | LED_value:%i\n", dt_now.c_str(), tempC, LED_value);
      old_valid = nowisValid;
    }
    if (date_error < 10) {
      date_error++;
    } else {
      lcd_hour = 0;
      lcd_min = 0;
      //dt_now = "";
      dt_now = "2020-12-12T09:15:40";
      led_bright = 0;
      bitSet(bf, 0);
      //bitClear(bf, 0);
      analogWrite(LEDPIN, 0);
      bitClear(bf, 3);
      bitClear(bf, 2);
      digitalOutput(RELAY1PIN, 3);
      digitalOutput(RELAY2PIN, 2);
    }
  } else {
    bitSet(bf, 0);
    dt_now = now.timestamp();
    ds_day = now.day();
    ds_month = now.month();
    ds_year = now.year();
    ds_hour = now.hour();
    ds_min = now.minute();
    ds_sec = now.second();
    nedelya = dotw[now.dayOfTheWeek()].toInt();
    switch (nedelya) {
      case 0:
        lcd.createChar(7, znak_P);
        break;
      case 3:
        lcd.createChar(7, znak_Ch);
        break;
      case 4:
        lcd.createChar(7, znak_P);
        break;
      case 5:
        lcd.createChar(7, znak_B);
        break;
    }
    lcd_hour = ds_hour;
    lcd_min = ds_min;
    led_schedule();
    relay_schedule();
    update_chart_values();
  }
}

void readTemp() {
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (t == -127.0 || t == 85.0) {
    if (t != old_temp) {
      Serial.printf("[%s]: Temp:%2.1fC\n", dt_now.c_str(), t);
      old_temp = t;
    }
    if (temp_error < waitForTemp) {
      temp_error++;
    } else {
      tempC = round(t * 10) / 10.0 + temp_koef;
      sprintf(temp_char, "--.-\2C");
      bitClear(bf, 5);
      bitClear(bf, 4);
      digitalOutput(FANPIN, 5);
      digitalOutput(TENPIN, 4);
    }
  } else {
    temp_error = 0;
    tempC = round(t * 10) / 10.0 + temp_koef;
    sprintf(temp_char, "%2.1f\2C", tempC);
    temp_fan_regulation(tempC);
    temp_ten_regulation(tempC);
  }
}

void measure_datetime() {
  readDateTime();
  readTemp();
  if (wifi_working < 2) {
    char led_bright_char[5];
    char* weekLang = (defLang == "RUS") ? daysOfTheWeekRUS[nedelya] : daysOfTheWeekENG[nedelya];
    sprintf(led_bright_char, "%d%%", led_bright);
    sprintf(line1, "%02d:%02d %s %-7s", lcd_hour, lcd_min, weekLang, temp_char);
    sprintf(line2, "\1%-5d\3%-5s", WiFi.RSSI(), led_bright_char);
    printLCD(2, 0, line1, line2);
    switch (bf_rel) {
      case 0:
        updateZnak(8, 0, 12, 1);
        break;
      case 2:
        lcd.createChar(8, znak_rel2);
        updateZnak(8, 1, 12, 1);
        break;
      case 1:
        lcd.createChar(8, znak_rel1);
        updateZnak(8, 1, 12, 1);
        break;
      case 3:
        lcd.createChar(8, znak_rel12);
        updateZnak(8, 1, 12, 1);
        break;
    }
    updateZnak(4, bitRead(bf, 5), 13, 1);
    updateZnak(5, bitRead(bf, 4), 14, 1);
    updateZnak(6, ws_working,  15, 1);
  }
}

void readJsonValues() {
  jsonWrite(configSetup, "ver", 0, fwver);
  saveConfigSetup();
  fsver = jsonReadToStr(configSetup, "ver", 1);
  temp_koef = jsonReadToFloat(configSetup, "input", 1, 0);
  max_day_percent   = jsonReadToInt(configSetup, "input", 1, 1);
  max_night_percent = jsonReadToInt(configSetup, "input", 1, 2);
  fan_start = jsonReadToFloat(configSetup, "input", 1, 3);
  fan_stop  = jsonReadToFloat(configSetup, "input", 1, 4);
  ten_stop  = jsonReadToFloat(configSetup, "input", 1, 5);
  ten_start = jsonReadToFloat(configSetup, "input", 1, 6);
  chk_inverse = jsonReadToInt(configSetup, "input", 1, 7);
  mqtt_server   = jsonReadToStr(configSetup, "input", 2, 0);
  mqtt_server_ip.fromString(mqtt_server);
  mqtt_port     = jsonReadToStr(configSetup, "input", 2, 1);
  mqtt_ID       = jsonReadToStr(configSetup, "input", 2, 2);
  mqtt_user     = jsonReadToStr(configSetup, "input", 2, 3);
  mqtt_password = jsonReadToStr(configSetup, "input", 2, 4);
  mqtt_topic    = jsonReadToStr(configSetup, "input", 2, 5);
  timeZone      = jsonReadToInt(configSetup, "input", 3, 2);
  ntpServerName = jsonReadToStr(configSetup, "input", 3, 3).c_str();
  ssidAP       = jsonReadToStr(configSetup, "input", 3, 4);
  passwordAP   = jsonReadToStr(configSetup, "input", 3, 5);
  ssid         = jsonReadToStr(configSetup, "input", 3, 6);
  password     = jsonReadToStr(configSetup, "input", 3, 7);
  ip_str     = jsonReadToStr(configSetup, "input", 3, 8);
  ip.fromString(ip_str);
  ip_gw_str  = jsonReadToStr(configSetup, "input", 3, 9);
  ip_gw.fromString(ip_gw_str);
  port       = jsonReadToInt(configSetup, "input", 3, 10);
  defLang    = jsonReadToStr(configSetup, "defaultLang").c_str();
  s_mode_led = jsonReadToInt(configSetup, "s_mode", 0);
  s_mode_r1  = jsonReadToInt(configSetup, "s_mode", 1);
  s_mode_r2  = jsonReadToInt(configSetup, "s_mode", 2);
  max_day    = max_day_percent * 1024 / 100;
  max_night  = max_night_percent * 1024 / 100;
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
        analogWrite(LEDPIN, 1024);
        break;
      case 2:
        led_bright = 0;
        analogWrite(LEDPIN, 0);
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
    sec_0 = hour_0[nedelya] * 3600 + min_0[nedelya] * 60;
    sec_1 = hour_1[nedelya] * 3600 + min_1[nedelya] * 60;
    sec_2 = hour_2[nedelya] * 3600 + min_2[nedelya] * 60;
    sec_3 = hour_3[nedelya] * 3600 + min_3[nedelya] * 60;
    sec_now  = ds_hour * 3600 + ds_min * 60 + ds_sec;
    rele_function(s_mode_r1, 3);
    rele_function(s_mode_r2, 2);
    digitalOutput(RELAY1PIN, 3);
    digitalOutput(RELAY2PIN, 2);
    bitWrite(bf_rel, 0, bitRead(bf, 3));
    bitWrite(bf_rel, 1, bitRead(bf, 2));
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

void temp_fan_regulation(float t) {
  bf = (t >= fan_start) ? bitSet(bf, 5) : bf;
  bf = (t <= fan_stop) ? bitClear(bf, 5) : bf;
  digitalOutput(FANPIN, 5);
}

void temp_ten_regulation(float t) {
  bf = (t <= ten_start) ? bitSet(bf, 4) : bf;
  bf = (t >= ten_stop) ? bitClear(bf, 4) : bf;
  digitalOutput(TENPIN, 4);
}

void digitalOutput(byte pin, byte bitNumber) {
  if (chk_inverse) {
    digitalWrite(pin, !bitRead(bf, bitNumber));
  } else {
    digitalWrite(pin, bitRead(bf, bitNumber));
  }
}

void update_chart_values() {
  int ostatok = ds_hour % 2;
  //if (ostatok == 0 && chartFlag == 0 && tempC != 85.0 || tempC != -127.0) {
  //if (!ostatok && !chartFlag && tempC != 85.0 || tempC != -127.0) {
  if (!ostatok && !chartFlag) {
    int point = ds_hour / 2;
    jsonWrite(configChart, "days", ds_day, point, tempC);
    saveConfigChart();
    chartFlag = 1;
  } else if (ostatok) {
    chartFlag = 0;
  }
}

void restart_esp() {
  Serial.println("RESTARTING ESP8266. PLEASE WAIT...");
  printLCD(2, 0, "   RESTARTING   ", "PLEASE WAIT...  ");
  delay(1500);
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  ESP.restart();
}

void updateTimeNTP() {
  now = time(nullptr);
  struct tm * tm = localtime(&now);
  if (tm->tm_year == 70) {
    isSyncOK = 0;
  } else {
    isSyncOK = 1;
    days = tm->tm_mday;
    month = tm->tm_mon + 1;
    year = tm->tm_year + 1900;
    hours = tm->tm_hour;
    minutes = tm->tm_min;
    seconds = tm->tm_sec;
  }
}
