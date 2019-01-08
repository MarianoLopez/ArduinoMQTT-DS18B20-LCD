#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "arduino_secrets.h"

/**************************************DS18B20 sensor**********************************/
#define ONE_WIRE_BUS 7 //PIN
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
float temp = 0;
/***************************************************************************************/

/*************************************Ethernet interface********************************/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//host MAC
IPAddress ip(192, 168, 0, 251);//IP host
EthernetClient ethClient;
/*************************************************************************************/

/*************************************MQTT*********************************************/
IPAddress mqtt_server(192, 168, 0, 250);// IP del servidor
const int mqtt_port = 1883;
const char* mqtt_topic = "test";// topic
const char* mqtt_user = MQTT_USER;//defined in arduino_secrets.h
const char* mqtt_password = MQTT_PASSWORD;//defined in arduino_secrets.h
PubSubClient client(ethClient);
/*************************************************************************************/

/*************************************LCD*********************************************/
//                     Addr, En, Rw, Rs, d4, d5, d6, d7
LiquidCrystal_I2C lcd( 0x27, 2, 1, 0, 4, 5, 6, 7);
/************************************************************************************/
void setup() {
  Serial.begin(9600);//init serial
  Serial.println("Ethernet init");
  Ethernet.begin(mac,ip);//init ethernet
  Serial.print("host ip: ");
  Serial.println(Ethernet.localIP());
  client.setServer(mqtt_server, mqtt_port);//set mqtt server 
  sensors.begin();//init ds18b20 sensor & anothers on the wire
  lcd.begin (16,2);// init display 16(chars)x2(lines)
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  delay(10);
  writeInLCD(0,"Hello World");
  writeInLCD(1,"Hola mundo");
}

void loop() {
  Serial.print("Requesting temperatures..."); 
     sensors.requestTemperatures(); // Send the command to get temperature readings 
     Serial.println("DONE");
     Serial.print("Temperature is: "); 
     temp = sensors.getTempCByIndex(0); // Why "byIndex"?  // You can have more than one DS18B20 on the same bus.  
     Serial.println(temp);
     writeInLCD(0,"Temp: "+String(temp,2)+"C");
  if (!client.connected()) {
    Serial.print("Connecting mqtt ...");
    client.connect("Arduino Client",mqtt_user,mqtt_password);
    String mqttStatus = getMqttStatus();
    Serial.println(mqttStatus);
    writeInLCD(1,mqttStatus);
  }else {
    char buffer[10];
    dtostrf(temp,0, 0, buffer);// converts the double value passed in val into an ASCII
    client.publish(mqtt_topic, buffer);
  }
  // Tiempo entre envios (en ms)
  delay(5000);
}
void writeInLCD(int line,String message){
   if(line==0) lcd.home(); else lcd.setCursor(0,line);
   if(message.length()>16){
    for(int i=0;i<=message.length()-1;i++){
      lcd.setCursor(0,line);
      String newString = message.substring(i);
      while(newString.length()<16){ newString+=" ";}
      lcd.print(newString);
      delay(250);
    }
    lcd.setCursor(0,line);
   }
  lcd.print(message);
}

String getMqttStatus(){
  switch(client.state()){
      case MQTT_CONNECTION_TIMEOUT:
        return "MQTT_CONNECTION_TIMEOUT";//"the server didn't respond within the keepalive time";
      case MQTT_CONNECTION_LOST:
        return "MQTT_CONNECTION_LOST";//"the network connection was broken";
      case MQTT_CONNECT_FAILED:
        return "MQTT_CONNECT_FAILED";//"the network connection failed";
      case MQTT_DISCONNECTED:
        return "MQTT_DISCONNECTED";//"the client is disconnected cleanly";
      case MQTT_CONNECTED:
        return "MQTT_CONNECTED";//"the client is connected";
      case MQTT_CONNECT_BAD_PROTOCOL:
      return "MQTT_CONNECT_BAD_PROTOCOL";//"the server doesn't support the requested version of MQTT";
      case MQTT_CONNECT_BAD_CLIENT_ID:
        return "MQTT_CONNECT_BAD_CLIENT_ID";//"the server rejected the client identifier";
      case MQTT_CONNECT_UNAVAILABLE:
        return "MQTT_CONNECT_UNAVAILABLE";//"the server was unable to accept the connection";
      case MQTT_CONNECT_BAD_CREDENTIALS:
        return "MQTT_CONNECT_BAD_CREDENTIALS";//"the username/password were rejected";
      case MQTT_CONNECT_UNAUTHORIZED:
        return "MQTT_CONNECT_UNAUTHORIZED";//"the client was not authorized to connect";
      default:
        return "unknow error";
    }
}

