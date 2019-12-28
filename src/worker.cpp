#include <heltec.h>

#include <FreeRTOS.h>

void TaskLoRaMsgProcessor(void* param)
{
  LoRaMessage msg;
  QueueHandle_t* queue = static_cast<QueueHandle_t*>(param);
  for (;;)
  {
    xQueueReceive(*queue, &msg, portMAX_DELAY);
    Serial.print("Message: ");
    for (size_t i = 0; i < msg.len; i++)
    {
      Serial.print(static_cast<char>(msg.buf[i]));
    }
    Serial.println();
  }
}
