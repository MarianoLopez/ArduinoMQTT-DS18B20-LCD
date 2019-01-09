package com.z.mqttservice.service

import com.z.mqttservice.domain.Response
import org.eclipse.paho.client.mqttv3.IMqttClient
import org.eclipse.paho.client.mqttv3.MqttClient
import org.eclipse.paho.client.mqttv3.MqttConnectOptions
import org.eclipse.paho.client.mqttv3.MqttException
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Value
import org.springframework.http.HttpStatus
import org.springframework.retry.support.RetryTemplate
import org.springframework.stereotype.Service
import java.util.*
import java.util.concurrent.Executors
import javax.annotation.PostConstruct

@Service
class MqttService(private val retryTemplate: RetryTemplate, private val streamService: StreamService){
    var logger: Logger = LoggerFactory.getLogger(MqttService::class.java)

    @Value("\${mqtt.url}") private lateinit var mqttUrl:String
    @Value("\${mqtt.username}") private lateinit var username:String
    @Value("\${mqtt.password}") private lateinit var password:String
    @Value("\${mqtt.topic}") private lateinit var topic: String
    private lateinit var mqttClient: IMqttClient

    @PostConstruct
    private fun start(){
        mqttClient = MqttClient(mqttUrl, UUID.randomUUID().toString())
        Executors.newSingleThreadExecutor().execute {
            retryTemplate.execute<Unit,RuntimeException> {
                val info = "try to connect: ${mqttClient.serverURI} - user: $username"
                if(it.retryCount>0) logger.warn("$info - count: ${it.retryCount} - ${it.lastThrowable.localizedMessage}") else logger.info(info)
                mqttClient.connect(MqttConnectOptions().apply {
                    isAutomaticReconnect = true
                    isCleanSession = true
                    connectionTimeout = 10
                    userName = username
                    password = this@MqttService.password.toCharArray()
                })
                subscribeToMqttTopic()
            }
        }
    }

    private fun subscribeToMqttTopic(){
        mqttClient.subscribe(topic) { topic, message ->
            logger.debug("New Message from topic: $topic - clientId: ${message.id} - payload: ${message.payload}")
            streamService.publish(message)
        }
    }

    fun publishToMqttTopic(payload:String): Response {
        return try{
            mqttClient.publish(topic,payload.toByteArray(),0,true)
            Response(message = "$payload sended to $topic", status = HttpStatus.OK)
        }catch (me: MqttException){
            Response(message = me.localizedMessage, status = HttpStatus.INTERNAL_SERVER_ERROR)
        }
    }
}