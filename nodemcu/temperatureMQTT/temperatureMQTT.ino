#define SERIAL_LOGGER //comment to disable logger
//#define MONITOR_PLOTTER

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Secrets.h"
#include "Utils.h"
#include "DS18B20.h"
#include "MqttClient.h"

const int sleep = 1000;

/*************************************MQTT*********************************************/
WiFiClient espClient;
IPAddress mqtt_server(192,168,0,250);//mqtt broker
const int mqtt_port = 1883;//server port
const char* mqtt_topic = "TEMPERATURE";//topic
const char* mqtt_client = "NODEMCU";//client name
const char* mqtt_user = MQTT_USER;//defined in arduino_secrets.h
const char* mqtt_password = MQTT_PASSWORD;//defined in arduino_secrets.h
MqttClient mqttClient(espClient, mqtt_server,mqtt_port,mqtt_client,mqtt_user,mqtt_password);
/*************************************************************************************/

/*************************************DS18B20*********************************************/
#define ONE_WIRE_BUS 12 //sensor pin https://alselectro.files.wordpress.com/2017/10/nodemcu_pins1_thumb.jpg?w=517&h=441
DS18B20 ds18b20(ONE_WIRE_BUS);
float temp = 0;//to store temp readed from sensor
/*************************************************************************************/
void setup() {
  Serial.begin(115200);
  toLogln("Connecting to WiFi: "+String(WIFI_SSID));
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);//defined in arduino_secrets.h
  delay(2000);
  toLogln("Local IP address: "+IPAddress2String(WiFi.localIP()));
}

void loop() {
  temp = ds18b20.getTemperature();
  toLogln(temp);
  toPlotter(temp);
  if(WiFi.status() == WL_CONNECTED){
    char buffer[10];
    dtostrf(temp,2, 2, buffer);// converts the double value passed in val into an ASCII
    mqttClient.send(buffer,mqtt_topic);
  }else{
    toLogln("No connected to WiFi: "+String(WIFI_SSID));
  }
  delay(sleep);
}
