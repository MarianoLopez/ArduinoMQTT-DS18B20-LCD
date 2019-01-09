package com.z.mqttservice.service

import org.eclipse.paho.client.mqttv3.MqttMessage
import org.springframework.http.codec.ServerSentEvent
import org.springframework.stereotype.Service
import reactor.core.publisher.EmitterProcessor
import java.time.LocalDateTime

@Service
class StreamService {
    private val emitter =  EmitterProcessor.create<ServerSentEvent<MqttMessage>>()

    fun subscribe() = emitter.log()

    fun publish(message:MqttMessage){
        emitter.onNext(ServerSentEvent.builder<MqttMessage>().apply {
            data(message)
            id(LocalDateTime.now().toString())
            event("new message")
        }.build())
    }
}