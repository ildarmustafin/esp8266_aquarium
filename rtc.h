#include "RTClib.h"
#include <time.h>
#include <sys/time.h>

RTC_DS3231 rtc;
char daysOfTheWeekENG[7][3] = {"MO", "TU", "WE", "TH", "FR", "SA", "SU"};
char daysOfTheWeekRUS[7][3] = {"\7H", "BT", "CP", "\7T", "\7T", "C\7", "BC"};
String dotw[7] = {"6", "0", "1", "2", "3", "4", "5"};

int LED_value = 0;
byte nedelya;
byte nowisValid;
byte old_valid = 1;
String dt_now;
int max_day_percent, max_night_percent, max_day, max_night;
String time_0[7], time_1[7], time_2[7], time_3[7];
int hour_0[7], min_0[7], hour_1[7], min_1[7], hour_2[7], min_2[7], hour_3[7], min_3[7];
long sec_0, sec_1, sec_2, sec_3, sec_now;
String schedule[3][10][10];
int ds_day, ds_month, ds_year, ds_hour, ds_min, ds_sec;
int hours, minutes, seconds, years, months, days;
int timeZone = 5;
int isSyncOK = 0;
time_t now;
const char* ntpServerName = "pool.ntp.org";

void updateTimeNTP() {
  now = time(nullptr);
  struct tm * tm = localtime(&now);
  if (tm->tm_year == 70) {
    isSyncOK = 0;
  } else {
    isSyncOK = 1;
    days = tm->tm_mday;
    months = tm->tm_mon + 1;
    years = tm->tm_year + 1900;
    hours = tm->tm_hour;
    minutes = tm->tm_min;
    seconds = tm->tm_sec;
  }
}
