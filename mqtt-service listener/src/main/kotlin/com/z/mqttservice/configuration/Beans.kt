package com.z.mqttservice.configuration

import com.fasterxml.jackson.core.Version
import com.fasterxml.jackson.databind.Module
import com.fasterxml.jackson.databind.module.SimpleModule
import org.springframework.context.annotation.Bean
import org.springframework.context.annotation.Configuration
import org.springframework.retry.backoff.FixedBackOffPolicy
import org.springframework.retry.policy.SimpleRetryPolicy
import org.springframework.retry.support.RetryTemplate


@Configuration
class Beans{
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