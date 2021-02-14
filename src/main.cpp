#include "GLOBAL.h"
#include "LCD.h"
#include "WIFI.h"
#include "MQTT_WS.h"
#include "JSON.h"
#include "FUNCTIONS.h"
#include "FILESYSTEM.h"

void readDateTime()
{
  rtc_now = rtc.now();
  bitWrite(bf, 0, rtc_now.isValid());
  if (!bitRead(bf, 0))
  {
    if (bitRead(bf, 0) != bitRead(aux_bf, 0))
    {
      //DEBUG_PRINT("[%s]: %2.1fC | LED_value:%i\n", rtc_now.timestamp().c_str(), option.tempC, LED_value);
      bitWrite(aux_bf, 0, bitRead(bf, 0));
    }
    if (date_error < 10)
    {
      date_error++;
    }
    else
    {
      lcd_hour = 0;
      lcd_min = 0;
      led_bright = 0;
      analogWrite(LEDPIN, 0);
      bitClear(bf, 3);
      bitClear(bf, 2);
      digitalOutput(RELAY1PIN, 3);
      digitalOutput(RELAY2PIN, 2);
    }
  }
  else
  {
    sec_now = rtc_now.hour() * 3600 + rtc_now.minute() * 60 + rtc_now.second();
    ntp.week = ntp.dayOfWeek[rtc_now.dayOfTheWeek()];
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
    lcd_hour = rtc_now.hour();
    lcd_min = rtc_now.minute();
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
    if (t != option.old_temp)
    {
      //DEBUG_PRINT("[%s]: Temp:%2.1fC\n", rtc_now.timestamp().c_str(), t);
      option.old_temp = t;
    }
    if (temp_error < waitForTemp)
    {
      temp_error++;
    }
    else
    {
      option.tempC = round(t * 10) / 10.0 + option.temp_koef;
      strcpy(temp_char, "--.-\2C"); //7
      bitClear(bf, 5);
      bitClear(bf, 4);
      digitalOutput(FANPIN, 5);
      digitalOutput(TENPIN, 4);
    }
  }
  else
  {
    temp_error = 0;
    option.tempC = round(t * 10) / 10.0 + option.temp_koef;
    snprintf(temp_char, 8, "%2.1f\2C", option.tempC);
    temp_fan_regulation(option.tempC);
    temp_ten_regulation(option.tempC);
  }
}

void measure_datetime()
{
  readDateTime();
  readTemp();
  heapCur = ESP.getFreeHeap();
  heapMin = (heapCur < heapMin) ? heapCur : heapMin;
  //DEBUG_PRINT("MaxBlockSize: %i | HeapFragm: %i%% | heapCur: %i | heapMin: %i \n", ESP.getMaxFreeBlockSize(), ESP.getHeapFragmentation(), heapCur, heapMin);
  !strcmp(defLang, "RUS") ? strcpy(weekLang, daysOfTheWeekRUS[ntp.week]) : strcpy(weekLang, daysOfTheWeekENG[ntp.week]);
  //DEBUG_PRINT("heapCur: %i | heapMin: %i | wifi: %i\n", heapCur, heapMin, wifi.working);
  //DEBUG_PRINT("line1: %s\n", line1);
  //DEBUG_PRINT("line2: %s\n", line2);
  line_show();
}

void setup()
{
#ifdef DEBUG_ON
  Serial.begin(115200);
#endif
  analogWriteFreq(100);
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(TENPIN, OUTPUT);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  analogWrite(LEDPIN, 0);
  initLCD();
  ArduinoOTA.setPort(8266);

  ArduinoOTA.onStart([]() {
    isUpdating = true;
    //DEBUG_PRINT("Start OTA ");
  });
  ArduinoOTA.onEnd([]() {
    //DEBUG_PRINT("\nEnd OTA");
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
    //DEBUG_PRINT("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    isUpdating = false;  
    //DEBUG_PRINT("Error[%u]: ", error);
    //if (error == OTA_AUTH_ERROR) DEBUG_PRINT("Auth Failed");
    //else if (error == OTA_BEGIN_ERROR) DEBUG_PRINT("Begin Failed");
    //else if (error == OTA_CONNECT_ERROR) DEBUG_PRINT("Connect Failed");
    //else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINT("Receive Failed");
    //else if (error == OTA_END_ERROR) DEBUG_PRINT("End Failed");
  });

  ArduinoOTA.begin();
  initFileSystem();
  digitalWrite(FANPIN, bitRead(aux_bf, 1));
  digitalWrite(TENPIN, bitRead(aux_bf, 1));
  digitalWrite(RELAY1PIN, bitRead(aux_bf, 1));
  digitalWrite(RELAY2PIN, bitRead(aux_bf, 1));
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
  //WiFi.setOutputPower(10); //0 - 20.5 dBm
  if (WiFi.getAutoConnect() != true)
  {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  configTime(timeZone * 3600, 0, "pool.ntp.org", "time.nist.gov", "ntp3.stratum2.ru");
  sensors.begin();
  rtc.begin();
  timer_datetime.attach(1, measure_datetime);
  timer_websocket.attach(1, send_ws);
  mqttClient.setServer(mqtt.server_ip, mqtt.port);
  timer_mqtt.attach(1, send_mqtt);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
}

void loop()
{
  yield();
  ws.cleanupClients();
  ArduinoOTA.handle();
  if (isUpdating)
  {
    timer_datetime.detach();
    timer_websocket.detach();
    timer_mqtt.detach();
    wifi_search.detach();
  }
}
