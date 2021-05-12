#include "relay.h"
#ifdef ARDUINO_ARCH_ESP8266
  //crock nodemcu
  #define RELAY0 D0
  //#define RELAY1 D1
#endif

#ifdef ARDUINO_ARCH_ESP32
  #define RELAY0 16
  //#define RELAY1 17
#endif

  relayNode::relayNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
    relay0 = new HomieSetting<long>("relay0", "State relay0 (range: 0,1)");
  }

bool relayNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "relay0"){
    if((uint8_t)value.toInt() == 0 || (uint8_t)value.toInt() == 1){
      digitalWrite(RELAY0, value.toInt() ? HIGH : LOW);
      setProperty("relay0").send(value);
    return true;
    }
  }

return false;
}

void relayNode::setup(){
  pinMode(RELAY0, OUTPUT);

  advertise("relay0").setRetained(true).setFormat("0:1").settable();
}

void relayNode::relaySetup(){
    relay0->setDefaultValue(0).setValidator([] (uint8_t candidate) {
    return (candidate == 0 || candidate == 1);
  });
}

void relayNode::loop(){
}