void WIFIinit() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  //wifi_set_sleep_type(NONE_SLEEP_T);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //==========================================================================================================
  //                                                 MODE STA
  //==========================================================================================================
  WiFi.mode(WIFI_STA);
  _ssid = jsonReadToStr(configSetup, "input", 13);
  _password = jsonReadToStr(configSetup, "input", 14);
  if (_ssid != "" && _password != "") {
    WiFiMulti.addAP(_ssid.c_str(), _password.c_str());
  }
  Serial.println("");
  while (--tries && WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    sprintf(line1, "CONNECTING: %02d", tries);
    sprintf(line2, "      VER: %s", ver);
    printLCD(2, 0, 0, line1, line2, 1000);
  }
  tries = 11;
  if (WiFiMulti.run() != WL_CONNECTED)  {
    StartAPMode();
  }
  else {
    printLCD(2, 0, 0, "                ", "                ", 0);
    wifi_working = 1;
    ip =  WiFi.localIP();
    lcd.createChar(1, znak_wifi_sta);
    Serial.printf("\nWIFI CONNECTED TO \"%s\"\nIP ADDRESS: %d.%d.%d.%d \nPORT: %i\n", _ssid.c_str(), ip[0], ip[1], ip[2], ip[3], port);
    printLCD(2, 0, 0, "                ", "                ", 0);
    printLCD(2, 0, 0, "CONNECTED TO:", _ssid, 1000);
    printLCD(2, 0, 0, "                ", "                ", 0);
    printLCD(2, 0, 0, ip.toString(), "PORT: " + String(port), 1500);
    printLCD(2, 0, 0, "                ", "                ", 0);
    jsonWrite(configSetup, "ip", ip.toString());
  }
}
//==========================================================================================================
//                                                 MODE AP
//==========================================================================================================
bool StartAPMode() {
  String _ssidAP = jsonReadToStr(configSetup, "input", 11);
  String _passwordAP = jsonReadToStr(configSetup, "input", 12);
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
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  return true;
}
