#include "main.h"

void ICACHE_RAM_ATTR inputButton();
void ICACHE_RAM_ATTR ipButton();
void readDateTime()
{
  state.valNow = rtc.now().isValid();
  if (!state.valNow)
  {
    if (state.valNow != oldState.valNow)
    {
      Serial.print(F("----.--.-- --:--:--]: Temp: "));
      Serial.print(option.tempC);
      Serial.print(F(" | LED_value: "));
      Serial.println(LED_value);
      oldState.valNow = state.valNow;
    }
    if (date_error < 10)
    {
      date_error++;
    }
    else
    {
      lcd_hour = 0;
      lcd_min = 0;
      lcd_sec = 0;      
      state.led = 0;
      state.rel1 = 0;
      state.rel2 = 0;
      pwmController.setChannelOff(LEDPIN);
      pwmController.setChannelOff(RELAY1PIN);
      pwmController.setChannelOff(RELAY2PIN);
    }
  }
  else
  {
    sec_now = rtc.now().hour() * 3600 + rtc.now().minute() * 60 + rtc.now().second();
    ntp.week = ntp.dayOfWeek[rtc.now().dayOfTheWeek()];
    switch (ntp.week)
    {
    case 0:
      lcd.createChar(7, z_P);
      break;
    case 3:
      lcd.createChar(7, z_Ch);
      break;
    case 4:
      lcd.createChar(7, z_P);
      break;
    case 5:
      lcd.createChar(7, z_B);
      break;
    }
    lcd_hour = rtc.now().hour();
    lcd_min = rtc.now().minute();
    lcd_sec = rtc.now().second();    
    led_schedule();
    relay_schedule();
    update_chart_values();
  }
}
void readTemp()
{
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (t == -127.0 || t == 85.0)
  {
    if (t != option.t_old)
    {
      Serial.print(F("["));
      if (!state.valNow)
        Serial.print(F("----.--.-- --:--:--"));
      else
        Serial.print(rtc.now().timestamp());
      Serial.print(F("]: Temp: "));
      Serial.println(t);
      option.t_old = t;
    }
    if (temp_error < waitForTemp)
    {
      temp_error++;
    }
    else
    {
      option.tempC = round(t * 10) / 10.0 + option.temp_koef;
      strcpy(option.tempC_char, "--.-\2C"); //7
      state.ten = 0;
      state.fan = 0;
      pwmController.setChannelOff(FANPIN);
      pwmController.setChannelOff(TENPIN);
    }
  }
  else
  {
    temp_error = 0;
    option.tempC = round(t * 10) / 10.0 + option.temp_koef;
    fan_reg.input = option.tempC;
    ten_reg.input = option.tempC;
    snprintf(option.tempC_char, 8, "%2.1f\2C", option.tempC);
    temp_fan_regulation(option.tempC);
    temp_ten_regulation(option.tempC);
  }
}
void measure_datetime()
{
  readDateTime();
  readTemp();
  state.heapCur = ESP.getFreeHeap();
  state.heapMin = (state.heapCur < state.heapMin) ? state.heapCur : state.heapMin;
  !strcmp(defLang, "RUS") ? strcpy(weekLang, daysOfTheWeekRUS[ntp.week]) : strcpy(weekLang, daysOfTheWeekENG[ntp.week]);
  if (state.heapMin != oldState.heapMin && !isUpdating)
  {
    events.send(String(state.heapMin).c_str(), "heap", millis());
    oldState.heapMin = state.heapMin;
  }
  //Serial.printf("MaxBlockSize: %i | HeapFragm: %i%% | heapCur: %i | heapMin: %i \n", ESP.getMaxFreeBlockSize(), ESP.getHeapFragmentation(), state.heapCur, state.heapMin);
  //Serial.print(F("heapMin: "));
  //Serial.println(state.heapMin);
  //Serial.print("line1: %s\n", line1);
  //Serial.print("line2: %s\n", line2);
  //Serial.print("state.wifi: %i\n", state.wifi);
  line_show();
}
void noLight()
{
  lcd.noBacklight();
  //Serial.printf("BUTTON FINISH: %i\n", option.delLed);
  flag.buttonPressed = 0;
}
void showIP()
{
  flag.ipPressed = 0;
}
void inputButton()
{
  if ((!flag.buttonPressed) && (option.delLed))
  {
    lcd.backlight();
    //Serial.printf("BUTTON START: %i\n", option.delLed);
    timer_once.once(option.delLed, noLight);
    flag.buttonPressed = 1;
  }
}
void ipButton()
{
  if (!flag.ipPressed)
  {
    timerIP_once.once(3, showIP);
    flag.ipPressed = 1;
  }
}
void send_data()
{
  if (isUpdating)
  {
    if (state.perc != oldState.perc)
    {
      events.send(String(state.perc).c_str(), "ota", millis());
      oldState.perc = state.perc;
    }
  }
  else
  {
    measure_datetime();
    pwmController.setChannelOn(DOP1LED);
    if (state.wifi == 1)
      wifi.rssi = WiFi.RSSI();
    flag.inc++;
    if (flag.inc >= 60)
    {
      oldState.rssi = sendOnChange("rssi", "rssi", wifi.rssi, oldState.rssi);    
      flag.inc = 0;
    }
    if (oldState.tempC != option.tempC && !isUpdating)
    {
      //Serial.print("[SSE] tempC: %2.1f\n", option.tempC);
      events.send(option.tempC_char, "temp", millis());
      if (mqttClient.connected())
        mqttClient.publish(mqtt.t_temp, 0, true, option.tempC_char);
      oldState.tempC = option.tempC;
    }
    oldState.led = sendOnChange("led", mqtt.t_led, state.led, oldState.led);
    if (state.ws)
      ws.textAll(sendJson());
    //ws.textAll(configLive);
    if (flag.serDebug)
    {
      if (str.indexOf("heap") > -1)
        Serial.printf("FRAGM: %i | CUR: %i | MIN: %i\n", ESP.getHeapFragmentation(), state.heapCur, state.heapMin);
      if (str.indexOf("led") > -1)
        Serial.printf("LED: %i | REL1: %i | REL2: %i\n", state.led, state.rel1, state.rel2);
      if (str.indexOf("temp") > -1)
        Serial.printf("TEMP: %2.1f | FAN: %i | TEN: %i\n", option.tempC, state.fan, state.ten);
      if (str.indexOf("wifi") > -1)
        Serial.printf("MODE: %i | STATE: %i | RSSI: %i\n", WiFi.getMode(), state.wifi, wifi.rssi);
    }
  }
}
void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(BUTPIN, INPUT);
  pinMode(IPPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTPIN), inputButton, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IPPIN), ipButton, CHANGE);
  initLCD();
  initOTA();
  initFileSystem();
  //Serial.print("[SETUP] INITIAL PIN STATES: %i\n", state.inv * 4096);
  pwmController.setChannelPWM(FANPIN, state.inv * 4096);
  pwmController.setChannelPWM(TENPIN, state.inv * 4096);
  pwmController.setChannelPWM(RELAY1PIN, state.inv * 4096);
  pwmController.setChannelPWM(RELAY2PIN, state.inv * 4096);
  pwmController.setChannelPWM(LEDPIN, state.inv * 4096);

  pwmController.setChannelPWM(FANLED, 4096 - state.inv * 4096);
  pwmController.setChannelPWM(TENLED, 4096 - state.inv * 4096);
  pwmController.setChannelPWM(RELAY1LED, 4096 - state.inv * 4096);
  pwmController.setChannelPWM(RELAY2LED, 4096 - state.inv * 4096);
  pwmController.setChannelPWM(LEDLED, 4096 - state.inv * 4096);
  pwmController.setChannelOn(DOP1LED);
  pwmController.setChannelOn(DOP2LED);

  snprintf(line2, 17, "FW:%s  FS:%s", ver.fw, ver.fs);
  printLCD(2, "  ESP8266_AQUA  ", line2);
  //WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3, WIFI_SHUTDOWN = 4, WIFI_RESUME = 8
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.hostname("ESP8266_AQUA");
  WiFi.setOutputPower(12); //0 - 20.5 dBm
  if (!WiFi.getAutoConnect())
    WiFi.setAutoConnect(true);
  if (!WiFi.getAutoReconnect())
    WiFi.setAutoReconnect(true);
  WiFi_initSTA();
  //WiFi_initAP_STA();
  STAconnected = WiFi.onStationModeConnected(&onStationConnected);
  STAgotIP = WiFi.onStationModeGotIP(&onStationGotIp);
  STAdisconnected = WiFi.onStationModeDisconnected(&onStationDisconnected);
  APconnected = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequestReceive);
  configTime(timeZone * 3600, 0, "pool.ntp.org", "time.nist.gov", "ntp3.stratum2.ru");
  sensors.begin();
  rtc.begin();
  timer_sendData.attach(1, send_data);
  mqttClient.setServer(mqtt.server_ip, mqtt.port);
  if (strcmp(mqtt.server, ""))
    mqttClient.connect();
  else
    mqttClient.disconnect();
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  pwmController.resetDevices();
  pwmController.init();
  pwmController.setPWMFrequency(1500);
  //Serial.print(pwmController.getChannelPWM(0));
  /*
  Serial.print("FS_PHYS_ADDR: ");
  Serial.println(FS_PHYS_ADDR); //DB000
  Serial.print("FS_PHYS_SIZE: ");
  Serial.println(FS_PHYS_SIZE);
  Serial.print("FS_PHYS_PAGE: ");
  Serial.println(FS_PHYS_PAGE);
  Serial.print("FS_PHYS_BLOCK: ");
  Serial.println(FS_PHYS_BLOCK);    
  */
}
void loop()
{
  ws.cleanupClients();
  ArduinoOTA.handle();
  if (Serial.available() > 0)
  {
    str = Serial.readString();
    if (str.indexOf("start") > -1)
      flag.serDebug = 1;
    if (str.indexOf("stop") > -1)
      flag.serDebug = 0;
  }
}
