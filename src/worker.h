#pragma once

#include <heltec.h>

#include <FreeRTOS.h>
#include <PubSubClient.h>

void TaskLoRaMsgProcessor(void* param);

class LoRaMsgProcessor
{
  public:
  LoRaMsgProcessor();

  void begin();
  void enqueue(LoRaMessage const& msg);

  private:
  static constexpr size_t const LORA_QUEUE_LENGTH = 4;

  void processMsgs();

  QueueHandle_t loRaMessages;
  WiFiClient wiFiClient;
  PubSubClient pubSubClient;
};
