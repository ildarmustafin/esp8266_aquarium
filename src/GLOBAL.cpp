#include "GLOBAL.h"
int BUTPIN = 14;
int IPPIN = 13;

struct ver_t ver;
struct wifi_t wifi;
struct mqtt_t mqtt;
struct ntp_t ntp;
struct opt_t option;
struct oldState_t oldState;
struct state_t state;
struct flag_t flag;

PCA9685 pwmController;

GyverRelay fan_reg(NORMAL);
GyverRelay ten_reg(REVERSE);

AsyncWebServer server(wifi.port);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

Ticker timer_sendData;
Ticker timer_once;
Ticker timerIP_once;
Ticker wifi_once;
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
size_t content_len = 0;
size_t fsSize = 0;

String configSetup = "{}";
String configChart = "{}";

char sched_led[7][4][6];
char sched_rel[7][4][6];

uint8_t bf_rel = 0;
uint8_t s_mode_led = 0;
uint8_t s_mode_r1 = 0;
uint8_t s_mode_r2 = 0;

uint8_t lcd_hour = 0;
uint8_t lcd_min = 0;
uint8_t lcd_sec = 0;
uint8_t temp_error = 0;
uint8_t date_error = 0;
bool isUpdating = false;
char line1[17] = "\0";
char line2[17] = "\0";
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
char defLang[4] = "RUS";
char weekLang[4] = "\0";
char configLive[86] = "\0";

int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
uint16_t max_day, max_night;
int8_t timeZone = 5;
uint16_t LED_value = 0;
long sec_0, sec_1, sec_2, sec_3, sec_now;
String str;
