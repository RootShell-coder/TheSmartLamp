#include "pingNode.h"

pingNode::pingNode(const char* id, const char* name, const char* type) : HomieNode(id, name, type){
  pingInterval = new HomieSetting<long>("pingInterval", "Execute ping interval in ms (range:5000-60000 default:60000)");
}

bool pingNode::handleInput(const HomieRange& range, const String& property, const String& value){
    if(property == "ping"){
        float time = (float)(micros() - pingTime)/1000;
        Homie.getLogger() << "  • " << property << " from mqtt = "<< time << " ms" << endl;
        setProperty("ping").send(String(time));
    }
    if(property == "interval"){
        uint16_t interval = (uint16_t)value.toInt();
        if( interval >= 5000 && interval <= 60000 ){
            setProperty("interval").send(String(interval));
            pingInterval->setDefaultValue(interval);
        }
    }
  return true;
}

void pingNode::pingSetup(){
    pingInterval->setDefaultValue(60000).setValidator([](float candidate) {
    return (candidate >= 5000) && (candidate <= 60000);
  });
}

void pingNode::setup(){
    advertise("ping").setRetained(true).settable();
    advertise("interval").setRetained(true).setFormat("5000:60000").settable();
}

void pingNode::loop(){
  if (millis() - lastPingLoop >= (unsigned long) pingInterval->get() || lastPingLoop == 0) {
    lastPingLoop = millis();
    pingTime = micros();
    setProperty("ping/set").send(F("echo"));
  }
}