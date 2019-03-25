package com.z.mqttservice.service

import org.eclipse.paho.client.mqttv3.MqttMessage
import org.slf4j.Logger
import org.slf4j.LoggerFactory
import org.springframework.stereotype.Service
import reactor.core.publisher.EmitterProcessor
import reactor.core.scheduler.Schedulers

@Service
class StreamService {
    var logger: Logger = LoggerFactory.getLogger(StreamService::class.java)
    private val emitter =  EmitterProcessor.create<MqttMessage>()

    fun subscribe() = emitter
            .subscribeOn(Schedulers.parallel())
            .doOnSubscribe { logger.info("New subscription $it") }
            .doOnError { logger.error("doOnError: ${it.localizedMessage}") }

    fun publish(message:MqttMessage){
        //logger.info("EmitterProcessor.onNext: $message")
        emitter.onNext(message)
    }
}