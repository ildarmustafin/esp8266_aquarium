#include "WIFI.h"

void showWifi()
{
  state.wifi = 1;
}

void WiFi_initAP_STA()
{
  //ESP8266.getConnectedDevices(callback);
  //WiFi.softAPgetStationNum();
  if (!strcmp(wifi.ssidAP, "")) //HEX: ESP-D9B0FF    DEC: 14266623
  {
    Serial.println();
    Serial.println(F("[WIFI] EMPTY AP OPTIONS. SET TO DEFAULT"));
    Serial.println(F("[WIFI] DEFAULT SSID AP = \"ESP8266_AQUA\""));
    Serial.println(F("[WIFI] DEFAULT PASSWORD AP = \"EMPTY\""));
    Serial.println();
    strcpy(wifi.ssidAP, "ESP8266_AQUA");
    strcpy(wifi.passwordAP, "");
  }
  snprintf(line2, 16, PSTR("   CREATE AP!   "));
  Serial.println(F("[WIFI] INIT AP MODE")); //7 byte
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  strcpy(state.myIP, "192.168.4.1");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifi.ssidAP, wifi.passwordAP);
  wifi_search.attach(1, search_wifi);
}
void WiFi_initSTA()
{
  wifi_search.detach();
  if (strcmp(wifi.ip_str, "") && strcmp(wifi.ip_gw_str, ""))
  {
    IPAddress ip_subnet(255, 255, 255, 0);
    WiFi.config(wifi.ip, wifi.ip_gw, ip_subnet, wifi.ip_gw);
  }
  if (strcmp(wifi.ssid, "") && strcmp(wifi.password, ""))
  {
    Serial.println(F("[WIFI] INIT STA MODE")); //8 byte
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi.ssid, wifi.password);
  }
  else
  {
    WiFi_initAP_STA();
  }
}
void printScanResult(int networksFound)
{
  //Serial.println(F("[WIFI] SEARCHING"));
  for (int i = 0; i < networksFound; i++)
  {
    if (WiFi.SSID(i) == wifi.ssid)
    {
      Serial.print(F("[WIFI] FOUND "));
      Serial.println(WiFi.SSID(i));
      Serial.println(F("[WIFI] CONNECTING...PLEASE WAIT..."));
      WiFi_initSTA();
    }
  }
}
void search_wifi()
{
  WiFi.scanNetworksAsync(printScanResult);
}
void onStationConnected(const WiFiEventStationModeConnected &evt)
{
  Serial.print(F("[WIFI] STATION CONNECTED TO: "));
  Serial.println(evt.ssid);
  snprintf(line2, 17, " WIFI CONNECTED ");  
  state.wifi = 1;
  timerIP_once.once(3, showWifi);  

}
void onStationDisconnected(const WiFiEventStationModeDisconnected &evt)
{
  //WiFi.reconnect();
  state.wifi = 0;
  if (--wifi.countDS)
  {
    Serial.print(F("[WIFI] CONNECTING...TRY:"));
    Serial.println(wifi.countDS);
    lcd.createChar(1, z_wifi_tc);
    snprintf(line1, 17, "%02d:%02d %s %-7s", lcd_hour, lcd_min, weekLang, option.tempC_char);    
    snprintf(line2, 17, "CONNECTING...:%i", wifi.countDS);
  }
  else
  {
    WiFi_initAP_STA();
    wifi.countDS = recTime;
  }
}
void onStationGotIp(const WiFiEventStationModeGotIP &evt)
{
  lcd.createChar(1, z_wifi_sta);
  (strcmp(wifi.ip_str, "") && strcmp(wifi.ip_gw_str, "")) ? strcpy(flag.ip_type, PSTR("STATIC")) : strcpy(flag.ip_type, PSTR("DHCP"));
  Serial.print(F("[WIFI] STATION GOT IP ["));
  Serial.print(flag.ip_type);
  strcpy(state.myIP, evt.ip.toString().c_str());
  Serial.println(PSTR("]: ") + evt.ip.toString());
  state.wifi = 1;
  pwmController.setChannelOn(DOP2LED);
}
void onProbeRequestReceive(const WiFiEventSoftAPModeProbeRequestReceived &evt)
{
  //Serial.print("CREATE AP\n");
  lcd.createChar(1, z_wifi_ap);
  wifi.rssi = evt.rssi;
  state.wifi = 2;
  pwmController.setChannelOff(DOP2LED);  
}