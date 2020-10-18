#include "ledNode.h"

//#define DEBUG

#if defined(ESP32)
#define WARM 32
#define COLD 33
  const int freq = 30000;
  const int ledChannelWarm = 0;
  const int ledChannelCold = 1;
  const int resolution = 10; //Resolution 8, 10, 12, 15
#endif

  ledNode::ledNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
    warmLed = new HomieSetting<long>("warm", "The brightness of the LED warm (PWM range: 0-1024 default 0)");
    coldLed = new HomieSetting<long>("cold", "The brightness of the LED cold (PWM range: 0-1024 default 0)");
    fadeLed = new HomieSetting<long>("fade", "The time fade of light (Fade LED range 0-60000");
  }

bool ledNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "warm"){
    uint16_t Warm = (uint16_t)value.toInt();
    if(Warm >= 0 && Warm <= 1024){
      setProperty("warm").send(String(Warm));
      setWarm = Warm;
    }else{ setWarm = 0; }
  }

  if(property == "cold"){
    uint16_t Cold = (uint16_t)value.toInt();
    if(Cold >= 0 && Cold <= 1024){
      setProperty("cold").send(String(Cold));
      setCold = Cold;
    }else{ setCold = 0; }
  }

  if(property == "fade"){
    fade = (uint8_t)value.toInt();
    if(fade >= 0 && fade <= 60000){
      setProperty("fade").send(String(fade));
    }
  }
  return true;
}

void ledNode::ledSetup(){
    warmLed->setDefaultValue(0).setValidator([] (long candidateWarm) {
    return (candidateWarm >= 0 && candidateWarm <= 1024);
  });
    coldLed->setDefaultValue(0).setValidator([] (long candidateCold) {
    return (candidateCold >= 0 && candidateCold <= 1024);
  });
    fadeLed->setDefaultValue(1000).setValidator([] (long candidateFade) {
    return (candidateFade >= 0 && candidateFade <= 60000);
  });
}

void ledNode::setup() {
  pinMode(WARM,OUTPUT);
  pinMode(COLD,OUTPUT);
  fade = fadeLed->get();
#if defined(ESP32)
  ledcSetup(ledChannelWarm, freq, resolution);
  ledcSetup(ledChannelCold, freq, resolution);
  ledcAttachPin(WARM, ledChannelWarm);
  ledcAttachPin(COLD, ledChannelCold);
#endif
  advertise("warm").setRetained(true).setFormat("0:1024").settable();
  advertise("cold").setRetained(true).setFormat("0:1024").settable();
  advertise("fade").setRetained(true).setFormat("0:60000").settable();
}

void ledNode::loop(){
  LedStripUpdate();
}

void ledNode::LedStripUpdate(){
  while(setWarm > saveWarm){ //Fade Warm on
    if(fade == 0){
      saveWarm = setWarm;
      LedWriteWarm(saveWarm);
    }else{
      if(micros() - currentMicros > fade){
        ++saveWarm;
        LedWriteWarm(saveWarm);
        currentMicros = micros();
      }
    }
  }

  while(setWarm < saveWarm){ //Fade Warm off
    if(fade == 0){
      saveWarm = setWarm;
      LedWriteWarm(saveWarm);
    }else{
      if(micros() - currentMicros > fade){
        --saveWarm;
        LedWriteWarm(saveWarm);
        currentMicros = micros();
      }
    }
  }

  while(setCold > saveCold){ //Fade Cold on
      if(fade == 0){
        saveCold = setCold;
        LedWriteCold(saveCold);
    }else{
      if(micros() - currentMicros > fade){
        ++saveCold;
        LedWriteCold(saveCold);
        currentMicros = micros();
      }
    }
  }

  while(setCold < saveCold){ //Fade Cold off
    if(fade == 0){
        saveCold = setCold;
        LedWriteCold(saveCold);
    }else{
      if(micros() - currentMicros > fade){
        --saveCold;
        LedWriteCold(saveCold);
        currentMicros = micros();
      }
    }
  }
}

void ledNode::LedWriteWarm(uint16_t value){
#ifdef DEBUG
    Serial.println(value);
#endif
#if defined(ESP8266)
    analogWrite(WARM, value);
#endif
#if defined(ESP32)
    ledcWrite(ledChannelWarm, value);
#endif
}

void ledNode::LedWriteCold(uint16_t value){
#ifdef DEBUG
    Serial.println(value);
#endif
#if defined(ESP8266)
    analogWrite(WARM,value);
#endif
#if defined(ESP32)
    ledcWrite(ledChannelCold, value);
#endif
}