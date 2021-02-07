#include <Arduino.h>
#include <ESPAsyncTCP.h>         // https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>   // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncMqttClient.h>     // https://github.com/marvinroger/async-mqtt-client
#include <LiquidCrystal_I2C.h>   // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <ESP8266WiFi.h>         // Standart ESP8266 2.7.3, NOT 2.7.4
#include <Ticker.h>              // Standart ESP8266 2.7.3, NOT 2.7.4
#include <LittleFS.h>            // Standart ESP8266 2.7.3, NOT 2.7.4
#include <FS.h>                  // Standart ESP8266 2.7.3, NOT 2.7.4
#include <ArduinoJson.h>         // v.5.13.4
#include <DallasTemperature.h>   // v.3.9.0
#include <Hash.h>
#include <Updater.h>
#include "RTClib.h"              //https://github.com/adafruit/RTClib/
#include <time.h>
#include <sys/time.h>
#include <OneWire.h>
#include <ESP8266httpUpdate.h>


#include "progmem.h"
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
String fwver = "5.2";
String fsver = "-.-";

#define reconnectTime 5
#define waitForTemp 10

int port = 80;
RTC_DS3231 rtc;
int cmd;
int perc = 0;
AsyncWebServer server(port);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

AsyncMqttClient mqttClient;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

size_t content_len;

Ticker timer_datetime;
Ticker timer_websocket;
Ticker timer_mqtt;
Ticker restart_once;
Ticker wifi_once;

IPAddress ip, ip_gw, mqtt_server_ip;
time_t now;

String configSetup = "{}";
String configChart = "{}";
String jsonLive = "{}";
String ssid, password, ssidAP, passwordAP;
String ip_str, ip_gw_str;
String mqtt_server, mqtt_port, mqtt_user, mqtt_password, mqtt_topic, mqtt_ID;
String dotw[7] = {"6", "0", "1", "2", "3", "4", "5"};
String dt_now;
String time_0[7], time_1[7], time_2[7], time_3[7];
String schedule[3][10][10];

float tempC = 12;
float old_temp = 0.0;
float temp_koef = 0.0;
float fan_start = 30.0;
float fan_stop = 25.0;
float ten_stop = 22.0;
float ten_start = 18.0;

byte disconnectCounter = reconnectTime;
byte temp_error = 0;
byte date_error = 0;
byte chk_inverse = 0;
byte wifi_working = 2;
byte ws_working = 0;
byte led_bright = 0;
byte bf = 0;
byte bf_rel = 0;
byte s_mode_led = 0;
byte s_mode_r1 = 0;
byte s_mode_r2 = 0;
byte lcd_hour, lcd_min;
byte chartFlag = 1;
byte mqttFlag = 0;
byte nedelya;
byte nowisValid;
byte old_valid = 1;

char temp_char[8];
char line1[16], line2[16];
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
const char* ntpServerName = "pool.ntp.org";
const char* defLang;

int max_day_percent, max_night_percent, max_day, max_night;
int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
int ds_day, ds_month, ds_year, ds_hour, ds_min, ds_sec;
int hours, minutes, seconds, year, month, days;
int timeZone = 5;
int isSyncOK = 0;
int LED_value = 0;

long sec_0, sec_1, sec_2, sec_3, sec_now;
