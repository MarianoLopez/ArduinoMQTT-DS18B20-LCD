package com.z.mqttservice.service

import com.z.mqttservice.domain.Response
import org.eclipse.paho.client.mqttv3.IMqttClient
import org.eclipse.paho.client.mqttv3.MqttException
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.beans.factory.annotation.Value
import org.springframework.http.HttpStatus
import org.springframework.stereotype.Service
import javax.annotation.PostConstruct

@Service
class MqttService(private val streamService: StreamService, private val mqttClient: IMqttClient){
    var logger: Logger = LoggerFactory.getLogger(MqttService::class.java)
    @Value("\${mqtt.topic}") private lateinit var topic: String

    @PostConstruct
    private fun subscribe(){
        mqttClient.subscribe(topic) { topic, message ->
            logger.debug("New Message from topic: $topic - clientId: ${message.id} - payload: ${String(message.payload)}")
            streamService.publish(message)
        }
    }

    fun publish(payload:String, topic:String): Response {
        return try{
            mqttClient.publish(topic,payload.toByteArray(),0,true)
            Response(message = "$payload sended to $topic", status = HttpStatus.OK)
        }catch (me: MqttException){
            Response(message = me.localizedMessage, status = HttpStatus.INTERNAL_SERVER_ERROR)
        }
    }
}