void WiFi_initAP_STA()
{
  snprintf(line2, 16, PSTR("   CREATE AP!   "));
  Serial.println(F("[WIFI] INIT AP MODE")); //7 byte   
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifi.ssidAP, wifi.passwordAP);
}
void WiFi_initSTA()
{
  Serial.println(F("[WIFI] INIT STA MODE"));  //8 byte
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  if (strcmp(wifi.ip_str, "") && strcmp(wifi.ip_gw_str, ""))
  {
    IPAddress ip_subnet(255, 255, 255, 0);
    WiFi.config(wifi.ip, wifi.ip_gw, ip_subnet, wifi.ip_gw);
  }
  if (strcmp(wifi.ssid, "") && strcmp(wifi.password, ""))
  {
    WiFi.begin(wifi.ssid, wifi.password);
  }
  else
  {
    WiFi_initAP_STA();
  }
}
void printScanResult(int networksFound)
{
  //Serial.print("[WIFI] SEARCHING\n");
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
  snprintf(line2, 16, PSTR("CONNECT TO WIFI!"));
  wifi_search.detach();
  state.wifi = 1;
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
    snprintf(line2, 16, "CONNECTING...:%i", wifi.countDS);
  }
  else
  {
    WiFi_initAP_STA();
    wifi_search.attach(10, search_wifi);
    wifi.countDS = recTime;
  }
}
void onStationGotIp(const WiFiEventStationModeGotIP &evt)
{
  //wifi.rssi = WiFi.RSSI();
  lcd.createChar(1, z_wifi_sta);
  if (strcmp(wifi.ip_str, "") && strcmp(wifi.ip_gw_str, ""))
    strcpy(flag.ip_type, PSTR("STATIC"));
  else
    strcpy(flag.ip_type, PSTR("DHCP"));
  Serial.print(F("[WIFI] STATION GOT IP ["));
  Serial.print(flag.ip_type);
  Serial.println(PSTR("]: ") + evt.ip.toString());
  state.wifi = 1;
}
void onProbeRequestReceive(const WiFiEventSoftAPModeProbeRequestReceived &evt)
{
  //Serial.print("CREATE AP\n");
  lcd.createChar(1, z_wifi_ap);
  wifi.rssi = evt.rssi;
  state.wifi = 2;
}