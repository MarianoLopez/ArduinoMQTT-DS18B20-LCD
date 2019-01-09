/*Author: Mariano Lopez
  Description: MQTT (Message Queuing Telemetry Transport "is a publish-subscribe based messaging protocol") sample project.
               The Arduino will measure temperature at fixed rate (SLEEP_MS) using the DS18B20 sensor.
               Then will try to connect and authenticate (credentials are store in arduino_secrets.h) with the MQTT broker/server to send the temperature to a MQTT topic.
               Every message log will be print in the LCD & Serial (IF SERIAL_LOGGER is defined).
  Items:
    Arduino Leonardo
    Ethernet Shield W5100
    DS18B20 sensor connected to (ONE_WIRE_BUS) pin w/4k7 Ohm resistor
    LCD 16x2 w/PCF8574 I2C adapter connected to SDA & SCL pins
  Context: I run the MQTT broker "eclipse-mosquitto" with docker at (mqtt_server:mqtt_port)
  Dependencies:
    SPI: Serial Peripheal Interface: synchonous serial data protocol used by communicating the arduino boad with ethernet shield
    Ethernet: Library designed to work with the arduino ethernet shield
    PubSubClient: Library for the arduino ethernet shield that provides support for MQTT (github.com/knolleary/pubsubclient)
    OneWire: Lets you access 1-wire protocol devices made by Maxim/Dallas, such as temperature sensors
    DallasTemperature: Library for Dallas Temperature integrated circuit. Supports (DS18B20,DS18S20,DS1822 & DS1820)
    Wire: This library allows you to communicate with I2C devices (is a serial protocol for two-wire interface to connect low-speed devices)
    LCD: Library designed to work with Liquid Crystal Displays
    LiquidCrystal_I2C: Allows you to control LCD displays that are compatible with the HD44780 driver (usually 16-pin interface). With I2C only uses 2-pin (SDA "Serial Data" & SCL "Serial clock")
*/
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "arduino_secrets.h"

/****************************************LOGGER******************************************/
#define SERIAL_LOGGER //comment to disable logger
/***************************************************************************************/

/**************************************DS18B20 sensor**********************************/
#define SLEEP_MS 5000 //measure time in milliseconds
#define ONE_WIRE_BUS 7 //sensor pin
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
float temp = 0;//to store temp readed from sensor
String sTemp = "";//string temp parse
/***************************************************************************************/

/********************************Arduino Ethernet interface*****************************/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Arduino MAC
IPAddress ip(192, 168, 1, 177);//Arduino IP
EthernetClient ethClient;
/*************************************************************************************/

/*************************************MQTT*********************************************/
const char* mqtt_server = "192.168.1.30";//server IP
const int mqtt_port = 1883;//server port
const char* mqtt_topic = "test";//topic
const char* mqtt_client = "Arduino Leonardo";//client name
const char* mqtt_user = MQTT_USER;//defined in arduino_secrets.h
const char* mqtt_password = MQTT_PASSWORD;//defined in arduino_secrets.h
PubSubClient client(ethClient);
/*************************************************************************************/

