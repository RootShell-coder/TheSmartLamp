#ifndef SENSORNODE_H
#define SENSORNODE_H
#include <Homie.h>
#include <SPI.h>
#include <BME280Spi.h>

class sensorNode : public HomieNode {
    public:
        explicit sensorNode(const char *id, const char *name, const char *type);
        void sensorSetup();

    protected:
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value) override;
        virtual void setup() override;
        virtual void loop() override;

    private:
        HomieSetting<long> *sensorInterval;
        unsigned long lastSensorLoop;
        float temperature = NAN,
              humidity = NAN,
              pressure = NAN;
        bool sensorEnable = true;
};
#endif
