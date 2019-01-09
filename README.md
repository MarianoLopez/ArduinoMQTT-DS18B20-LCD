# Arduino

  - The Arduino will measure temperature at fixed rate (SLEEP_MS) using the DS18B20 sensor.
  - Then will try to connect and authenticate (credentials are store in arduino_secrets.h) with the MQTT broker/server to send the temperature to a MQTT topic.
- Every message log will be print in the LCD & Serial (IF SERIAL_LOGGER is defined).
    # Items:
  - Arduino Leonardo
  - Ethernet Shield W5100
  - DS18B20 sensor connected to (ONE_WIRE_BUS) pin w/4k7 Ohm resistor
  - LCD 16x2 w/PCF8574 I2C adapter connected to SDA & SCL pins
# Docker:
I run the MQTT broker "eclipse-mosquitto" with docker-compose with ports 1883 & 9001
# MQTT - Service listener
A spring boot project w/kotlin-maven that run on port 8080, listens to the MQTT topic & publish the messages to a SSE (ServerSentEvent) stream (:8080/sse).
