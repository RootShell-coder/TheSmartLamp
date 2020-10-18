#ifndef LEDNODE_H
#define LEDNODE_H
#include <Homie.h>

#if defined(ESP8266)
#endif

#if defined(ESP32)
 // #include <driver/rmt.h>
#endif

class ledNode : public HomieNode {
    public:
        explicit ledNode(const char *id, const char *name, const char *type);
        void ledSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *warmLed;
        HomieSetting<long> *coldLed;
        HomieSetting<long> *fadeLed;
        uint16_t setWarm, setCold, saveWarm, saveCold, fade;
        unsigned long currentMicros = micros();
        void LedStripUpdate();
        void LedWriteWarm(uint16_t value);
        void LedWriteCold(uint16_t value);
};
#endif