/*************************************LCD*********************************************/
//                     Addr, En,Rw,Rs,d4,d5,d6,d7
LiquidCrystal_I2C lcd( 0x27, 2, 1, 0, 4, 5, 6, 7);
/************************************************************************************/
void setup() {
  Serial.begin(9600);//init serial
  while(!Serial){delay(1);}//wait init
  toLog("Ethernet init... ");
  Ethernet.begin(mac,ip);//init ethernet
  if (Ethernet.hardwareStatus() == EthernetNoHardware){// Check for Ethernet hardware present
    toLogln("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {delay(1);} // do nothing, no point running without Ethernet hardware
  }
  if (Ethernet.linkStatus() == LinkOFF) Serial.println("Ethernet cable is not connected.");
  toLogln("host ip: "+IPAddress2String(Ethernet.localIP()));

  sensors.begin();//init ds18b20 sensor & anothers on the bus
  lcd.begin (16,2);// init display 16(chars)x2(lines)
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  delay(10);

  client.setServer(mqtt_server, mqtt_port);//set mqtt server 
}

void loop() {
  toLog("Requesting temperature... "); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  temp = sensors.getTempCByIndex(0); // Why "byIndex"?. You can have more than one DS18B20 on the same bus.  
  sTemp = "Temp: "+String(temp,2)+"C  ";
  toLogln(sTemp);
  writeInLCD(0,sTemp);
  toMqttTopic(temp,mqtt_topic);
  delay(SLEEP_MS);
}

/*if mqtt client is not connected will try to connect with @mqtt_client, @mqtt_user & @mqtt_password*/
void connectToMqtt(){
  if (!client.connected() || client.state()!=MQTT_CONNECTED) {
    toLog("Connecting mqtt... ");
    client.connect(mqtt_client,mqtt_user,mqtt_password);
    String mqttStatus = getMqttStatus();
    toLogln(" - "+String(mqtt_server)+":"+mqtt_port+"@"+String(mqtt_client)+"@"+String(mqtt_user)+"@"+String(mqtt_password)+" -->: "+mqttStatus);
    writeInLCD(1,mqttStatus);
  }
}

/*send @temperature to @topic if mqtt client is connected else will try to connect once*/
void toMqttTopic(const float temperature, const char* topic){
  connectToMqtt();
  if(client.connected() && client.state()==MQTT_CONNECTED){
    char buffer[10];
    dtostrf(temperature,2, 2, buffer);// converts the double value passed in val into an ASCII
    toLogln("sending: ("+String(buffer)+") to: '"+topic+"' topic");
    client.publish(topic, buffer); 
  }
}

/*print @message on @line*/
void writeInLCD(int line,String message){
   if(line==0) lcd.home(); else lcd.setCursor(0,line);
   if(message.length()>16){ //if message > 16 will show char by char at 250ms delay
    for(int i=0;i<=message.length()-1;i++){
      lcd.setCursor(0,line);
      String newString = message.substring(i);
      while(newString.length()<16){ newString+=" ";}
      lcd.print(newString);
      delay(250);
    }
    lcd.setCursor(0,line);
   }
  while(message.length()<16){message+=" ";} 
  lcd.print(message);
}

/*@return String mqtt connection status enum*/
String getMqttStatus(){
  switch(client.state()){
      case MQTT_CONNECTION_TIMEOUT:
        toLog("the server didn't respond within the keepalive time");
        return "MQTT_CONNECTION_TIMEOUT";
      case MQTT_CONNECTION_LOST:
        toLog("the network connection was broken");
        return "MQTT_CONNECTION_LOST";
      case MQTT_CONNECT_FAILED:
        toLog("the network connection failed");
        return "MQTT_CONNECT_FAILED";
      case MQTT_DISCONNECTED:
        toLog("the client is disconnected cleanly");
        return "MQTT_DISCONNECTED";
      case MQTT_CONNECTED:
        toLog("the client is connected");
        return "MQTT_CONNECTED";
      case MQTT_CONNECT_BAD_PROTOCOL:
        toLog("the server doesn't support the requested version of MQTT");
        return "MQTT_CONNECT_BAD_PROTOCOL";
      case MQTT_CONNECT_BAD_CLIENT_ID:
        toLog("the server rejected the client identifier");
        return "MQTT_CONNECT_BAD_CLIENT_ID";
      case MQTT_CONNECT_UNAVAILABLE:
        toLog("the server was unable to accept the connection");
        return "MQTT_CONNECT_UNAVAILABLE";
      case MQTT_CONNECT_BAD_CREDENTIALS:
        toLog("the username/password were rejected");
        return "MQTT_CONNECT_BAD_CREDENTIALS";
      case MQTT_CONNECT_UNAUTHORIZED:
        toLog("the client was not authorized to connect");
        return "MQTT_CONNECT_UNAUTHORIZED";//"the client was not authorized to connect";
      default:
        toLog("unknow error");
        return "unknow error";
    }
}

/*print @message on Serial if @SERIAL_LOGGER if defined*/
void toLog(String message){
  #if defined SERIAL_LOGGER
    Serial.print(message);
  #endif
}

/*println @message on Serial if @SERIAL_LOGGER if defined*/
void toLogln(String message){
  #if defined SERIAL_LOGGER
    Serial.println(message);
  #endif
}

/*parse @ipAddress to String*/
String IPAddress2String(const IPAddress ipAddress){
  return String(ipAddress[0])+String(".")+String(ipAddress[1])+String(".")+String(ipAddress[2])+String(".")+String(ipAddress[3]);
}
