#include <Arduino.h>
#include <ESPAsyncTCP.h>         // https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>   // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncMqttClient.h>     // https://github.com/marvinroger/async-mqtt-client
#include <LiquidCrystal_I2C.h>   // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <ESP8266WiFi.h>         // Standart ESP8266 2.7.4
#include <Ticker.h>              // Standart ESP8266 2.7.4
#include <LittleFS.h>            // Standart ESP8266 2.7.4
#include <FS.h>                  // Standart ESP8266 2.7.4
#include <ArduinoJson.h>         // v.5.13.4
#include <DallasTemperature.h>   // v.3.9.0
#include <Hash.h>
#include <Updater.h>

#include <OneWire.h>
#include "user_interface.h"

#define SCL           5  //D1 GPIO5   SCL
#define SDA           4  //D2 GPIO4   SDA
#define ONE_WIRE_BUS  0  //D3 GPIO0   DS18B20
#define TENPIN        14 //D5 GPIO14
#define LEDPIN        12 //D6 GPIO12
#define FANPIN        15 //D8 GPIO15   
#define RELAY1PIN     16 //D0 GPIO16
#define RELAY2PIN     13 //D7 GPIO13
#define maxPWM        1023
#define U_PART U_FS
#define ver "5.1a"
#define reconnectTime 10
#define waitForTemp 10

int port = 80;

size_t content_len;

AsyncWebServer server(port);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

AsyncMqttClient mqttClient;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static const char successResponse[] PROGMEM =
  "<META http-equiv=\"refresh\" content=\"20;URL=/\">Update Success! Rebooting...Wait for 20 sec\n";
LiquidCrystal_I2C lcd(0x27, 16, 2);

Ticker timer_datetime;
Ticker timer_websocket;
Ticker timer_mqtt;
Ticker restart_once;
Ticker wifi_once;

String configSetup = "{}";
String configChart = "{}";
String jsonLive = "{}";
String ssid, password, ssidAP, passwordAP;
String ip_str, ip_gw_str;
IPAddress ip, ip_gw, mqtt_server_ip;

int disconnectCounter = reconnectTime;

String inString;
String defLang;

byte id = 0;
float old_temp = 0.0;
String temp_debug;
byte temp_error = 0;
byte date_error = 0;
float tempC = 12;
char temp_char[8];

float temp_koef = 0.0;
float fan_start = 30.0;
float fan_stop = 25.0;
float ten_stop = 22.0;
float ten_start = 18.0;

byte relay1_working = 0;
byte relay2_working = 0;
byte fan_working = 0;
byte ten_working = 0;
byte wifi_working = 2;
byte ws_working = 0;
byte mqtt_working = 0;
byte led_bright = 0;

byte bf = 0;
byte bf_rel = 0;
byte s_mode_led = 0;
byte s_mode_r1 = 0;
byte s_mode_r2 = 0;

int uploadProc = 0;
int chartFlag = 1;
char line1[16], line2[16], logWrite[60];
int lcd_hour, lcd_min;
String mqtt_server, mqtt_port, mqtt_user, mqtt_password, mqtt_topic, mqtt_ID;
byte mqttFlag = 0;
