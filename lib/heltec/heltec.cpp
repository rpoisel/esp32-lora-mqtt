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

#if defined(WIFI_LoRa_32_V2) || defined(Wireless_Stick) || defined(Wireless_Stick_Lite)

  VextON();
#endif

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
#if defined(Wireless_Stick_Lite)
    if (SerialEnable && Wireless_Stick_Lite)
    {
      Serial.print("The Wireless Stick Lite not have an on board display, Display option must be "
                   "FALSE!!!\r\n");
    }
#endif

#if defined(WIFI_Kit_32) || defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) ||                   \
    defined(Wireless_Stick)
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "OLED initial done!");
    display.display();

    if (SerialEnable)
    {
      Serial.print("you can see OLED printed OLED initial done!\r\n");
    }
#endif
  }

  // LoRa INIT
  if (LoRaEnable)
  {
#if defined(WIFI_Kit_32)
    if (SerialEnable && WIFI_Kit_32)
    {
      Serial.print("The WiFi Kit 32 not have LoRa function, LoRa option must be FALSE!!!\r\n");
    }
#endif

#if defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) || defined(Wireless_Stick) ||                \
    defined(Wireless_Stick_Lite)

    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST_LoRa, DIO0);
    if (!LoRa.begin(BAND))
    {
      if (SerialEnable)
      {
        Serial.print("Starting LoRa failed!\r\n");
      }
#if defined(WIFI_Kit_32) || defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) ||                   \
    defined(Wireless_Stick)
      if (DisplayEnable)
      {
        display.clear();
        display.drawString(0, 0, "Starting LoRa failed!");
        display.display();
        delay(300);
      }
#endif
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
#if defined(WIFI_Kit_32) || defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) ||                   \
    defined(Wireless_Stick)
    if (DisplayEnable)
    {
      display.clear();
      display.drawString(0, 0, "LoRa Initial success!");
      display.display();
      delay(300);
    }
#endif

#endif
  }
  pinMode(LED, OUTPUT);
}

void Heltec_ESP32::displaySendReceive(size_t cnt, int packSize, String const& packet, int rssi)
{
  displaySendReceive((String)(cnt - 1), String(packSize, DEC), packet, String(LoRa.packetRssi(), DEC));
}

void Heltec_ESP32::displaySendReceive(String const& cnt, String const& packSize, String const& packet,
                        String const& rssi)
{
  auto rssiStr = "RSSI: " + rssi;

  Heltec.display.drawString(0, 50, "Packet " + cnt + " sent done");
  Heltec.display.drawString(0, 0, "Recv size " + packSize + " packages:");
  Heltec.display.drawString(0, 10, packet);
  Heltec.display.drawString(0, 20, "With " + rssiStr);
  Heltec.display.display();
  delay(100);
  Heltec.display.clear();
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
