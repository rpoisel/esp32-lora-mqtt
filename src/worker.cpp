#include "config.h"
#include "worker.h"

using namespace LoRaGateway;

LoRaMsgProcessor::LoRaMsgProcessor()
    : loRaMessages(xQueueCreate(LORA_QUEUE_LENGTH, sizeof(LoRaMessage))), wiFiClient(),
      pubSubClient(wiFiClient)
{
}

void LoRaMsgProcessor::begin()
{
  pubSubClient.setServer(MQTT_BROKER, MQTT_PORT);
  xTaskCreate([](void* param) { static_cast<LoRaMsgProcessor*>(param)->processMsgs(); },
              WORKER_NAME, TASK_DEFAULT_STACK, this /* parameter */, 2 /* priority */,
              nullptr /* task handle */);
}

void LoRaMsgProcessor::enqueue(LoRaMessage const& msg)
{
  xQueueSend(loRaMessages, &msg, portMAX_DELAY);
}

static void traceLoRaMsg(LoRaMessage const& msg)
{
  Serial.print("Message: ");
  for (size_t i = 0; i < msg.len; i++)
  {
    Serial.print(static_cast<char>(msg.buf[i]));
  }
  Serial.println();
}

void LoRaMsgProcessor::processMsgs()
{
  LoRaMessage msg;
  for (;;)
  {
    if (!pubSubClient.connected())
    {
      pubSubClient.connect(MQTT_CLIENTID);
    }
    if (xQueueReceive(loRaMessages, &msg, 0) == pdTRUE)
    {
      traceLoRaMsg(msg);
      if (pubSubClient.connected())
      {
        pubSubClient.publish(MQTT_TOPIC, &msg.buf[0], msg.len);
      }
    }
    if (pubSubClient.connected())
    {
      pubSubClient.loop();
    }
    delay(20);
  }
}
