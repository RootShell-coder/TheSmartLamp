#ifndef REMOTENODE_H
#define REMOTENODE_H
#include <Homie.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>

#if defined(ESP8266)
#endif

#if defined(ESP32)
 // #include <driver/rmt.h>
#endif

class remoteNode : public HomieNode {
    public:
        explicit remoteNode(const char *id, const char *name, const char *type);
        void remoteSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *modulationFrequency;
        virtual bool parseStringAndSendRaw(const String str, uint16_t freq);
        virtual uint16_t countValuesInStr(const String str, char sep);
        virtual uint16_t *newCodeArray(const uint16_t size);
};
#endif