/*
    WIFI_EVENT_STAMODE_CONNECTED          = 0,
    WIFI_EVENT_STAMODE_DISCONNECTED       = 1,
    WIFI_EVENT_STAMODE_AUTHMODE_CHANGE    = 2,
    WIFI_EVENT_STAMODE_GOT_IP             = 3,
    WIFI_EVENT_STAMODE_DHCP_TIMEOUT       = 4,
    WIFI_EVENT_SOFTAPMODE_STACONNECTED    = 5,
    WIFI_EVENT_SOFTAPMODE_STADISCONNECTED = 6,
    WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED  = 7,
    WIFI_EVENT_MAX
*/

void WiFiEvent(WiFiEvent_t event) {
  char currentIP[16];
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      disconnectCounter = reconnectTime;
      lcd.createChar(1, znak_wifi_sta);
      Serial.printf("WIFI CONNECTED TO \"%s\"\n", ssid.c_str());
      printLCD(2, 0, "CONNECTED... IP:", "                ");
      if (WiFi.getAutoConnect() != true) {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
      }
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:
      wifi_working = 1;
      sprintf(currentIP, "%s", WiFi.localIP().toString().c_str());
      Serial.printf("IP ADDRESS: %s \nPORT: %i\n", currentIP, port);
      printLCD(2, 0, "CONNECTED... IP:", currentIP);
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      wifi_working = 2;
      Serial.printf("CONNECTING...TRY:%i\n", disconnectCounter);
      sprintf(line2, " CONNECTING: %i  ", disconnectCounter);
      printLCD(2, 0, "  PLEASE WAIT   ", line2);
      WiFi.reconnect();
      disconnectCounter--;
      if (!disconnectCounter) {
        disconnectCounter = reconnectTime;
        initAP();
      }
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
      wifi_working = 0;
      WiFi.scanNetworksAsync(prinScanResult);
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      Serial.printf("\nINITIALIZING WIFI MODULE\n");
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
      Serial.printf("CLIENT CONNECTED\n");
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
      Serial.printf("CLIENT DISCONNECTED\n");
      break;
  }
}

void initSTA() {
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  if (ip_str != "" && ip_gw_str != "") {
    IPAddress ip_subnet(255, 255, 255, 0);
    WiFi.config(ip, ip_gw, ip_subnet, ip_gw);
  }
  //Serial.printf("\nssid: %s | password: %s\n", ssid.c_str(), password.c_str());
  if (ssid != "" && password != "") {
    WiFi.begin(ssid, password);
  }
}

void initAP() {
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssidAP, passwordAP);
  lcd.createChar(1, znak_wifi_ap);
  Serial.printf("\nAP CREATED SUCCESSFULLY \nSSID: %s | PASSWORD: %s \nIP ADDRESS: 192.168.4.1\n", ssidAP.c_str(), passwordAP.c_str());
  printLCD(2, 0, "   AP CREATED   ", "IP: 192.168.4.1 ");
}


void prinScanResult(int networksFound) {
  for (int i = 0; i < networksFound; i++) {
    if ( WiFi.SSID(i) == ssid.c_str()) {
      Serial.printf("\nFOUND \"%s\"\nCONNECTING...PLEASE WAIT...\n", ssid.c_str());
      initSTA();
    }
  }
}
