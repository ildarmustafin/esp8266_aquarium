void WiFi_initAP_STA()
{
  snprintf(line2, 16, "   CREATE AP!   ");
  SERIAL_PRINT("[WIFI] INIT AP MODE\n");
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifi.ssidAP, wifi.passwordAP);
}
void WiFi_initSTA()
{
  SERIAL_PRINT("[WIFI] INIT STA MODE\n");
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
  DEBUG_PRINT("[WIFI] SEARCHING\n");
  for (int i = 0; i < networksFound; i++)
  {
    if (WiFi.SSID(i) == wifi.ssid)
    {
      SERIAL_PRINT("\n[WIFI] FOUND \"%s\"\n[WIFI] CONNECTING...PLEASE WAIT...\n", WiFi.SSID(i).c_str());
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
  SERIAL_PRINT("[WIFI] STATION CONNECTED TO: %s\n", evt.ssid.c_str());
  snprintf(line2, 16, "CONNECT TO WIFI!");
  wifi_search.detach();
  state.wifi = 1;
}
void onStationDisconnected(const WiFiEventStationModeDisconnected &evt)
{
  //WiFi.reconnect();
  state.wifi = 0;
  if (--wifi.countDS)
  {
    SERIAL_PRINT("[WIFI] CONNECTING...TRY:%i\n", wifi.countDS);
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
    strcpy(flag.ip_type, "STATIC");
  else
    strcpy(flag.ip_type, "DHCP");
  SERIAL_PRINT("[WIFI] STATION GOT IP [%s]: %s\n", flag.ip_type, evt.ip.toString().c_str());
  state.wifi = 1;
}
void onProbeRequestReceive(const WiFiEventSoftAPModeProbeRequestReceived &evt)
{
  //SERIAL_PRINT("CREATE AP\n");
  lcd.createChar(1, z_wifi_ap);
  wifi.rssi = evt.rssi;
  state.wifi = 2;
}