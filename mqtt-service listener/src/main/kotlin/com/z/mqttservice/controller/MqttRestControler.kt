package com.z.mqttservice.controller

import com.z.mqttservice.domain.Response
import com.z.mqttservice.service.MqttService
import com.z.mqttservice.service.StreamService
import org.springframework.http.MediaType
import org.springframework.http.ResponseEntity
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RestController
import reactor.core.publisher.Mono
import reactor.core.publisher.toMono

@RestController
class MqttRestControler(private val mqttService: MqttService, private val streamService: StreamService) {
    @GetMapping("/publish/{data}")
    fun sendMessage(@PathVariable data:String): Mono<ResponseEntity<Response>> = mqttService.publishToMqttTopic(data).let { ResponseEntity.status(it.status).body(it).toMono() }


    @GetMapping("/sse",produces = [MediaType.TEXT_EVENT_STREAM_VALUE])
    fun subscribe() = streamService.subscribe()

    @GetMapping("/test") fun test() = "Hi there"

}