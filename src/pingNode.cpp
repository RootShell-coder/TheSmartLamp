#include "pingNode.h"

pingNode::pingNode(const char* id, const char* name, const char* type) : HomieNode(id, name, type){
  pingInterval = new HomieSetting<long>("pingInterval", "Execute ping interval in ms (range:5000-65000 default:65000)");
}

bool pingNode::handleInput(const HomieRange& range, const String& property, const String& value){
    if(property == "ping"){
        float time = (float)(micros() - pingTime)/1000;
        Homie.getLogger() << "  • " << property << " from mqtt = "<< time << " ms" << endl;
        setProperty("ping").send(String(time));
    }
    if(property == "interval"){
        if( (uint16_t)value.toInt() >= 5000 && (uint16_t)value.toInt() <= 65000 ){
          uint16_t interval = (uint16_t)value.toInt();
          setProperty("interval").send(String(interval));
          pingInterval->setDefaultValue(interval);
        }
    }
  return true;
}

void pingNode::pingSetup(){
    pingInterval->setDefaultValue(65000).setValidator([](float candidate) {
    return (candidate >= 5000) && (candidate <= 65000);
  });
}

void pingNode::setup(){
    advertise("ping").setRetained(false).settable();
    advertise("interval").setRetained(true).setFormat("5000:65000").settable();
}

void pingNode::loop(){
  if (millis() - lastPingLoop >= (unsigned long) pingInterval->get() || lastPingLoop == 0) {
    lastPingLoop = millis();
    pingTime = micros();
    setProperty("ping/set").send(F("echo"));
  }
}
