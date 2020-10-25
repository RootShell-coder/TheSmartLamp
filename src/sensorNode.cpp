#include "sensorNode.h"

#if defined(ESP8266)
  //HSPI ESP8266
  #define SPI_SS        D4
#endif

#if defined(ESP32)
  #define SPI_MOSI      23  //sda
  #define SPI_MISO      19  //sdo
  #define SPI_SCK       18  //scl
  #define SPI_SS        5   //csb
#endif

BME280Spi::Settings settings(SPI_SS);
BME280Spi bme(settings);

sensorNode::sensorNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
  sensorInterval = new HomieSetting<long>("sensorInterval", "Read sensor interval in ms (range:10000-65000 default:65000)");
}

bool sensorNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "interval"){
    uint16_t interval = (uint16_t)value.toInt();
    if( interval >= 10000 && interval <= 65000 ){
      setProperty("interval").send(String(interval));
      sensorInterval->setDefaultValue(interval);
    }
  }
return true;
}
void sensorNode::sensorSetup(){
    sensorInterval->setDefaultValue(65000).setValidator([](float candidate) {
    return (candidate >= 10000) && (candidate <= 65000);
  });
}

void sensorNode::setup(){
  #if defined(ESP8266)
    SPI.begin();
  #endif
  #if defined(ESP32)
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  #endif

    bme.begin();
    switch(bme.chipModel()){
      case BME280::ChipModel_BME280:
        settings.tempOSR = BME280::OSR_X16;
        settings.presOSR = BME280::OSR_X16;
        settings.humOSR  = BME280::OSR_X16;
        settings.filter  = BME280::Filter_16;
        bme.setSettings(settings);
          advertise("BME280").setName("Sensors").setRetained(true).setFormat("json");
      break;
      case BME280::ChipModel_BMP280:
          advertise("BMP280").setName("Sensors").setRetained(true).setFormat("json");
      break;
      default:
        sensorEnable = false;
  }
  advertise("interval").setRetained(true).setFormat("10000:65000").settable();
}

void sensorNode::loop(){
  if(!sensorEnable){return;}

  if (millis() - lastSensorLoop >= (unsigned long) sensorInterval->get() || lastSensorLoop == 0) {
    lastSensorLoop = millis();
    bme.read(pressure, temperature, humidity, BME280::TempUnit_Celsius, BME280::PresUnit_torr);

    if(bme.chipModel() == BME280::ChipModel_BME280){
      setProperty("BME280").send("{\"Temperature\":"+String(temperature)+",\"Pressure\":"+String(pressure)+",\"Humidity\":"+String(humidity)+"}");
      Homie.getLogger()<< "  • " << "Temperature: " << temperature << "°C " << " Pressure: " << pressure << "Torr " << " Humidity: " << humidity << "%RH";
    }

    if(bme.chipModel() == BME280::ChipModel_BMP280){
      setProperty("BMP280").send("{\"Temperature\":"+String(temperature)+",\"Pressure\":"+String(pressure)+"}");
      Homie.getLogger()<< "  • " << "Temperature: " << temperature << "°C " << " Pressure: " << pressure << "Torr ";
    }
  }
}
