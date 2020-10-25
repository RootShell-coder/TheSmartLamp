#ifndef PINGNODE_H
#define PINGNODE_H
#include <Homie.h>
class pingNode : public HomieNode {
    public:
        explicit pingNode(const char *id, const char *name, const char *type);
        void pingSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *pingInterval;
        unsigned long lastPingLoop;
        unsigned long pingTime;
};
#endif
