// Copyright (c) Heltec Automation. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "heltec.h"
#include <LoRa.h>

Heltec_ESP32::Heltec_ESP32()
    : display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64), onReceiveCb(nullptr),
      onButtonCb(nullptr), onDrawCb(nullptr), flagButton(false),
      loRaMessages(xQueueCreate(LORA_QUEUE_LEN, sizeof(LoRaMessage)))
{
}

Heltec_ESP32::~Heltec_ESP32()
{
}

void Heltec_ESP32::begin(bool DisplayEnable, bool LoRaEnable, bool SerialEnable, long BAND,
                         ReceiveCb receiveCb, ButtonCb buttonCb, DrawCb drawCb)
{
  onReceiveCb = receiveCb;
  onButtonCb = buttonCb;
  onDrawCb = drawCb;

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
  LoRa.onReceive(
      [](void* context, int pSize) {
        LoRaMessage msg{0, -1, {'\0'}};
        for (msg.len = 0; LoRa.available() && msg.len < LORA_BUF_LEN - 1 && msg.len < pSize;
             msg.len++)
        {
          msg.buf[msg.len] = static_cast<byte>(LoRa.read());
        }
        msg.rssi = LoRa.packetRssi();
        auto heltec = static_cast<Heltec_ESP32*>(context);
        if (xQueueSend(heltec->loRaMessages, &msg, 0) != pdTRUE)
        {
          Serial.print("Dropping LoRa message: ");
          Serial.println(reinterpret_cast<char const*>(&msg.buf[0]));
        }
      },
      this);

  attachInterrupt(BUTTON, globalOnButton, FALLING);
  display.clear();
}

void Heltec_ESP32::loop()
{
  LoRaMessage msg;

  display.clear();
  LoRa.receive();
  while (onReceiveCb && xQueueReceive(loRaMessages, &msg, 0) == pdTRUE)
  {
    onReceiveCb(msg);
  }
  if (flagButton)
  {
    if (onButtonCb)
    {
      onButtonCb(LOW);
    }
    flagButton = false;
  }
  if (onDrawCb)
  {
    onDrawCb(&display);
  }
  display.display();
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

void globalOnButton()
{
  if (digitalRead(Heltec_ESP32::BUTTON) == LOW)
  {
    Heltec.flagButton = true;
  }
}

Heltec_ESP32 Heltec;
