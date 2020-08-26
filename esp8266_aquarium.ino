#include "include_define.h"
#include "rtc.h"
#include "ruswords.h"
//==========================================================================================================//
//                                                 INITIAL SETUP                                            //
//==========================================================================================================//
void setup() {
  analogWriteFreq(100);  //new_frequency = 100..40000Hz     
  pinMode(LEDPIN, OUTPUT);
  pinMode(FANPIN, OUTPUT);
  pinMode(TENPIN, OUTPUT);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  analogWrite(LEDPIN, 0);
  digitalWrite(FANPIN, LOW);
  digitalWrite(TENPIN, LOW);
  digitalWrite(RELAY1PIN, LOW);
  digitalWrite(RELAY2PIN, LOW);
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args,BYTE);
#endif
  initLCD();
  FS_init();
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  configSetup  = readFile("configSetup.json", 4096);
  configChart  = readFile("configChart.json", 4096);
  if (configSetup == "Failed") {
    Serial.printf("\nERROR OPENING configSetup.json\n");
    printLCD(2, 0, 0, "ERROR OPENING   ", "configSetup.json", 1000);
    printLCD(2, 0, 0, "                ", "                ", 0);
  } else {
    readJsonValues();
  }
  if (configChart == "Failed") {
    Serial.printf("ERROR OPENING configChart.json\n");
    printLCD(2, 0, 0, "ERROR OPENING   ", "configChart.json", 1000);
    printLCD(2, 0, 0, "                ", "                ", 0);
  }
  server.begin(port);
  initSTA();
  if (WiFi.getAutoConnect() != true) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  sensors.begin();
  udp.begin(localPort);
  rtc.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  ts.add(0, 1000, [&](void *) {
    check_connection();
    measure_datetime();
  }, nullptr, true);
  ts.add(1, 500, [&](void *) {
    send_values_by_websocket();
  }, nullptr, true);

}
//==========================================================================================================//
//                                                 LOOP                                                     //
//==========================================================================================================//
void loop() {
  ts.update();
  server.handleClient();
  webSocket.loop();
  yield();
}
