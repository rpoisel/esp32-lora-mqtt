// Copyright (c) Heltec Automation. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "heltec.h"

Heltec_ESP32::Heltec_ESP32() : display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64)
{
}

Heltec_ESP32::~Heltec_ESP32()
{
}

void Heltec_ESP32::begin(bool DisplayEnable, bool LoRaEnable, bool SerialEnable, bool PABOOST,
                         long BAND)
{
  // UART
  if (SerialEnable)
  {
    Serial.begin(115200);
    Serial.flush();
    delay(50);
    Serial.print("Serial initial done\r\n");
  }

  // OLED
  if (DisplayEnable)
  {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "OLED initial done!");
    display.display();

    if (SerialEnable)
    {
      Serial.print("you can see OLED printed OLED initial done!\r\n");
    }
  }

  // LoRa INIT
  if (LoRaEnable)
  {
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST_LoRa, DIO0);
    if (!LoRa.begin(BAND))
    {
      if (SerialEnable)
      {
        Serial.print("Starting LoRa failed!\r\n");
      }
      if (DisplayEnable)
      {
        display.clear();
        display.drawString(0, 0, "Starting LoRa failed!");
        display.display();
        delay(300);
      }
      while (1)
        ;
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setPreambleLength(8);
    LoRa.setSignalBandwidth(125000);
    LoRa.disableCrc();
    LoRa.setCodingRate4(5);
    if (SerialEnable)
    {
      Serial.print("LoRa Initial success!\r\n");
    }
    if (DisplayEnable)
    {
      display.clear();
      display.drawString(0, 0, "LoRa Initial success!");
      display.display();
      delay(300);
    }
  }
  pinMode(LED, OUTPUT);
}

void Heltec_ESP32::drawSend(size_t cnt)
{
  drawSend(String(cnt - 1, DEC));
}

void Heltec_ESP32::drawSend(String const& cnt)
{
  Heltec.display.drawString(0, 50, "Packet " + cnt + " sent done");
}

void Heltec_ESP32::drawRecv(int packSize, String const& packet, int rssi)
{
  drawRecv(String(packSize, DEC), packet, String(LoRa.packetRssi(), DEC));
}

void Heltec_ESP32::drawRecv(String const& packSize, String const& packet, String const& rssi)
{
  auto rssiStr = "RSSI: " + rssi;

  Heltec.display.drawString(0, 0, "Recv size " + packSize + " packages:");
  Heltec.display.drawString(0, 10, packet);
  Heltec.display.drawString(0, 20, "With " + rssiStr);
}

void Heltec_ESP32::send(size_t cnt)
{
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(cnt);
  LoRa.endPacket();
}

void Heltec_ESP32::VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void Heltec_ESP32::VextOFF() // Vext default OFF
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

Heltec_ESP32 Heltec;
