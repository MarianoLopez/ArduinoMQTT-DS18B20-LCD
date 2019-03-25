package com.z.mqttservice.configuration

import com.fasterxml.jackson.core.Version
import com.fasterxml.jackson.databind.Module
import com.fasterxml.jackson.databind.module.SimpleModule
import org.eclipse.paho.client.mqttv3.IMqttClient
import org.eclipse.paho.client.mqttv3.MqttClient
import org.eclipse.paho.client.mqttv3.MqttConnectOptions
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Value
import org.springframework.context.annotation.Bean
import org.springframework.context.annotation.Configuration
import org.springframework.retry.backoff.FixedBackOffPolicy
import org.springframework.retry.policy.SimpleRetryPolicy
import org.springframework.retry.support.RetryTemplate


@Configuration
class Beans{
    var logger: Logger = LoggerFactory.getLogger(Beans::class.java)
    @Value("\${mqtt.url}") private lateinit var mqttUrl:String
    @Value("\${mqtt.username}") private lateinit var username:String
    @Value("\${mqtt.password}") private lateinit var password:String
    @Value("\${mqtt.clientId}") private lateinit var clientId:String

    @Bean
    fun mqttConnectionOptions() = MqttConnectOptions().apply {
        isAutomaticReconnect = true
        keepAliveInterval = 120
        isCleanSession = true
        connectionTimeout = 60
        userName = username
        password = this@Beans.password.toCharArray()
    }


    @Bean
    fun mqtt(): IMqttClient = MqttClient(mqttUrl,clientId).apply {
        setManualAcks(false)
        retryTemplate().execute<Unit,RuntimeException> {
            val info = "try to connect: ${this.serverURI} - user: $username"
            if(it.retryCount>0) logger.warn("$info - count: ${it.retryCount} - ${it.lastThrowable}") else logger.info(info)
            this.connect(mqttConnectionOptions())
            logger.info("connected to: ${this.currentServerURI}")
        }
    }

    @Bean
    fun retryTemplate(): RetryTemplate = RetryTemplate().apply {
        setBackOffPolicy(FixedBackOffPolicy().apply { backOffPeriod=5000L })
        setRetryPolicy(SimpleRetryPolicy().apply { maxAttempts=10 })
    }

    @Bean
    fun byteArrayCustomSerializer(): Module {
        val module = SimpleModule("ByteArray-Module", Version(1, 0, 0, null, null, null))
        module.addSerializer(ByteArray::class.java, ByteArrayCustomSerializer())
        return module
    }
}