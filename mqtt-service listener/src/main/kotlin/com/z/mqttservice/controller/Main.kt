package com.z.mqttservice.controller

import com.z.mqttservice.domain.Response
import com.z.mqttservice.service.MqttService
import com.z.mqttservice.service.StreamService
import org.springframework.http.MediaType
import org.springframework.http.ResponseEntity
import org.springframework.stereotype.Controller
import org.springframework.web.bind.annotation.GetMapping
import org.springframework.web.bind.annotation.PathVariable
import org.springframework.web.bind.annotation.RequestMapping
import org.springframework.web.bind.annotation.RestController
import reactor.core.publisher.Mono
import reactor.core.publisher.toMono

@RestController
@RequestMapping("/api")
class MqttRestController(private val mqttService: MqttService, private val streamService: StreamService) {
    @GetMapping("/publish/{topic}/{data}")
    fun sendMessage(@PathVariable topic:String, @PathVariable data:String): Mono<ResponseEntity<Response>> = mqttService.publish(data,topic).let { ResponseEntity.status(it.status).body(it).toMono() }

    @GetMapping("/sse",produces = [MediaType.TEXT_EVENT_STREAM_VALUE])
    fun subscribe() = streamService.subscribe()

}

@Controller
@RequestMapping("/")
class HttpController{
    @GetMapping fun home() = "index"
}