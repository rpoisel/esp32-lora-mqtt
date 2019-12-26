/*
 * HelTec Automation(TM) WIFI_LoRa_32 factory test code, witch includ
 * follow functions:
 *
 * - Basic OLED function test;
 *
 * - Basic serial port test(in baud rate 115200);
 *
 * - LED blink test;
 *
 * - WIFI join and scan test;
 *
 * - LoRa Ping-Pong test(DIO0 -- GPIO26 interrup check the new incoming
 * messages;
 *
 * - Timer test and some other Arduino basic functions.
 *
 * by Aaron.Lee from HelTec AutoMation, ChengDu, China
 * www.heltec.cn
 *
 * this project also realess in GitHub:
 * https://github.com/HelTecAutomation/Heltec_ESP32
 */

#include <Arduino.h>
#include <WiFi.h>
#include <heltec.h>

constexpr long const BAND = 868E6; // you can set band here directly,e.g. 868E6,915E6

static String packet;
static int packetSize;
static size_t counterSend;
static size_t counterRecv;
static volatile bool sendFlag = false;

static void onReceive(int packetSize);

void WIFISetUp()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously
  // connected
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
    Heltec.display.drawString(0, 0, "Connecting...");
    Heltec.display.display();
  }

  Heltec.display.clear();
  if (WiFi.status() == WL_CONNECTED)
  {
    Heltec.display.drawString(0, 0, "Connecting...OK.");
    Heltec.display.display();
  }
  else
  {
    Heltec.display.clear();
    Heltec.display.drawString(0, 0, "Connecting...Failed");
    Heltec.display.display();
  }
  Heltec.display.drawString(0, 10, "WIFI Setup done");
  Heltec.display.display();
}

static void interrupt_GPIO0()
{
  delay(10);
  if (digitalRead(Heltec_ESP32::BUTTON) == LOW && digitalRead(Heltec_ESP32::LED) == LOW)
  {
    sendFlag = true;
  }
}

void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, true /*Serial Enable*/,
               true /*LoRa use PABOOST*/, BAND /*LoRa RF working band*/);

  Heltec.display.clear();

  WIFISetUp();
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);

  attachInterrupt(Heltec_ESP32::BUTTON, interrupt_GPIO0, FALLING);
  LoRa.onReceive(onReceive);
  Heltec.send(counterSend);
  counterSend++;
  Heltec.drawRecv();
}

void loop()
{
  Heltec.display.clear();

  if (sendFlag)
  {
    Heltec.send(counterSend);
    counterSend++;
    sendFlag = false;
  }
  LoRa.receive();

  if (counterRecv == 0)
  {
    Heltec.drawRecv();
  }
  else
  {
    Heltec.drawRecv(packetSize, packet, LoRa.packetRssi());
  }
  Heltec.drawSend(counterSend);
  Heltec.display.display();
}

static void onReceive(int pSize)
{
  packet = "";
  while (LoRa.available())
  {
    packet += (char)LoRa.read();
  }

  packetSize = pSize;

  counterRecv++;
}
