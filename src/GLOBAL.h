#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>         // Standart ESP8266 2.7.3, NOT 2.7.4
#include <Ticker.h>              // Standart ESP8266 2.7.3, NOT 2.7.4
#include <ESPAsyncTCP.h>         // https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>   // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncMqttClient.h>     // https://github.com/marvinroger/async-mqtt-client
#include <ArduinoJson.h>         // v.6.17.2
#include <LittleFS.h>            // Standart ESP8266 2.7.3, NOT 2.7.4
#include <FS.h>                  // Standart ESP8266 2.7.3, NOT 2.7.4
#include <LiquidCrystal_I2C.h>   // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <DallasTemperature.h>   // v.3.9.0
#include <Hash.h>
#include <Updater.h>
#include "RTClib.h"              // https://github.com/adafruit/RTClib/
#include <time.h>
#include <sys/time.h>
#include <OneWire.h>
#include <ESP8266httpUpdate.h>
#include "progmem.h"
#include "ruswords.h"
extern "C" {
#include "user_interface.h"
}

#define SCL           5  //D1 GPIO5   SCL
#define SDA           4  //D2 GPIO4   SDA
#define ONE_WIRE_BUS  0  //D3 GPIO0   DS18B20
#define TENPIN        14 //D5 GPIO14
#define LEDPIN        12 //D6 GPIO12
#define FANPIN        15 //D8 GPIO15   
#define RELAY1PIN     16 //D0 GPIO16
#define RELAY2PIN     13 //D7 GPIO13
#define recTime 10
#define waitForTemp 10
#define waitForDisplay 3

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_PRINT(...) Serial.printf(__VA_ARGS__)
#else 
#define DEBUG_PRINT(...)
#endif

struct ver_t {
  char fw[6] = "5.4";
  char fs[6] = "-.-";
} ver;

struct wifi_t {
  char ssid[32];
  char ssidAP[32] = "ESP8266_AQUA";
  char password[32];
  char passwordAP[32] = "11110000";
  int port = 80;
  IPAddress ip;
  IPAddress ip_gw;
  char ip_str[16];
  char ip_gw_str[16];
  byte working = 0;
  byte countDS = recTime;
  byte count = waitForDisplay;
  int rssi = 31;
} wifi;

struct mqtt_t {
  char server[16] = "192.168.4.1";
  int port = 1883;
  char user[12];
  char password[12];
  char topic[12] = "all";
  char ID[12] = "esp_aqua";
  IPAddress server_ip;
} mqtt;

struct ntp_t {
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
  byte week;
  byte dayOfWeek[7] = {6, 0, 1, 2, 3, 4, 5};
  time_t now;
} ntp;

DateTime rtc_now;

struct opt_t {
  float tempC = 12;
  float old_temp = 0.0;
  float temp_koef = 0.0;
  float fan_start = 30.0;
  float fan_stop = 25.0;
  float ten_stop = 22.0;
  float ten_start = 18.0;
  byte max_d_perc = 100;
  byte max_n_perc = 0;
} option;

AsyncWebServer server(wifi.port);
AsyncWebSocket ws("/ws");

Ticker timer_datetime;
Ticker timer_websocket;
Ticker timer_mqtt;
Ticker restart_once;
Ticker wifi_once;
Ticker mqttReconnectTimer;
Ticker wifi_search;

WiFiEventHandler STAconnected;
WiFiEventHandler STAdisconnected;
WiFiEventHandler APconnected;
WiFiEventHandler STAgotIP;

AsyncMqttClient mqttClient;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

RTC_DS3231 rtc;
size_t content_len;
size_t fsSize;

String configSetup = "{}";
String configChart = "{}";

String schedule[3][10][10];

byte bf = 0;
byte aux_bf = 101;

byte bf_rel = 0;
byte s_mode_led = 0;
byte s_mode_r1 = 0;
byte s_mode_r2 = 0;

byte ws_working;
byte led_bright = 0;
byte lcd_hour, lcd_min;
byte temp_error = 0;
byte date_error = 0;
byte perc = 0;
bool isUpdating = false;
char temp_char[8] = "--.-\2C";
char line1[17], line2[17];
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
char defLang[4] = "RUS";
char weekLang[4];
char configLive[100];

int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
int max_day, max_night;
int timeZone = 5;
int LED_value = 0;
int heapCur = 0;
int heapMin = 50000;

long sec_0, sec_1, sec_2, sec_3, sec_now;