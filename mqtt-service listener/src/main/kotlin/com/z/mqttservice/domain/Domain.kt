package com.z.mqttservice.domain

import com.fasterxml.jackson.databind.node.ObjectNode
import org.springframework.http.HttpStatus

data class Response(val message:String,val status: HttpStatus = HttpStatus.OK, val payload:ObjectNode? = null)