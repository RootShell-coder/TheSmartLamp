#include <Homie.h>
#include "pingNode.h"
#include "sensorNode.h"
#include "remoteNode.h"
#include "ambilightNode.h"

#define BRAND     "HSL"
#define FM_NAME   "The Smart Lamp"
#define FM_VER    "0.0.2"

pingNode PingNode("network", "connection", "ping");
sensorNode SensorNode("sensor", "environment", "BME280");
remoteNode RemoteNode("remote", "control", "IR");
ambilightNode AmbilihgtNode("ambilight", "Led", "WS2812B");

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setBrand(BRAND);
  Homie_setFirmware(FM_NAME, FM_VER);
  PingNode.pingSetup();
  SensorNode.sensorSetup();
  RemoteNode.remoteSetup();
  AmbilihgtNode.ambilightSetup();
  Homie.setup();
}

void loop() {
  Homie.loop();
}