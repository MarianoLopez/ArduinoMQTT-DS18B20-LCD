package com.z.mqttservice

import org.springframework.boot.autoconfigure.SpringBootApplication
import org.springframework.boot.runApplication


@SpringBootApplication
class MqttServiceApplication

fun main(args: Array<String>) {
	runApplication<MqttServiceApplication>(*args)
}

