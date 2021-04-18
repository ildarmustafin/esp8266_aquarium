#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <ESPAsyncTCP.h>       // https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncMqttClient.h>   // https://github.com/marvinroger/async-mqtt-client
#include <ArduinoJson.h>       // v.6.17.2
#include <LittleFS.h>
#include <FS.h>
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <DallasTemperature.h> // v.3.9.0
#include <Hash.h>
#include "RTClib.h"            // https://github.com/adafruit/RTClib/
#include <time.h>
#include <sys/time.h>
#include <OneWire.h>
//#include <ESP8266httpUpdate.h>
//#include <Updater.h>
///#include <ESP8266HTTPUpdateServer.h> //sync webserver
//#include <WiFiUdp.h>

#include "progmem.h"
#include "ruswords.h"
#include "PCA9685.h"           // https://github.com/NachtRaveVL/PCA9685-Arduino 
#include "GyverRelay.h"        // https://github.com/AlexGyver/GyverLibs/tree/master/GyverRelay

#include "LCD.h"
#include "WIFI.h"
#include "MQTT_WS.h"
#include "JSON.h"
#include "FUNCTIONS.h"
#include "FILESYSTEM.h"

extern "C"
{
#include "user_interface.h"
}
//#define PROGMEM ICACHE_RODATA_ATTR
//#define ICACHE_RODATA_ATTR __attribute__((section(".irom.text")))

extern int BUTPIN;
extern int IPPIN;
#define SCL 5        //D1 GPIO5   SCL
#define SDA 4        //D2 GPIO4   SDA
#define SENSOR_PIN 0 //D3 GPIO0   DS18B20

#define RELAY2PIN 0
#define RELAY1PIN 1
#define TENPIN 2
#define LEDPIN 3
#define FANPIN 4

#define RELAY2LED 5
#define RELAY1LED 6
#define TENLED 7
#define LEDLED 8
#define FANLED 9
#define DOP1LED 10
#define DOP2LED 11

#define recTime 4
#define waitForTemp 10
#define waitForDisplay 3

struct ver_t
{
  char fw[6] = "7.1";
  char fs[6] = "-.-";
};
struct wifi_t
{
  char ssid[16];
  char ssidAP[16];
  char password[16];
  char passwordAP[16];
  char ip_str[16];
  char ip_gw_str[16];
  IPAddress ip;
  IPAddress ip_gw;
  uint16_t port = 80;
  int8_t rssi = 31;
  uint8_t countDS = recTime;
  uint8_t count = waitForDisplay;
};
struct mqtt_t
{
  uint16_t port = 1883;
  char server[16] = "192.168.4.1";
  char user[12];
  char password[12];
  char t_all[10] = "all";
  char t_temp[10] = "temp";
  char t_led[10] = "led";
  char t_fan[10] = "fan";
  char t_ten[10] = "ten";
  char t_rel1[10] = "rel1";
  char t_rel2[10] = "rel2";          
  char t_date[10] = "date";  
  char ID[12] = "esp_aqua";
  IPAddress server_ip;
};
struct ntp_t
{
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
  uint8_t week;
  uint8_t dayOfWeek[7] = {6, 0, 1, 2, 3, 4, 5};
  time_t now;
};
struct opt_t
{
  float tempC = 12.0;
  float t_old = 0.0;
  float temp_koef = 0.0;
  uint8_t delLed = 100;
  char tempC_char[8] = "--.-\2C";
  uint8_t max_d_perc = 100;
  uint8_t max_n_perc = 0;
};
struct oldState_t
{
  uint8_t rel1 = 0;
  uint8_t rel2 = 0;
  uint8_t fan = 0;
  uint8_t ten = 0;
  uint8_t led = 0;
  uint8_t mqtt = 0;  
  uint8_t valNow = 0;
  uint8_t perc = 0;    
  int8_t rssi = 0;
  uint16_t heapMin = 65000;
  float tempC = 12;
};
struct state_t
{
  uint8_t rel1 = 0;
  uint8_t rel2 = 0;
  uint8_t fan = 0;
  uint8_t ten = 0;
  uint8_t valNow = 0;
  uint8_t valSync = 0;
  uint8_t inv = 0;
  uint8_t led = 0;
  uint8_t mqtt = 0;
  uint8_t ws = 0;
  uint8_t bf = 0;
  uint8_t wifi = 0;
  uint8_t perc = 0;  
  uint16_t heapMin = 65000;  
  uint16_t heapCur = 0;  
  char led_char[5];
  char myIP[16];  
};
struct flag_t
{
  uint8_t serDebug = 0;    
  uint8_t chart = 1;
  uint8_t buttonPressed = 0;   
  uint8_t ipPressed = 0;   
  uint8_t inc = 0;
  char ip_type[7];
};

extern struct ver_t ver;
extern struct wifi_t wifi;
extern struct mqtt_t mqtt;
extern struct ntp_t ntp;
extern struct opt_t option;
extern struct oldState_t oldState;
extern struct state_t state;
extern struct flag_t flag;

extern PCA9685 pwmController;

extern GyverRelay fan_reg;
extern GyverRelay ten_reg;

extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncEventSource events;

extern Ticker timer_sendData;
extern Ticker timer_once;
extern Ticker timerIP_once;
extern Ticker wifi_once;
extern Ticker mqttReconnectTimer;
extern Ticker wifi_search;

extern WiFiEventHandler STAconnected;
extern WiFiEventHandler STAdisconnected;
extern WiFiEventHandler APconnected;
extern WiFiEventHandler STAgotIP;

extern AsyncMqttClient mqttClient;
extern OneWire oneWire;
extern DallasTemperature sensors;
extern LiquidCrystal_I2C lcd;
extern RTC_DS3231 rtc;
extern size_t content_len;
extern size_t fsSize;

extern String configSetup;
extern String configChart;

extern char sched_led[7][4][6];
extern char sched_rel[7][4][6];

extern uint8_t bf_rel;
extern uint8_t s_mode_led;
extern uint8_t s_mode_r1;
extern uint8_t s_mode_r2;

extern uint8_t lcd_hour;
extern uint8_t lcd_min;
extern uint8_t lcd_sec;
extern uint8_t temp_error;
extern uint8_t date_error;
extern bool isUpdating;
extern char line1[17];
extern char line2[17];
extern char daysOfTheWeekENG[7][3];
extern char daysOfTheWeekRUS[7][3];
extern char defLang[4];
extern char weekLang[4];
extern char configLive[86];

extern int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
extern uint16_t max_day, max_night;
extern int8_t timeZone;
extern uint16_t LED_value;
extern long sec_0, sec_1, sec_2, sec_3, sec_now;

extern String str;
#endif