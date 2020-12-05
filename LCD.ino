void printLCD(byte colEmpty, byte col, char* line1, char* line2) {
  switch (colEmpty) {
    case 1:
      lcd.setCursor(col, 1);
      lcd.print(line2);
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(col, 1);
      lcd.print(line2);
      break;
  }
}

void updateZnak(byte printB, bool value, byte col, byte row) {
  if (value) {
    lcd.setCursor(col, row);
    lcd.printByte(printB);
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
  lcd.createChar(2, znak_gradus);
  lcd.createChar(3, znak_led);
  lcd.createChar(4, znak_fan);
  lcd.createChar(5, znak_ten);
  lcd.createChar(6, znak_ws);
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
