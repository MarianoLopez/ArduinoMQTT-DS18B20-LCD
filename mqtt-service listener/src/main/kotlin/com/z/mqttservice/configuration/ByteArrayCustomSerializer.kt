package com.z.mqttservice.configuration

import com.fasterxml.jackson.core.JsonGenerator
import com.fasterxml.jackson.databind.JsonSerializer
import com.fasterxml.jackson.databind.SerializerProvider
import java.nio.charset.StandardCharsets

class ByteArrayCustomSerializer: JsonSerializer<ByteArray>() {
    override fun serialize(p0: ByteArray, p1: JsonGenerator?, p2: SerializerProvider?) {
        p1?.writeObject(String(p0, StandardCharsets.UTF_8))
    }

}