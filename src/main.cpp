#include <Arduino.h>
#include <heltec.h>

constexpr long const BAND = 868E6; // you can set band here directly,e.g. 868E6,915E6

static size_t counterRecv;
static size_t counterSend;

void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/,
               true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);

  Heltec.onReceive([](String const& packet, int rssi) {
    counterRecv++;
    Heltec.drawRecv(packet.length(), packet, rssi);
  });
  Heltec.onButton([](ButtonState state) {
    counterSend++;
    Heltec.send(counterSend);
  });
  Heltec.onDraw([]() {
    if (counterRecv == 0)
    {
      Heltec.drawRecv();
    }
    Heltec.drawSend(counterSend);
  });
}

void loop()
{
  Heltec.loop();
}
