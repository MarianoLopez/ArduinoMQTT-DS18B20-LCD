/*IPAddress to String*/
String IPAddress2String(const IPAddress ipAddress){
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
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

void toLogln(float value){
  #if defined SERIAL_LOGGER
    Serial.println(value);
  #endif
}

void toPlotter(float value){
  #if defined MONITOR_PLOTTER
    Serial.println(value);
  #endif
}
