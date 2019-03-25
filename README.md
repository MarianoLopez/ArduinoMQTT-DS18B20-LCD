# Arduino or NodeMCU

  - The board will measure temperature at fixed rate (SLEEP_MS) using the DS18B20 sensor.
  - Then will try to connect and authenticate (credentials are store in arduino_secrets.h) with the MQTT broker/server to send the temperature to a MQTT topic.
- Every message log will be print in the LCD & Serial (IF SERIAL_LOGGER is defined).
    # Items:
  - Arduino Leonardo or NodeMcu
  - Ethernet Shield W5100 (Only for Leonardo)
  - DS18B20 sensor connected to (ONE_WIRE_BUS) pin w/4k7 Ohm resistor
  - LCD 16x2 w/PCF8574 I2C adapter connected to SDA & SCL pins
# Docker:
- MQTT broker "eclipse-mosquitto". Exposed ports 1883 & 9001
- Portainer a docker management web gui. Exposed port 9000
# MQTT - Service listener
A spring boot w/kotlin-maven project that runs on port 8080
- Listens to the MQTT topic & publish the messages to a SSE (ServerSentEvent) stream.
- The root end-point "/": has a simple dashboard with Vue.js & MaterializeCss to display then temperature stream.
- Rest end-points: 
        - GET /api/sse = consume SSE stream
        - GET /api/publish/{topic}/{data} = publish {data} in {topic} through MQTT