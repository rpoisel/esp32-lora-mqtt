#include <Arduino.h>
#include <heltec.h>

constexpr long const BAND = 868E6; // you can set band here directly,e.g. 868E6,915E6

static size_t counterRecv;
static size_t counterSend;
static String lastPacket;
static int lastRssi;

void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/,
               true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);

  Heltec.onReceive([](String const& packet, int rssi) {
    counterRecv++;
    lastPacket = packet;
    lastRssi = rssi;
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
      auto rssiStr = "RSSI: " + String(lastRssi, DEC);

      display->drawString(0, 0, "Recv size " + String(lastPacket.length(), DEC) + " packages:");
      display->drawString(0, 10, lastPacket);
      display->drawString(0, 20, "With " + rssiStr);
    }
    if (counterSend == 0)
    {
      display->drawString(0, 50, "No packet sent yet.");
      return;
    }
    display->drawString(0, 50, "Packet " + String(counterSend, DEC) + " sent done");
  });
}

void loop()
{
  Heltec.loop();
  delay(20);
}
