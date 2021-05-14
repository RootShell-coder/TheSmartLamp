#include "led.h"

#define LOG

#ifdef ESP8266
    //fix this
#endif

#ifdef ESP32
#define WARM 32
#define COLD 33
  const int freq = 30000;     //PWM hz
  const int channelWarm = 0;
  const int channelCold = 1;
  const int resolution = 10; //Resolution 8, 10, 12, 15 (10bit = 1024 value)
#endif

  ledNode::ledNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
    warmLed = new HomieSetting<long>("warm", "Warm light LEDs, brightness (range: 0-1023)");
    coldLed = new HomieSetting<long>("cold", "Cold light LEDs, brightness (range: 0-1023)");
    fadeLed = new HomieSetting<long>("fade", "LEDs ignition time (range 0-65000");
  }

bool ledNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "warm"){
    if((uint16_t)value.toInt() >= 0 && (uint16_t)value.toInt() <= 1023){
      warm = (uint16_t)value.toInt();
      setProperty("warm").send(String(warm));
    }
  }

  if(property == "cold"){
    if((uint16_t)value.toInt() >= 0 && (uint16_t)value.toInt() <= 1023){
      cold = (uint16_t)value.toInt();
      setProperty("cold").send(String(cold));
    }
  }

  if(property == "fade"){
    if((uint16_t)value.toInt() >= 0 && (uint16_t)value.toInt() <= 65000){
      fade = (uint16_t)value.toInt();
      setProperty("fade").send(String(fade));
    }
  }
  return true;
}

void ledNode::ledSetup(){
    warmLed->setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0 && candidate <= 1023);
  });
    coldLed->setDefaultValue(0).setValidator([] (long candidate) {
    return (candidate >= 0 && candidate <= 1023);
  });
    fadeLed->setDefaultValue(2000).setValidator([] (long candidate) {
    return (candidate >= 0 && candidate <= 65000);
  });
}

void ledNode::setup() {
  warm = warmLed->get();
  cold = coldLed->get();
  fade = fadeLed->get();

#ifdef ESP32
  pinMode(WARM, OUTPUT);
  pinMode(COLD, OUTPUT);
  ledcSetup(channelWarm, freq, resolution);
  ledcSetup(channelCold, freq, resolution);
  ledcAttachPin(WARM, channelWarm);
  ledcAttachPin(COLD, channelCold);
#endif
#ifdef ESP8266
  analogWrite(WARM, warm);
  analogWrite(COLD, cold);
#endif
  advertise("warm").setRetained(true).setFormat("0:1023").settable();
  advertise("cold").setRetained(true).setFormat("0:1023").settable();
  advertise("fade").setRetained(true).setFormat("0:65000").settable();

}

void ledNode::loop(){
  ledUpdate();
}

void ledNode::ledUpdate(){
  if(micros() - saveMicros > fade){
    if(_warm < warm){
        ++_warm;
        LedWriteWarm(_warm);
     }
    if(_cold < cold){
        ++_cold;
        LedWriteCold(_cold);
    }
    if(_warm > warm){
        --_warm;
        LedWriteWarm(_warm);
     }
    if(_cold > cold){
        --_cold;
        LedWriteCold(_cold);
    }
    saveMicros = micros();
  }
}


void ledNode::LedWriteWarm(uint16_t &value){
#ifdef LOG
if(_warm == warm){
  Homie.getLogger() << "🔶 warm: " << value << " pwm, " << fade << " us" << endl;
}
#endif
#ifdef ESP8266
    analogWrite(WARM, value);
#endif
#ifdef ESP32
    ledcWrite(channelWarm, value);
#endif
}

void ledNode::LedWriteCold(uint16_t &value){
#ifdef LOG
if(_cold == cold){
  Homie.getLogger() << "🔷 cold: " << value << " pwm, " << fade << " us" << endl;
}
#endif
#ifdef ESP8266
    analogWrite(COLD, value);
#endif
#ifdef ESP32
    ledcWrite(channelCold, value);
#endif
}
