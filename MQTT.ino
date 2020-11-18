void send_values_by_mqtt() {
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(), 0, true, jsonLive.c_str());
  } else {
    mqttClient.connect();
  }
}
void onMqttConnect(bool sessionPresent) {
  mqtt_working = 1;
  if (!mqttFlag && mqtt_working) {
    Serial.printf("Установлено MQTT-соединение!\n");
    mqttFlag = 1;
  }
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  mqtt_working = 0;
  if (mqttFlag && !mqtt_working) {
    Serial.printf("MQTT-соединение разорвано!\n");
    mqttFlag = 0;
  }
}
