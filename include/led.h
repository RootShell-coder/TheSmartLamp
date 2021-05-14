#ifndef LED_H
#define LED_H
#include <Homie.h>


class ledNode : public HomieNode {
    public:
        ledNode(const char *id, const char *name, const char *type);
        void ledSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *warmLed;
        HomieSetting<long> *coldLed;
        HomieSetting<long> *fadeLed;

        unsigned long saveMicros = micros();
        uint16_t  warm, _warm, cold, _cold, fade;
        void ledUpdate();
        //void coldUpdate();
        void LedWriteWarm(uint16_t &value);
        void LedWriteCold(uint16_t &value);
};
#endif
