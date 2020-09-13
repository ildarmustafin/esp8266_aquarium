void initSTA() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //WiFi.softAPdisconnect(true);
  //==========================================================================================================
  //                                                 MODE STA
  //==========================================================================================================
  WiFi.mode(WIFI_STA);
  String currentIP;
  if (ip_str != "" && ip_gw_str != "") {
    IPAddress ip_subnet(255, 255, 255, 0);
    WiFi.config(ip, ip_gw, ip_subnet, ip_gw);
  }
  if (_ssid != "" && _password != "") {
    WiFi.begin(_ssid.c_str(), _password.c_str());
  }
  Serial.println("");
  while (--tries && WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    sprintf(line1, "CONNECTING: %02d ", tries);
    sprintf(line2, "      VER: %s", ver);
    printLCD(2, 0, 0, line1, line2, 1000);
  }
  tries = 11;
  if (WiFi.status() != WL_CONNECTED)  {
    StartAPMode();
  }
  else {
    currentIP = WiFi.localIP().toString();
    wifi_working = 1;
    printLCD(2, 0, 0, "                ", "                ", 0);
    lcd.createChar(1, znak_wifi_sta);
    Serial.printf("\nWIFI CONNECTED TO \"%s\"\nIP ADDRESS: %s \nPORT: %i\n", _ssid.c_str(), currentIP.c_str(), port);
    printLCD(2, 0, 0, "                ", "                ", 0);
    printLCD(2, 0, 0, "CONNECTED TO:", _ssid, 1000);
    printLCD(2, 0, 0, "                ", "                ", 0);
    printLCD(2, 0, 0, currentIP, "PORT: " + String(port), 1500);
    printLCD(2, 0, 0, "                ", "                ", 0);
  }
}
//==========================================================================================================
//                                                 MODE AP
//==========================================================================================================
bool StartAPMode() {
  wifi_working = 0;
  lcd.createChar(1, znak_wifi_ap);
  Serial.println("");
  Serial.printf("AP CREATED SUCCESSFULLY \nSSID: %s | PASSWORD: EMPTY \nIP ADDRESS: 192.168.4.1 \n", _ssidAP.c_str(), _passwordAP.c_str());
  printLCD(2, 0, 0, "                ", "                ", 0);
  printLCD(2, 0, 0, "AP CREATE", _ssidAP, 1000);
  printLCD(2, 0, 0, "                ", "                ", 0);
  printLCD(2, 0, 0, "IP: 192.168.4.1", "PORT: " + String(port), 1500);
  printLCD(2, 0, 0, "                ", "                ", 0);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  return true;
}
