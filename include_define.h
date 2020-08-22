#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <PubSubClient.h>
//================================================================================================================================
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
//================================================================================================================================
#include <DallasTemperature.h>
#include <TickerScheduler.h>

#define SCL           5  //D1 GPIO5   SCL
#define SDA           4  //D2 GPIO4   SDA
#define ONE_WIRE_BUS  0  //D3 GPIO0   DS18B20
#define TENPIN        14 //D5 GPIO14
#define LEDPIN        12 //D6 GPIO12
#define FANPIN        15 //D8 GPIO15
#define RELAY1PIN     16 //D0 GPIO16
#define RELAY2PIN     13 //D7 GPIO13
#define maxPWM        1009
#define websocketPort 81

int Terr = 0;
int port = 80;

const char* ver = "4.1b";
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static const char successResponse[] PROGMEM =
  "<META http-equiv=\"refresh\" content=\"10;URL=/\">Update Success! Rebooting...\n";

LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(port);
ESP8266HTTPUpdateServer httpUpdater;

//WiFiClient espClient;
//PubSubClient mqttClient(espClient);

WebSocketsServer webSocket = WebSocketsServer(websocketPort);
File fsUploadFile;
TickerScheduler ts(2);

int CheckDelay = 10;
int n = 0;
String _ssid, _password, _ssidAP, _passwordAP;
IPAddress ip, ip_gw;
byte tries = 11;
String configSetup = "{}";
String configChart = "{}";
String configLog = "";
String jsonLive = "{}";
//String jsonLive = "{\"temp\":\"--\",\"now\":\"-\",\"led\":\"-\",\"bitFlags\":\"-\",\"Terr\":\"-\",\"ws\":\"-\",\"rssi\":\"-\"}";
float tempC, temp_filtered;
float temp_koef = 0.0;
float fan_start = 30.0;
float fan_stop = 25.0;
float ten_stop = 22.0;
float ten_start = 18.0;
byte relay1_working = 0;
byte relay2_working = 0;
byte fan_working = 0;
byte ten_working = 0;
byte wifi_working = 0;
byte ws_working = 0;
byte mqtt_working = 0;
byte bitFlags = 0;
int led_bright = 0;
byte s_mode_led = 0;
byte s_mode_r1 = 0;
byte s_mode_r2 = 0;
int rssi;
uint32_t myTimer1, myTimer2, myTimer3;
int uploadProc = 0;          // загрузка % при обновлении прошивки
int flag = 1;
byte triesCon = 21;
char line1[16], line2[16], line2_1[16], line2_2[16], logWrite[60];

String mqtt_server = "192.168.0.2";
int mqtt_port = 1883;
String mqtt_user = "";
String mqtt_password = "";
String mqtt_topic = "";
String mqtt_ID = "aqua";
