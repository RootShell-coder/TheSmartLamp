#include <Homie.h>
#include "pingNode.h"
#include "sensorNode.h"
#include "remoteNode.h"
#include "ambilightNode.h"
#include "ledNode.h"

/*
* Smart lamp with MQTT IR devices control
* Original idea @ElizabethMorves こんばんは [konbanwa]
* Remake author: RootShell-coder <Root.Shelling@gmail.com>
*/

#define BRAND     "TSL"
#define FM_NAME   "The Smart Lamp"
#define FM_VER    "0.0.5"

pingNode PingNode("network", "connection", "ping");
sensorNode SensorNode("sensor", "environment", "BME280");
remoteNode RemoteNode("remote", "control", "IR");
ambilightNode AmbilihgtNode("ambilight", "Led", "WS2812B");
ledNode LedNode("light", "Led", "PWM");

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setBrand(BRAND);
  Homie_setFirmware(FM_NAME, FM_VER);

  PingNode.pingSetup();
  SensorNode.sensorSetup();
  RemoteNode.remoteSetup();
  AmbilihgtNode.ambilightSetup();
  LedNode.ledSetup();

  Homie.setup();
}

void loop() {
  Homie.loop();
}
