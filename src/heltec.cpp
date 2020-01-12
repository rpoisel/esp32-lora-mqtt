// Copyright (c) Heltec Automation. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "heltec.h"

Heltec_ESP32::Heltec_ESP32()
    : display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64), onReceiveCb(nullptr),
      onButtonCb(nullptr), onDrawCb(nullptr),
      flagButton(false), cBuffer{}, rf95{PIN_RFM95_CS, PIN_RFM95_INT}
{
}

Heltec_ESP32::~Heltec_ESP32()
{
}

void Heltec_ESP32::begin(ReceiveCb receiveCb, ButtonCb buttonCb, DrawCb drawCb)
{
  onReceiveCb = receiveCb;
  onButtonCb = buttonCb;
  onDrawCb = drawCb;

  pinMode(LED, OUTPUT);

  // UART
  Serial.begin(115200);
  Serial.flush();
  delay(50);
  Serial.print("Serial initial done\r\n");

  // OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "OLED initial done!");
  display.display();

  Serial.print("you can see OLED printed OLED initial done!\r\n");

  // LoRa INIT
  pinMode(PIN_RFM95_RST, OUTPUT);
  digitalWrite(PIN_RFM95_RST, HIGH);

  delay(100);
  digitalWrite(PIN_RFM95_RST, LOW);
  delay(10);
  digitalWrite(PIN_RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init())
  {
    display.clear();
    display.drawString(0, 0, "LoRa Initial failure!");
    display.display();
    Serial.println("Starting LoRa failed!");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1)
      ;
  }
  display.clear();
  display.drawString(0, 0, "LoRa Initial success!");
  display.display();

  if (!rf95.setFrequency(868.0))
  {
    Serial.println("Could not set frequency");
    while (1)
      ;
  }
  rf95.setTxPower(23, false);
  rf95.setSpreadingFactor(7);
  rf95.setPreambleLength(8);
  rf95.setSignalBandwidth(125000);
  rf95.setPayloadCRC(false);
  rf95.setCodingRate4(5);

  attachInterrupt(BUTTON, globalOnButton, FALLING);
  display.clear();
}

void Heltec_ESP32::loop()
{
  display.clear();
  rf95.setModeRx();
  if (rf95.waitAvailableTimeout(100))
  {
    LoRaMessage msg;
    if (!rf95.recv(&msg.buf[0], &msg.len))
    {
      Serial.println("Recv failed ...");
    }
    msg.rssi = rf95.lastRssi();
    cBuffer.put(msg);
  }
  while (onReceiveCb && !cBuffer.empty())
  {
    onReceiveCb(cBuffer.get());
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
  String msg("Hello ");
  msg += cnt;
  send(reinterpret_cast<uint8_t const*>(msg.c_str()), static_cast<uint8_t>(msg.length() + 1));
}

void Heltec_ESP32::send(uint8_t const* buf, uint8_t buflen)
{
  rf95.setModeTx();
  Serial.print("Sending message of len = ");
  Serial.print(buflen, DEC);
  Serial.print(": '");
  Serial.write(buf, buflen);
  Serial.println("'");
  rf95.send(buf, buflen);
  delay(10);
  rf95.waitPacketSent();
}

void globalOnButton()
{
  if (digitalRead(Heltec_ESP32::BUTTON) == LOW)
  {
    Heltec.flagButton = true;
  }
}

Heltec_ESP32 Heltec;
