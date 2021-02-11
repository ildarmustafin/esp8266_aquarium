void printLCD(byte colEmpty, const char *line1, const char *line2) {
  switch (colEmpty) {
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(line2);
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);
      break;
  }
}
void updateZnak(byte printB, bool value, byte col, byte row) {
  if (value) {
    lcd.setCursor(col, row);
    lcd.write(printB);
  } else {
    lcd.setCursor(col, row);
    lcd.print(" ");
  }
}
void updateZnak(String printB, bool value, byte col, byte row) {
  if (value) {
    lcd.setCursor(col, row);
    lcd.print(printB);
  } else {
    lcd.setCursor(col, row);
    lcd.print(" ");
  }
}
void initLCD() {
  lcd.begin();
  lcd.backlight();
  lcd.createChar(1, z_wifi_tc);  
  lcd.createChar(2, z_gradus);
  lcd.createChar(3, z_led);
  lcd.createChar(4, z_fan);
  lcd.createChar(5, z_ten);
  lcd.createChar(6, z_ws);
}
void initBar2() {
  lcd.createChar(0, left_empty);
  lcd.createChar(1, center_empty);
  lcd.createChar(2, right_empty);
  lcd.createChar(3, left_full);
  lcd.createChar(4, center_full);
  lcd.createChar(5, right_full);
}
void fillBar2(byte start_pos, byte row, byte bar_length, byte fill_percent) {
  byte infill = round((float)bar_length * fill_percent / 100);
  lcd.setCursor(start_pos, row);
  if (infill == 0) lcd.write(0);
  else lcd.write(3);
  for (int n = 1; n < bar_length - 1; n++) {
    if (n < infill) lcd.write(4);
    if (n >= infill) lcd.write(1);
  }
  if (infill == bar_length) lcd.write(5);
  else lcd.write(2);
}
void line_show() {
    char led_bright_char[5];
    snprintf(led_bright_char, 5, "%d", led_bright);
    snprintf(line1, 17, "%02d:%02d %s %-7s", lcd_hour, lcd_min, weekLang, temp_char);
    snprintf(line2, 12, "\1%-5d\3%-5s", wifi.rssi, led_bright_char);
    //snprintf(line2, 12, "\1%-5d\3%-5s", ESP.getHeapFragmentation(), led_bright_char);
    switch (bf_rel) {    // Знак Реле1_2
    case 0:
      updateZnak(8, 0, 11, 1);
      break;
    case 2:
      lcd.createChar(8, z_rel2);
      updateZnak(8, 1, 11, 1);
      break;
    case 1:
      lcd.createChar(8, z_rel1);
      updateZnak(8, 1, 11, 1);
      break;
    case 3:
      lcd.createChar(8, z_rel12);
      updateZnak(8, 1, 11, 1);
      break;
    }
    updateZnak(4,   bitRead(bf, 5), 12, 1); // Знак FAN
    updateZnak(5,   bitRead(bf, 4), 13, 1); // Знак TEN
    updateZnak("Q", bitRead(bf, 1), 14, 1); // Знак MQTT
    updateZnak(6,   ws_working,  15, 1);    // Знак WEBSOCKET
}

