#include "ambilightNode.h"

#if defined(ESP32)
  #define AMBILIGHT_PIN 27
#endif

static WS2812FX *ws2812fx;

  ambilightNode::ambilightNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
    ambilightLedCount = new HomieSetting<long>("ambilightLedCount", "Count WS2812 led (range:1-1000 default:18)");
    ambilightLedColor = new HomieSetting<const char*>("ambilightLedColor", "Color led hex range 0x000000:0xFFFFFF");
    ambilightLedBrightness = new HomieSetting<long>("ambilightLedBrightness", "Brightness led range 0:255");
    ambilightLedSpeed = new HomieSetting<long>("ambilightLedSpeed", "Speed FX range 2:60000");
    ambilightLedModeFX = new HomieSetting<long>("ambilightLedModeFX", "Mode FX range 0:54");
  }

bool ambilightNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "color"){
      unsigned long color = strtoul(value.c_str(), 0, 16);
      setProperty("color").send(String(value));
      ws2812fx->setColor(color);
  }
  if(property == "brightness"){
    uint8_t brightness = (uint8_t)value.toInt();
    if( brightness >= 0 && brightness <= 255 ){
      setProperty("brightness").send(String(brightness));
      ws2812fx->setBrightness(brightness);
    }
  }
  if(property == "speed"){
    uint16_t speed = (uint16_t)value.toInt();
    if( speed >= 2 && speed <= 60000 ){
      setProperty("speed").send(String(value));
      ws2812fx->setSpeed(speed);
    }
  }
  if(property == "modefx"){
    uint8_t modefx = (uint16_t)value.toInt();
    if( modefx >= 0 && modefx <= 54 ){
      setProperty("modefx").send(String(modefx));
      ws2812fx->setMode(modefx);
    }
  }
  return true;
}

void ambilightNode::ambilightSetup(){
  ambilightLedCount->setDefaultValue(18).setValidator([](float candidateCount) {
    return (candidateCount >= 1) && (candidateCount <= 1000);
  });
    ambilightLedColor->setDefaultValue("0x000000").setValidator([] (const char* candidateLedColor) {
    return (strlen(candidateLedColor) == 8 && candidateLedColor[0] == '0' && candidateLedColor[1] == 'x');
  });
    ambilightLedBrightness->setDefaultValue(100).setValidator([] (long candidateLedBrightness) {
    return (candidateLedBrightness >= 0 && candidateLedBrightness <= 255);
	});
  ambilightLedSpeed->setDefaultValue(1000).setValidator([] (long candidateLedSpeed) {
      return (candidateLedSpeed >= 2 && candidateLedSpeed <= 60000);
  });
  ambilightLedModeFX->setDefaultValue(0).setValidator([] (long candidateLedModeFX) {
      return (candidateLedModeFX >= 0 && candidateLedModeFX <= 54);
  });
}

void ambilightNode::setup() {
      ws2812fx = new WS2812FX(ambilightLedCount->get(), AMBILIGHT_PIN, NEO_GRB + NEO_KHZ800);
      ws2812fx->init();
      String confColor = ambilightLedColor->get();
      ws2812fx->setColor(strtoul(confColor.c_str(), 0, 16));
      ws2812fx->setBrightness((uint8_t)ambilightLedBrightness->get());
      ws2812fx->setSpeed(ambilightLedSpeed->get());
      ws2812fx->setMode(ambilightLedModeFX->get());

#ifdef ARDUINO_ARCH_ESP32
      rmt_tx_int(RMT_CHANNEL_0, ws2812fx->getPin());
      ws2812fx->setCustomShow(myCustomShow);
#endif
    ws2812fx->start();
    //advertise("count").setRetained(true).setFormat("0:1000").settable(); //the quantity is set at the setup stage
    advertise("color").setRetained(true).setFormat("0x000000:0xFFFFFF").settable();
    advertise("brightness").setRetained(true).setFormat("0:255").settable();
    advertise("speed").setRetained(true).setFormat("2:60000").settable();
    advertise("modefx").setRetained(true).setFormat("0:54").settable();
}

#ifdef ARDUINO_ARCH_ESP32
void ambilightNode::myCustomShow() {
  uint8_t *pixels = ws2812fx->getPixels();
  uint16_t numBytes = ws2812fx->getNumBytes() + 1;
  rmt_write_sample(RMT_CHANNEL_0, pixels, numBytes, false); // channel 0
}
#endif

void ambilightNode::loop() {
  ws2812fx->service();
}
