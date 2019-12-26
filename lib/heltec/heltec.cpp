// Copyright (c) Heltec Automation. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "heltec.h"
#include <LoRa.h>
#include <WiFi.h>

Heltec_ESP32::Heltec_ESP32()
    : display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64),
      onReceiveCb([](String const& packet, int rssi) {}), onButtonCb([](ButtonState state) {}),
      onDrawCb([]() {}), flagButton(false)
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
  LoRa.onReceive(globalOnReceive);
  attachInterrupt(BUTTON, globalOnButton, FALLING);
  Heltec.display.clear();

  // WiFi
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin("SSID",
             "Password"); // fill in "Your WiFi SSID","Your Password"
  delay(100);

  byte count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 10)
  {
    count++;
    display.drawString(0, 0, "Connecting...");
    display.display();
  }

  display.clear();
  if (WiFi.status() == WL_CONNECTED)
  {
    display.drawString(0, 0, "Connecting...OK.");
    display.display();
  }
  else
  {
    display.clear();
    display.drawString(0, 0, "Connecting...Failed");
    display.display();
  }
  display.drawString(0, 10, "WIFI Setup done");
  display.display();

  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
}

void Heltec_ESP32::loop()
{
  Heltec.display.clear();
  LoRa.receive();
  if (flagButton)
  {
    onButtonCb(LOW);
    flagButton = false;
  }
  onDrawCb();
  Heltec.display.display();
}

void Heltec_ESP32::onReceive(std::function<void(String const& packet, int rssi)> cb)
{
  onReceiveCb = cb;
}

void Heltec_ESP32::onButton(std::function<void(ButtonState state)> cb)
{
  onButtonCb = cb;
}

void Heltec_ESP32::onDraw(std::function<void()> cb)
{
  onDrawCb = cb;
}

void Heltec_ESP32::drawSend(size_t cnt)
{
  if (cnt == 0)
  {
    Heltec.display.drawString(0, 50, "No packet sent yet.");
    return;
  }
  Heltec.display.drawString(0, 50, "Packet " + String(cnt, DEC) + " sent done");
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

void globalOnReceive(int pSize)
{
  String packet;
  while (LoRa.available())
  {
    packet += (char)LoRa.read();
  }
  Heltec.onReceiveCb(packet, LoRa.packetRssi());
}

void globalOnButton()
{
  if (digitalRead(Heltec_ESP32::BUTTON) == LOW)
  {
    Heltec.flagButton = true;
  }
}
