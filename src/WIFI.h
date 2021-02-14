void WiFi_initAP_STA()
{
  snprintf(line2, 16, "   CREATE AP!   ");  
  DEBUG_PRINT("WIFI INIT AP\n");
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(wifi.ssidAP, wifi.passwordAP);
}
void WiFi_initSTA()
{ 
  DEBUG_PRINT("WIFI INIT STA\n");
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
  DEBUG_PRINT("SEARCHING\n");
  for (int i = 0; i < networksFound; i++)
  {
    if (WiFi.SSID(i) == wifi.ssid)
    {
      DEBUG_PRINT("\nFOUND \"%s\"\nCONNECTING...PLEASE WAIT...\n", WiFi.SSID(i).c_str());
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
  DEBUG_PRINT("STA CONNECTED\n");
  snprintf(line2, 16, "CONNECT TO WIFI!");  
  wifi_search.detach();
  wifi.working = 1;

}
void onStationDisconnected(const WiFiEventStationModeDisconnected &evt)
{
  //WiFi.reconnect();
  wifi.working = 0;
  if (--wifi.countDS)
  {
    DEBUG_PRINT("CONNECTING...TRY:%i\n", wifi.countDS);
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
  lcd.createChar(1, z_wifi_sta);
  DEBUG_PRINT("onStationGotIp\n");
  wifi.rssi = WiFi.RSSI();
  wifi.working = 1;
}
void onProbeRequestReceive(const WiFiEventSoftAPModeProbeRequestReceived &evt)
{
  DEBUG_PRINT("CREATE AP\n");
  lcd.createChar(1, z_wifi_ap);
  wifi.rssi = evt.rssi;
  wifi.working = 1;
}