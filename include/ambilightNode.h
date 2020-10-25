#ifndef AMBILIHGTNODE_H
#define AMBILIHGTNODE_H
#include <Homie.h>
#include <WS2812FX.h>

#if defined(ESP32)
  #include "ESP32_RMT_Driver.h"
#endif

class ambilightNode : public HomieNode {
    public:
        explicit ambilightNode(const char *id, const char *name, const char *type);
    #if defined(ESP32)
        static void myCustomShow();
    #endif
        void ambilightSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *ambilightLedCount;
        HomieSetting<const char*> *ambilightLedColor;
        HomieSetting<long> *ambilightLedBrightness;
        HomieSetting<long> *ambilightLedSpeed;
        HomieSetting<long> *ambilightLedModeFX;

};
#endif
