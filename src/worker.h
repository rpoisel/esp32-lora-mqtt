#pragma once

#include <heltec.h>

#include <FreeRTOS.h>

void TaskLoRaMsgProcessor(void* param);

class LoRaMsgProcessor
{
  public:
  static void taskWrapper(void* param);
  LoRaMsgProcessor() : loRaMessages(xQueueCreate(LORA_QUEUE_LENGTH, sizeof(LoRaMessage)))
  {
  }

  void begin();
  void enqueue(LoRaMessage const& msg);
  void processMsgs();

  private:
  static constexpr size_t const LORA_QUEUE_LENGTH = 4;

  QueueHandle_t loRaMessages;
};
