void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void send_values_by_mqtt() {
  sprintf(date_now, "%02i.%02i.%04i | %02i:%02i:%02i",  ds_day, ds_month, ds_year, ds_hour, ds_min, ds_sec);
  client.publish("now",  date_now);
  //Serial.println(date_now);
/*  
  client.publish("temp", String(temp_filtered).c_str());
  client.publish("led",  String(led_bright).c_str());
  client.publish("fan",  String(fan_working).c_str());
  client.publish("ten",  String(ten_working).c_str());
  client.publish("rel1", String(relay1_working).c_str());
  client.publish("rel2", String(relay2_working).c_str());
  client.publish("rssi", String(rssi).c_str());  
  */    
}
void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266_str";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("#");
    } else {
      delay(1000);
    }
  }
}
