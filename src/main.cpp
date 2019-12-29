#include "worker.h"

#include <heltec.h>

#include <FreeRTOS.h>

static LoRaMsgProcessor processor;
static size_t counterRecv;
static size_t counterSend;
static LoRaMessage lastPacket;
static int lastRssi;

static void messageReceived(LoRaMessage const& msg, int rssi);
static void buttonPressed(ButtonState state);
static void displayInfo(SSD1306Wire* display);

void setup()
{
  processor.begin();
  Heltec.begin(true /* enable display */, true /* enable LoRa */, true /* enable serial */,
               true /* enable boost */, 868E6 /* band */, &messageReceived, &buttonPressed,
               &displayInfo);
}

void loop()
{
  Heltec.loop();
  delay(20);
}

static void messageReceived(LoRaMessage const& msg, int rssi)
{
  counterRecv++;
  lastPacket = msg;
  lastRssi = rssi;
  processor.enqueue(msg);
}

static void buttonPressed(ButtonState state)
{
  counterSend++;
  Heltec.send(counterSend);
}

static void displayInfo(SSD1306Wire* display)
{
  if (counterRecv == 0)
  {
    display->drawString(0, 0, "No packet received yet.");
  }
  else
  {
    display->drawString(
        0, 0, "Packet " + String(counterRecv, DEC) + ", size " + String(lastPacket.len, DEC) + ":");
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
}
