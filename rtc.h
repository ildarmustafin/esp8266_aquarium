#include <TimeLib.h>
#include <WiFiUdp.h>
#include "RTClib.h"

RTC_DS3231 rtc;
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
String dotw[7] = {"6", "0", "1", "2", "3", "4", "5"};
WiFiUDP udp;
int LED_value = 0;
byte nedelya, isRTCconnected;
String dt_now;
int max_day_percent, max_night_percent;
int max_day, max_night;
String time_0[7], time_1[7], time_2[7], time_3[7];
int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
long sec_0, sec_1, sec_2, sec_3, sec_now;
String schedule[3][10][10];
int ds_day, ds_month, ds_year, ds_hour, ds_min, ds_sec;
int hours, minutes, seconds, years, months, days;
int timeZone = 5;
unsigned int localPort = 2390;
int cb;

IPAddress timeServerIP;
const char* ntpServerName;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

unsigned long sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void updateTimeNTP() {
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(1000);
  cb = udp.parsePacket();
  if (!cb) {
    Serial.println("Can't connect to the server");
  } else {
    udp.read(packetBuffer, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears + timeZone * SECS_PER_HOUR;
    minutes = ((epoch % 3600) / 60);
    hours = (epoch  % 86400L) / 3600;
    seconds = (epoch % 60);
    years = year(epoch);
    months = month(epoch);
    days = day(epoch);
  }
}
