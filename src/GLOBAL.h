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
#include <Updater.h>
#include "RTClib.h"            // https://github.com/adafruit/RTClib/
#include <time.h>
#include <sys/time.h>
#include <OneWire.h>
#include <ESP8266httpUpdate.h>
#include "progmem.h"
#include "ruswords.h"
#include "PCA9685.h"           // https://github.com/NachtRaveVL/PCA9685-Arduino 
#include "GyverRelay.h"        // https://github.com/AlexGyver/GyverLibs/tree/master/GyverRelay

extern "C"
{
#include "user_interface.h"
}
const int BUTPIN = 14;

#define SCL 5        //D1 GPIO5   SCL
#define SDA 4        //D2 GPIO4   SDA
#define SENSOR_PIN 0 //D3 GPIO0   DS18B20
#define TENPIN 2
#define LEDPIN 3
#define FANPIN 4
#define RELAY1PIN 1
#define RELAY2PIN 0
#define recTime 4
#define waitForTemp 10
#define waitForDisplay 3

#define SERIAL_ON
//#define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#ifdef SERIAL_ON
#define SERIAL_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define SERIAL_PRINT(...)
#endif

struct ver_t
{
  char fw[6] = "6.1";
  char fs[6] = "-.-";
} ver;
struct wifi_t
{
  char ssid[16];
  char ssidAP[16] = "ESP8266_AQUA";
  char password[16] = "12345678";
  char passwordAP[16] = "12345678";
  char ip_str[16];
  char ip_gw_str[16];
  IPAddress ip;
  IPAddress ip_gw;
  uint16_t port = 80;
  int8_t rssi = 31;
  uint8_t countDS = recTime;
  uint8_t count = waitForDisplay;
} wifi;
struct mqtt_t
{
  uint16_t port = 1883;
  char server[16] = "192.168.4.1";
  char user[12];
  char password[12];
  char topic[12] = "all";
  char ID[12] = "esp_aqua";
  IPAddress server_ip;
} mqtt;
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
} ntp;
struct opt_t
{
  float tempC = 12.0;
  float t_old = 0.0;
  float temp_koef = 0.0;
  uint8_t delLed = 100;
  char tempC_char[8] = "--.-\2C";
  uint8_t max_d_perc = 100;
  uint8_t max_n_perc = 0;
} option;
struct oldState_t
{
  uint8_t rel1 = 0;
  uint8_t rel2 = 0;
  uint8_t fan = 0;
  uint8_t ten = 0;
  uint8_t led = 0;
  uint8_t valNow = 0;
  uint8_t perc = 0;    
  int8_t rssi = 0;
  uint16_t heapMin = 65000;
  float tempC = 12;
} oldState;
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
} state;
struct flag_t
{
  uint8_t chart = 1;
  uint8_t mqtt = 0;
  uint8_t inter = 0;
  uint8_t inc = 0;
  char ip_type[7];
} flag;

PCA9685 pwmController;

GyverRelay fan_reg(NORMAL);
GyverRelay ten_reg(REVERSE);

AsyncWebServer server(wifi.port);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

Ticker timer_sendData;
Ticker timer_once;
Ticker mqttReconnectTimer;
Ticker wifi_search;

WiFiEventHandler STAconnected;
WiFiEventHandler STAdisconnected;
WiFiEventHandler APconnected;
WiFiEventHandler STAgotIP;

AsyncMqttClient mqttClient;
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

RTC_DS3231 rtc;
size_t content_len;
size_t fsSize;

String configSetup = "{}";
String configChart = "{}";

char sched_led[7][4][6];
char sched_rel[7][4][6];

uint8_t bf_rel = 0;
uint8_t s_mode_led = 0;
uint8_t s_mode_r1 = 0;
uint8_t s_mode_r2 = 0;

uint8_t lcd_hour, lcd_min;
uint8_t temp_error = 0;
uint8_t date_error = 0;
bool isUpdating = false;
char line1[17], line2[17];
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
char defLang[4] = "RUS";
char weekLang[4];
char configLive[86];

int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
uint16_t max_day, max_night;
int8_t timeZone = 5;
uint16_t LED_value = 0;
long sec_0, sec_1, sec_2, sec_3, sec_now;
