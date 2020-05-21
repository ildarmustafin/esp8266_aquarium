#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include <SimpleMovingAverage.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <PubSubClient.h>
//================================================================================================================================
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
//================================================================================================================================
#define SCL           D1  // SCL
#define SDA           D2  // SDA
#define ONE_WIRE_BUS  D3  // DS18B20
#define TENPIN        D5
#define LEDPIN        D6
#define FANPIN        D7
#define RELAY1PIN     D0
#define RELAY2PIN     D8
int port = 49201;
int websocketPort = 81;

//String f_ver = "4.2";
const char* ver = "2.11c";

static const char successResponse[] PROGMEM =
  "<META http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...\n";

LiquidCrystal_I2C lcd(0x27, 16, 2);

SimpleMovingAverage avg;
ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server = "broker.mqtt-dashboard.com";

WebSocketsServer webSocket = WebSocketsServer(websocketPort);
File fsUploadFile;
OneWire  ds(ONE_WIRE_BUS);

byte tries = 11;
byte triesCon = 11;
int CheckDelay = 10;
int n = 0;
String _ssid, _password;
IPAddress ip;
String configSetup = "{}";
String configChart = "{}";
String jsonLive = "{\"temp\":\"--\",\"now\":\"--.--.----    --:--\",\"led\":\"-\",\"fan\":\"-\",\"ten\":\"-\",\"rel1\":\"-\",\"rel2\":\"-\",\"rssi\":\"-\"}";

String clientId = "ESP8266";
static char send_temp[15];
float tempC, temp_filtered, values_day, temp_koef, fan_stop, fan_start, ten_start, ten_stop;
byte relay1_working = 0;
byte relay2_working = 0;
byte led_working = 0;
byte fan_working = 0;
byte ten_working = 0;
byte wifi_working = 0;
byte ws_working = 0;

int led_bright = 0;
int randomize, rssi;

unsigned long previousMillis = 0; // для таймера
int uploadProc = 0;               // загрузка % при обновлении прошивки
int flag = 1;             // для записи графика по времени 3-9-15-21
char line1[16], line2[16], line2_1[16], line2_2[16];        // для вывода инфы на lcd1602
//floor() — округление вниз
//ceil() — округление вверх
//round() — округление в ближайшую сторону
