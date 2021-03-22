void readJsonValues()
{
  jsonWrite(configSetup, "ver", 0, ver.fw);
  saveConfig("/configSetup.json", configSetup);
  DynamicJsonDocument root(4096);
  deserializeJson(root, configSetup);
  strcpy(ver.fs, root["ver"][1].as<const char *>());       
  strcpy(defLang, root["defaultLang"].as<const char *>()); 
  timeZone = root["input"][3][2].as<signed int>();
  strcpy(wifi.ssidAP, root["input"][3][3].as<const char *>());     
  strcpy(wifi.passwordAP, root["input"][3][4].as<const char *>()); 
  strcpy(wifi.ssid, root["input"][3][5].as<const char *>());       
  strcpy(wifi.password, root["input"][3][6].as<const char *>());   
  strcpy(wifi.ip_str, root["input"][3][7].as<const char *>());     
  strcpy(wifi.ip_gw_str, root["input"][3][8].as<const char *>());  
  strcpy(mqtt.server, root["input"][2][0].as<const char *>());     
  mqtt.port = root["input"][2][1].as<signed int>();
  strcpy(mqtt.ID, root["input"][2][2].as<const char *>());       
  strcpy(mqtt.user, root["input"][2][3].as<const char *>());     
  strcpy(mqtt.password, root["input"][2][4].as<const char *>()); 
  strcpy(mqtt.t_all, root["input"][2][5].as<const char *>());    
  strcpy(mqtt.t_temp, root["input"][2][6].as<const char *>());    
  strcpy(mqtt.t_led, root["input"][2][7].as<const char *>());  
  strcpy(mqtt.t_fan, root["input"][2][8].as<const char *>());  
  strcpy(mqtt.t_ten, root["input"][2][9].as<const char *>());  
  strcpy(mqtt.t_rel1, root["input"][2][10].as<const char *>());  
  strcpy(mqtt.t_rel2, root["input"][2][11].as<const char *>());  
  strcpy(mqtt.t_date, root["input"][2][12].as<const char *>());            
  option.temp_koef = root["input"][1][0].as<float>();
  option.max_d_perc = root["input"][1][1].as<signed int>();
  option.max_n_perc = root["input"][1][2].as<signed int>();
  fan_reg.setpoint = root["input"][1][3].as<float>();   // установка градусов
  fan_reg.hysteresis = root["input"][1][4].as<float>(); // ширина гистерезиса
  fan_reg.k = root["input"][1][5].as<float>();          // коэффициент обратной связи (подбирается по факту)
  ten_reg.setpoint = root["input"][1][6].as<float>();   // установка градусов
  ten_reg.hysteresis = root["input"][1][7].as<float>(); // ширина гистерезиса
  ten_reg.k = root["input"][1][8].as<float>();          // коэффициент обратной связи (подбирается по факту)
  option.delLed = root["input"][1][9].as<signed int>();
  state.inv = root["input"][1][10].as<signed int>();
  s_mode_led = root["s_mode"][0].as<signed int>();
  s_mode_r1 = root["s_mode"][1].as<signed int>();
  s_mode_r2 = root["s_mode"][2].as<signed int>();

  (option.delLed) ? lcd.noBacklight() : lcd.backlight();
  mqtt.server_ip.fromString(mqtt.server);
  wifi.ip.fromString(wifi.ip_str);
  wifi.ip_gw.fromString(wifi.ip_gw_str);
  max_day = option.max_d_perc * 4096 / 100;
  max_night = option.max_n_perc * 4096 / 100;
  mqttClient.disconnect();
  mqttClient.setServer(mqtt.server_ip, mqtt.port);
  mqttClient.connect();
  for (int a = 0; a <= 6; a++)
  {
    for (int b = 0; b <= 3; b++)
    {
      strcpy(sched_led[a][b], root["schedule"][0][a][b].as<const char *>());
      strcpy(sched_rel[a][b], root["schedule"][1][a][b].as<const char *>());
    }
  }
}
void updateTimeNTP()
{
  ntp.now = time(nullptr);
  struct tm *tm = localtime(&ntp.now);
  if (tm->tm_year == 70)
  {
    state.valSync = 0;
  }
  else
  {
    state.valSync = 1;
    ntp.day = tm->tm_mday;
    ntp.month = tm->tm_mon + 1;
    ntp.year = tm->tm_year + 1900;
    ntp.hour = tm->tm_hour;
    ntp.min = tm->tm_min;
    ntp.sec = tm->tm_sec;
  }
}
void restart_esp()
{
  Serial.println(F("RESTARTING ESP8266. PLEASE WAIT..."));
  printLCD(2, PSTR("   RESTARTING   "), PSTR("PLEASE WAIT...  "));
  delay(2000);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  ESP.restart();
}
void led_schedule()
{
  if (strcmp(sched_led[ntp.week][0], "") || strcmp(sched_led[ntp.week][1], "") || strcmp(sched_led[ntp.week][2], "") || strcmp(sched_led[ntp.week][3], ""))
  {
    sscanf(sched_led[ntp.week][0], "%d:%d", &hour_0[ntp.week], &min_0[ntp.week]);
    sscanf(sched_led[ntp.week][1], "%d:%d", &hour_1[ntp.week], &min_1[ntp.week]);
    sscanf(sched_led[ntp.week][2], "%d:%d", &hour_2[ntp.week], &min_2[ntp.week]);
    sscanf(sched_led[ntp.week][3], "%d:%d", &hour_3[ntp.week], &min_3[ntp.week]);
    sec_0 = hour_0[ntp.week] * 3600 + min_0[ntp.week] * 60; // сек ОТ Рассвет
    sec_1 = hour_1[ntp.week] * 3600 + min_1[ntp.week] * 60; // сек ДО Рассвет
    sec_2 = hour_2[ntp.week] * 3600 + min_2[ntp.week] * 60; // сек ОТ Закат
    sec_3 = hour_3[ntp.week] * 3600 + min_3[ntp.week] * 60; // сек ДО Закат
    switch (s_mode_led)
    {
    case 0:
      if (sec_now < sec_0 || sec_now > sec_3)
      { //до рассвета и после заката
        state.led = option.max_n_perc;
        LED_value = max_night;
      }
      else if (sec_now >= sec_0 && sec_now <= sec_1)
      { //Рассвет
        LED_value = map(sec_now, sec_0, sec_1, max_night, max_day);
        state.led = map(sec_now, sec_0, sec_1, option.max_n_perc, option.max_d_perc);
      }
      else if (sec_now >= sec_2 && sec_now <= sec_3)
      { //Закат
        LED_value = map(sec_now, sec_2, sec_3, max_day, max_night);
        state.led = map(sec_now, sec_2, sec_3, option.max_d_perc, option.max_n_perc);
      }
      else if (sec_now > sec_1 && sec_now < sec_2)
      { //после рассвета и до заката
        state.led = option.max_d_perc;
        LED_value = max_day;
      }
      pwmController.setChannelPWM(LEDPIN, LED_value);
      break;
    case 1:
      state.led = 100;
      pwmController.setChannelOn(LEDPIN);
      break;
    case 2:
      state.led = 0;
      pwmController.setChannelOff(LEDPIN);
      break;
    }
  }
}
byte rele_function(long s0, long s1, byte s_m)
{
  byte rel = 0;
  switch (s_m)
  {
  case 0:
    if (sec_now < s0 || sec_now > s1)
      rel = 0; //до ВКЛ и после ВЫКЛ
    else if (sec_now >= s0 && sec_now <= s1)
      rel = 1; //между ВКЛ-ВЫКЛ
    break;
  case 1:
    rel = 1;
    break;
  case 2:
    rel = 0;
    break;
  }
  return rel;
}
void relay_schedule()
{

  if (strcmp(sched_rel[ntp.week][0], "") || strcmp(sched_rel[ntp.week][1], "") || strcmp(sched_rel[ntp.week][2], "") || strcmp(sched_rel[ntp.week][3], ""))
  {
    sscanf(sched_rel[ntp.week][0], "%d:%d", &hour_0[ntp.week], &min_0[ntp.week]);
    sscanf(sched_rel[ntp.week][1], "%d:%d", &hour_1[ntp.week], &min_1[ntp.week]);
    sscanf(sched_rel[ntp.week][2], "%d:%d", &hour_2[ntp.week], &min_2[ntp.week]);
    sscanf(sched_rel[ntp.week][3], "%d:%d", &hour_3[ntp.week], &min_3[ntp.week]);
    sec_0 = hour_0[ntp.week] * 3600 + min_0[ntp.week] * 60;
    sec_1 = hour_1[ntp.week] * 3600 + min_1[ntp.week] * 60;
    sec_2 = hour_2[ntp.week] * 3600 + min_2[ntp.week] * 60;
    sec_3 = hour_3[ntp.week] * 3600 + min_3[ntp.week] * 60;

    state.rel1 = rele_function(sec_0, sec_1, s_mode_r1); //определение состояния rel1 с инверсией
    state.rel2 = rele_function(sec_2, sec_3, s_mode_r2); //определение состояния rel2 с инверсией
    oldState.rel1 = sendOnChange("rel1", mqtt.t_rel1, state.rel1, oldState.rel1);
    oldState.rel2 = sendOnChange("rel2", mqtt.t_rel2, state.rel2, oldState.rel2);
    bitWrite(bf_rel, 0, state.rel1);                        //Запись бит в bf_rel для вывода символа на LCD1602
    bitWrite(bf_rel, 1, state.rel2);                        //Запись бит в bf_rel для вывода символа на LCD1602
    byte PWMrel1 = (state.inv) ? !state.rel1 : state.rel1;  //Определение наличия инверсии
    byte PWMrel2 = (state.inv) ? !state.rel2 : state.rel2;  //Определение наличия инверсии
    pwmController.setChannelPWM(RELAY1PIN, PWMrel1 * 4096); //передача состояния rel1 на pca9685
    pwmController.setChannelPWM(RELAY2PIN, PWMrel2 * 4096); //передача состояния rel2 на pca9685
  }
}
void update_chart_values()
{
  int ostatok = rtc.now().hour() % 2; //Определение 2 часа
  if (!ostatok && !flag.chart)        //если 2 часа и chartFlag
  {
    int point = rtc.now().hour() / 2;
    configChart = readFile("/configChart.json", 2048);
    jsonWrite(configChart, "days", rtc.now().day(), point, option.tempC);
    saveConfig("/configChart.json", configChart);
    configChart = "";
    flag.chart = 1;
    events.send(rtc.now().timestamp().c_str(), "now", millis());
  }
  else if (ostatok)
  {
    flag.chart = 0;
  }
}
void temp_fan_regulation(float t)
{
  state.fan = (state.inv) ? !fan_reg.getResult() : fan_reg.getResult();
  oldState.fan = sendOnChange("fan", mqtt.t_fan, fan_reg.getResult(), oldState.fan);
  pwmController.setChannelPWM(FANPIN, state.fan * 4096);
}
void temp_ten_regulation(float t)
{
  state.ten = (state.inv) ? !ten_reg.getResult() : ten_reg.getResult();
  oldState.ten = sendOnChange("ten", mqtt.t_ten, ten_reg.getResult(), oldState.ten);
  pwmController.setChannelPWM(TENPIN, state.ten * 4096);
}
void initOTA()
{
  ArduinoOTA.setPort(8266);
  ArduinoOTA.onStart([]() {
    isUpdating = true;
    Serial.println(F("[OTA] Start OTA"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println();
    Serial.println(F("[OTA] End OTA"));
    isUpdating = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    isUpdating = true;
    state.perc = progress * 100 / total;
    initBar2();
    snprintf(line1, 17, "%-8i (%3i %%)", progress, state.perc);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    fillBar2(0, 1, 16, state.perc);
    Serial.print(F("[OTA] Progress: "));
    Serial.print((progress / (total / 100)));
    Serial.println(F(" %"));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    isUpdating = false;
    //Serial.print("[OTA] Error[%u]: ", error);
    //if (error == OTA_AUTH_ERROR)
      //Serial.print("[OTA] Auth Failed");
    //else if (error == OTA_BEGIN_ERROR)
      //Serial.print("[OTA] Begin Failed");
    //else if (error == OTA_CONNECT_ERROR)
      //Serial.print("[OTA] Connect Failed");
    //else if (error == OTA_RECEIVE_ERROR)
      //Serial.print("[OTA] Receive Failed");
    //else if (error == OTA_END_ERROR)
      //Serial.print("[OTA] End Failed");
  });
  ArduinoOTA.begin();
}