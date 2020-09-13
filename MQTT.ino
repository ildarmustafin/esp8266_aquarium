void send_values_by_mqtt() {
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(),  jsonLive.c_str());
    mqttcount = 0;
    mqtt_working = 1;
    if (mqttFlag == 0) Serial.printf("Установлено MQTT-соединение!\n");
    mqttFlag = 1;
  } else {
    mqttcount++;
    mqtt_working = 0;
    if (mqttFlag == 1) Serial.printf("MQTT-соединение разорвано!\n");
    mqttFlag = 0;
  }
  if (mqttcount == 60 && !mqttClient.connected()) {
    mqttClient.connect(mqtt_ID.c_str(), mqtt_user.c_str(), mqtt_password.c_str());
    mqttcount = 0;
  }
}
