#include <Arduino.h>
#include <Homie.h>
#include "sensor.h"
#include "ambilight.h"
#include "led.h"
#include "relay.h"

#define BRAND "SmartLamp"
#define FW_NAME "Smart Lamp"
#define FW_VER "0.1.1"

void onHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::WIFI_DISCONNECTED:
      Serial << "Wi-Fi disconnected, reason: " << (int8_t)event.wifiReason << endl;
      break;
      case HomieEventType::MQTT_DISCONNECTED:
      Serial << "MQTT disconected, reason: " << (int8_t)event.mqttReason << endl;
      break;
  }
}

  sensorNode SensorNode("sensor","Sensor", "BME280");
  ambilightNode AmbilightNode("ambilight","Ambilight", "WS2812B");
  ledNode LedNode("led","Led", "PWM");
  relayNode RelayNode("relay","Relay", "Force-guided");

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setBrand(BRAND);
  Homie_setFirmware(FW_NAME, FW_VER);
  Homie.onEvent(onHomieEvent);
  SensorNode.sensorSetup();
  AmbilightNode.ambilightSetup();
  LedNode.ledSetup();
  RelayNode.relaySetup();
  Homie.setup();
}

void loop() {
  Homie.loop();
}
