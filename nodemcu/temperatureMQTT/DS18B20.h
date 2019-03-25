#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

class DS18B20{
  private:
    OneWire oneWire; /*add default constructor OneWire(){} to OneWire.h*/
    DallasTemperature sensors;
  public:
    DS18B20(){}
    DS18B20(int pin){
      begin(pin);
    }

    void begin(int pin){
      oneWire = OneWire(pin);
      sensors = DallasTemperature(&oneWire);
    }

    float getTemperatureByIndex(int index){
      sensors.requestTemperatures(); // Send the command to get temperature readings 
      return sensors.getTempCByIndex(index); // Why "byIndex"?. You can have more than one DS18B20 on the same bus.  
    }

    float getTemperature(){
      return getTemperatureByIndex(0);
    }
};
