#include "GLOBAL.h"
#include "LCD.h"
#include "WIFI.h"
#include "MQTT_WS.h"
#include "JSON.h"
#include "FUNCTIONS.h"
#include "FILESYSTEM.h"

void ICACHE_RAM_ATTR inputButton();
void readDateTime()
{
  state.valNow = rtc.now().isValid();
  if (!state.valNow)
  {
    if (state.valNow != oldState.valNow)
    {
      SERIAL_PRINT("[%s]: %2.1fC | LED_value:%i\n", rtc.now().timestamp().c_str(), option.tempC, LED_value);
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
      SERIAL_PRINT("[%s]: Temp:%2.1fC\n", rtc.now().timestamp().c_str(), t);
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
  //DEBUG_PRINT("MaxBlockSize: %i | HeapFragm: %i%% | heapCur: %i | heapMin: %i \n", ESP.getMaxFreeBlockSize(), ESP.getHeapFragmentation(), heapCur, heapMin);
  //SERIAL_PRINT("heapCur: %i | heapMin: %i | wifi: %i\n", state.heapCur, state.heapMin, state.wifi);
  //DEBUG_PRINT("line1: %s\n", line1);
  //DEBUG_PRINT("line2: %s\n", line2);
  //DEBUG_PRINT("state.wifi: %i\n", state.wifi);
  line_show();
}
void noLight()
{
  lcd.noBacklight();
  SERIAL_PRINT("BUTTON FINISH: %i\n", option.delLed);
  flag.inter = 0;
}
void inputButton()
{
  if ((!flag.inter) && (option.delLed))
  {
    lcd.backlight();
    SERIAL_PRINT("BUTTON START: %i\n", option.delLed);
    timer_once.once(option.delLed, noLight);
    flag.inter = 1;
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
    if (mqttClient.connected())
      mqttClient.publish(mqtt.topic, 0, true, sendJson());
    //mqttClient.publish(mqtt.topic, 0, true, configLive);

    if (state.wifi == 1)
      wifi.rssi = WiFi.RSSI();
    flag.inc++;
    if (flag.inc >= 60)
    {
      oldState.rssi = sendOnChange("rssi", wifi.rssi, oldState.rssi);
      flag.inc = 0;
    }
    if (oldState.tempC != option.tempC && !isUpdating)
    {
      //SERIAL_PRINT("[SSE] tempC: %2.1f\n", option.tempC);
      events.send(option.tempC_char, "temp", millis());
      oldState.tempC = option.tempC;
    }
    oldState.led = sendOnChange("led", state.led, oldState.led);
    if (state.ws)
      ws.textAll(sendJson());
    //ws.textAll(configLive);
  }
}
void setup()
{
#ifdef SERIAL_ON
  Serial.begin(115200);
  Serial.println();
#endif
#ifdef DEBUG_ON
  Serial.begin(115200);
  Serial.println();
#endif
  pinMode(BUTPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTPIN), inputButton, CHANGE);
  initLCD();
  initOTA();
  initFileSystem();
  DEBUG_PRINT("[SETUP] INITIAL PIN STATES: %i\n", state.inv * 4096);
  pwmController.setChannelPWM(FANPIN, state.inv * 4096);
  pwmController.setChannelPWM(TENPIN, state.inv * 4096);
  pwmController.setChannelPWM(RELAY1PIN, state.inv * 4096);
  pwmController.setChannelPWM(RELAY2PIN, state.inv * 4096);
  pwmController.setChannelPWM(LEDPIN, state.inv * 4096);

  snprintf(line2, 17, "FW:%s  FS:%s", ver.fw, ver.fs);
  printLCD(2, "  ESP8266_AQUA  ", line2);
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.hostname("ESP8266_AQUA");
  WiFi_initSTA();
  STAconnected = WiFi.onStationModeConnected(&onStationConnected);
  STAgotIP = WiFi.onStationModeGotIP(&onStationGotIp);
  STAdisconnected = WiFi.onStationModeDisconnected(&onStationDisconnected);
  APconnected = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequestReceive);
  WiFi.setOutputPower(12); //0 - 20.5 dBm
  if (WiFi.getAutoConnect() != true)
  {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  configTime(timeZone * 3600, 0, "pool.ntp.org", "time.nist.gov", "ntp3.stratum2.ru");
  sensors.begin();
  rtc.begin();
  timer_sendData.attach(1, send_data);
  mqttClient.setServer(mqtt.server_ip, mqtt.port);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  pwmController.resetDevices();
  pwmController.init();
  pwmController.setPWMFrequency(1500);
  //SERIAL_PRINT(pwmController.getChannelPWM(0));
}

void loop()
{
  ws.cleanupClients();
  ArduinoOTA.handle();
  if (isUpdating)
  {
    //detachInterrupt(BUTPIN);
    mqttClient.disconnect();
    wifi_search.detach();
  }
}
