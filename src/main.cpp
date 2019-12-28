#include "worker.h"

#include <heltec.h>

#include <FreeRTOS.h>

constexpr long const BAND = 868E6; // you can set band here directly,e.g. 868E6,915E6
constexpr size_t const LORA_QUEUE_LENGTH = 4;

static size_t counterRecv;
static size_t counterSend;
static LoRaMessage lastPacket;
static int lastRssi;

static QueueHandle_t loRaMessages = xQueueCreate(LORA_QUEUE_LENGTH, sizeof(LoRaMessage));

void setup()
{
  Heltec.onReceive([](LoRaMessage const& msg, int rssi) {
    counterRecv++;
    lastPacket = msg;
    lastRssi = rssi;
    xQueueSend(loRaMessages, &msg, portMAX_DELAY);
  });
  Heltec.onButton([](ButtonState state) {
    counterSend++;
    Heltec.send(counterSend);
  });
  Heltec.onDraw([](SSD1306Wire* display) {
    if (counterRecv == 0)
    {
      display->drawString(0, 0, "No packet received yet.");
    }
    else
    {
      display->drawString(0, 0,
                          "Packet " + String(counterRecv, DEC) + ", size " +
                              String(lastPacket.len, DEC) + ":");
      display->drawString(0, 10, reinterpret_cast<char const*>(lastPacket.buf));
      display->drawString(0, 20, "With RSSI: " + String(lastRssi, DEC));
    }
    if (counterSend == 0)
    {
      display->drawString(0, 50, "No packet sent yet.");
    }
    else
    {
      display->drawString(0, 50, "Packet " + String(counterSend, DEC) + " sent done");
    }
  });
  xTaskCreate(TaskLoRaMsgProcessor, "LoRaMsgReader", 4096 /* stack size */,
              &loRaMessages /* parameter */, 2 /* priority */, nullptr /* task handle */);
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/,
               true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);
}

void loop()
{
  Heltec.loop();
  delay(20);
}
