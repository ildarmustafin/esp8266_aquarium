void ds18b20_measure() {
  byte type_s;
  byte data[12];
  byte addr[8];
  static unsigned long prevTime = 0;
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    return;
  }
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.write(0xCC);
  ds.write(0x44, 1);        // начало коммуникации

  if (millis() - prevTime > 1000) {
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);         // читаем значение
    for (byte i = 0; i < 9; i++) {
      data[i] = ds.read();
    }
    /*
      Serial.print(" CRC=");
      Serial.print(OneWire::crc8(data, 8), HEX);
      Serial.println();
    */
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3;
      if (data[7] == 0x10) {
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw & ~7;
      else if (cfg == 0x20) raw = raw & ~3;
      else if (cfg == 0x40) raw = raw & ~1;
    }
    tempC = (float)raw / 16.0 + temp_koef;
    temp_filtered = round(avg.update(tempC) * 10) / 10;
    prevTime = millis();
  }
}
