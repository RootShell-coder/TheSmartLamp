#include "remoteNode.h"

//#define DEBUG
const uint16_t RecvPin = 14;
const uint16_t IrLedPin = 12;
const uint16_t CaptureBufferSize = 1024;
const uint8_t Timeout = 50;
const uint16_t Frequency = 38000;

IRsend irsend(IrLedPin);
IRrecv irrecv(RecvPin, CaptureBufferSize, Timeout, false);
decode_results results;

remoteNode::remoteNode(const char* id, const char* name, const char* type): HomieNode(id, name, type){
  modulationFrequency = new HomieSetting<long>("modulationFrequency", "Set IR modulation frequency in kHz (TSOP = 17, 22, 24, 36, 38, 44, 48 default:38)");
}

bool remoteNode::handleInput(const HomieRange& range, const String& property, const String& value){
  if(property == "frequency"){
      uint16_t frequency = (uint16_t)value.toInt();
        if( frequency == 17 || frequency == 22 || frequency == 24 || frequency == 36 || frequency == 38 || frequency == 44 || frequency == 48 ){
            setProperty("frequency").send(String(frequency));
            modulationFrequency->setDefaultValue(frequency);
        }
  }

  if(property == "oscillator"){
    parseStringAndSendRaw(value.c_str(), modulationFrequency->get());
/*
  uint16_t rawData[] = {//thomson led TV power on/off
    4010,3904,564,1926,554,1916,564,1916,566,1926,556,922,566,924,564,1916,564,926,562,1918,562,934,554,1920,562,932,556,924,564,934,554,924,564,926,562,1916,564,1918,562,924,564,1916,564,926,562,1916,566,928,560,1916,564,8918,
    4010,3902,564,1926,554,1926,556,1916,564,1918,564,924,564,926,562,1916,564,934,554,1920,560,934,554,1926,556,924,564,924,564,934,554,924,564,926,562,1918,564,1914,566,924,564,1914,566,934,554,1916,566,934,554,1916,566,8916
  };

  uint16_t rawData[] = {//thomson led TV menu (counting with an oscilloscope)
    4020,3900,540,1920,560,1960,550,1940,570,1940,550,950,550,940,560,950,580,920,570,1930,560,970,520,930,560,980,530,920,560,940,560,930,570,940,550,1930,560,1940,550,1950,550,1930,570,960,570,1910,540,1930,580,1900,580,8960,
    4020,3900,540,1920,560,1960,550,1940,570,1940,550,950,550,940,560,950,580,920,570,1930,560,970,520,930,560,980,530,920,560,940,560,930,570,940,550,1930,560,1940,550,1950,550,1930,570,960,570,1910,540,1930,580,1900,580,8960
  };

  uint16_t size = sizeof(rawData) / sizeof(rawData[0]);
  irsend.sendRaw(rawData, size, 38);
  */
  }
  return true;
}

bool remoteNode::parseStringAndSendRaw(const String str, uint16_t freq) {
  uint16_t count;
  uint16_t *raw_array;
  count = countValuesInStr(str, ',');
  raw_array = newCodeArray(count);
  int16_t index = -1;
  uint16_t start_from = 0;
  count = 0;
  do {
    index = str.indexOf(',', start_from);
    raw_array[count] = str.substring(start_from, index).toInt();
    start_from = index + 1;
    count++;
  } while (index != -1);

  irsend.sendRaw(raw_array, count, freq);
  #ifdef DEBUG
      for (int i = 1; i <= count; i++){
        Serial.print(raw_array[i - 1], DEC);
        if(i < count){
          Serial.print(",");
        }else{Serial.println();}
      }
  #endif
  free(raw_array);
  return count > 0 ?  true : false;
}

uint16_t remoteNode::countValuesInStr(const String str, char sep){
  int16_t index = -1;
  uint16_t count = 0;
  do {
    index = str.indexOf(sep, index + 1);
    count++;
  } while (index != -1);
    return count;
}

uint16_t *remoteNode::newCodeArray(const uint16_t size) {
  uint16_t *result;
  result = reinterpret_cast<uint16_t*>(malloc(size * sizeof(uint16_t)));
  if (result == NULL){
    Serial.println(F("FATAL: Can't allocate memory for an array."));
    delay(10000);
    ESP.restart();
  }
    return result;
}


void remoteNode::remoteSetup(){
    modulationFrequency->setDefaultValue(38).setValidator([](float candidateFrequency) {
    return (candidateFrequency == 17) || (candidateFrequency == 22) || (candidateFrequency == 24) || (candidateFrequency == 36) ||
           (candidateFrequency == 38) ||(candidateFrequency == 44) || (candidateFrequency == 48);
  });
}

void remoteNode::setup() {
    irrecv.enableIRIn();
    irsend.begin();
    advertise("oscillator").settable();
    advertise("frequency").setRetained(true).setFormat("17, 22, 24, 36, 38, 44, 48").settable();
}

void remoteNode::loop() {
  if (irrecv.decode(&results)) {
    decode_type_t protocol = results.decode_type;
    if (protocol != decode_type_t::UNKNOWN) {
      String lastIrReceived = "";
      for (uint16_t i = 1; i < results.rawlen; i++) {
        uint32_t usecs;
        for (usecs = results.rawbuf[i] * kRawTick; usecs > UINT16_MAX; usecs -= UINT16_MAX) {
          lastIrReceived += uint64ToString(UINT16_MAX);
          lastIrReceived += ",0,";
        }
        lastIrReceived += uint64ToString(usecs, 10);
        if (i < results.rawlen - 1)
          lastIrReceived += ",";
      }
        //Serial.println(lastIrReceived); Serial.println();
        //Serial.println(resultToSourceCode(&results));
        //setProperty("source").send(resultToSourceCode(&results).c_str());
        setProperty("source").send(lastIrReceived);
        setProperty("lenght").send((String)(results.rawlen - 1));
        setProperty("hex").send(resultToHexidecimal(&results).c_str());
        setProperty("protocol").send(typeToString(protocol).c_str());
    }
  irrecv.resume();
  }
}
