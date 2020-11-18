void readDateTime() {
  DateTime now = rtc.now();
  nowisValid = now.isValid();
  if (!nowisValid) {
    if (nowisValid != old_valid) {  //при изменении ошибки, выводит 1 раз сообщение на ком-порт
      Serial.printf("[%s]: %2.1fC | LED_value:%i\n", dt_now.c_str(), tempC, LED_value);
      old_valid = nowisValid;
    }
    if (date_error < 10) {
      date_error++;
    } else {
      lcd_hour = 0;
      lcd_min = 0;
      dt_now = "";
      led_bright = 0;
      relay1_working = 0;
      relay2_working = 0;
      analogWrite(LEDPIN, 0);
      digitalWrite(RELAY1PIN, relay1_working);
      digitalWrite(RELAY2PIN, relay2_working);
    }
  } else {
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
    bitWrite(bf, 2, relay2_working);
    bitWrite(bf, 3, relay1_working);
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
      fan_working = 0;
      ten_working = 0;
      digitalWrite(FANPIN, fan_working);
      digitalWrite(TENPIN, ten_working);
    }
  } else {
    temp_error = 0;
    tempC = round(t * 10) / 10.0 + temp_koef;
    sprintf(temp_char, "%2.1f\2C", tempC);
    bitWrite(bf, 4, ten_working);
    bitWrite(bf, 5, fan_working);
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
    updateZnak(4, fan_working, 13, 1);
    updateZnak(5, ten_working, 14, 1);
    updateZnak(6, ws_working,  15, 1);
    bitWrite(bf, 1, mqtt_working);
  }
}

void readJsonValues() {
  jsonWrite(configSetup, "ver", ver);
  saveConfigSetup();
  temp_koef = jsonReadToFloat(configSetup, "input", 1, 0);
  max_day_percent   = jsonReadToInt(configSetup, "input", 1, 1);
  max_night_percent = jsonReadToInt(configSetup, "input", 1, 2);
  fan_start = jsonReadToFloat(configSetup, "input", 1, 3);
  fan_stop  = jsonReadToFloat(configSetup, "input", 1, 4);
  ten_stop  = jsonReadToFloat(configSetup, "input", 1, 5);
  ten_start = jsonReadToFloat(configSetup, "input", 1, 6);
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
  defLang    = jsonReadToStr(configSetup, "defaultLang");
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
        analogWrite(LEDPIN, 1023);
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
    bitWrite(bf_rel, 0, relay1_working);

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
    bitWrite(bf_rel, 1, relay2_working);
  }
}

void temp_fan_regulation(float t) {
  if (t >= fan_start) fan_working = 1;
  if (t <= fan_stop) fan_working = 0;
  digitalWrite(FANPIN, fan_working);
}

void temp_ten_regulation(float t) {
  if (t <= ten_start) ten_working = 1;
  if (t >= ten_stop) ten_working = 0;
  digitalWrite(TENPIN, ten_working);
}

void update_chart_values() {
  int ostatok = ds_hour % 2;
  if (ostatok == 0 && chartFlag == 0 && tempC != 85.0 || tempC != -127.0) {
    int point = ds_hour / 2;
    jsonWrite(configChart, "days", ds_day, point, tempC);
    saveConfigChart();
    chartFlag = 1;
  } else if (ostatok != 0) {
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
