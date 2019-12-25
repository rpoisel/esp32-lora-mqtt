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

static int rssi;
static int packSize;
static String packet;
static unsigned int counter = 0;
static bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.

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
    //		delay(500);
  }
  else
  {
    Heltec.display.clear();
    Heltec.display.drawString(0, 0, "Connecting...Failed");
    Heltec.display.display();
    // while(1);
  }
  Heltec.display.drawString(0, 10, "WIFI Setup done");
  Heltec.display.display();
}

void WIFIScan(unsigned int value)
{
  unsigned int i;
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_MODE_NULL);
  }
  for (i = 0; i < value; i++)
  {
    Heltec.display.drawString(0, 20, "Scan start...");
    Heltec.display.display();

    int n = WiFi.scanNetworks();
    Heltec.display.drawString(0, 30, "Scan done");
    Heltec.display.display();
    Heltec.display.clear();

    if (n == 0)
    {
      Heltec.display.clear();
      Heltec.display.drawString(0, 0, "no network found");
      Heltec.display.display();
      // while(1);
    }
    else
    {
      Heltec.display.drawString(0, 0, (String)n);
      Heltec.display.drawString(14, 0, "networks found:");
      Heltec.display.display();

      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Heltec.display.drawString(0, (i + 1) * 9, (String)(i + 1));
        Heltec.display.drawString(6, (i + 1) * 9, ":");
        Heltec.display.drawString(12, (i + 1) * 9, (String)(WiFi.SSID(i)));
        Heltec.display.drawString(90, (i + 1) * 9, " (");
        Heltec.display.drawString(98, (i + 1) * 9, (String)(WiFi.RSSI(i)));
        Heltec.display.drawString(114, (i + 1) * 9, ")");
        //            display.println((WiFi.encryptionType(i) ==
        //            WIFI_AUTH_OPEN)?" ":"*");
        delay(10);
      }
    }

    Heltec.display.display();
    delay(800);
    Heltec.display.clear();
  }
}

static bool resendflag = false;
static void interrupt_GPIO0()
{
  delay(10);
  if (digitalRead(BUTTON) == LOW && digitalRead(LED) == LOW)
  {
    resendflag = true;
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

  WIFIScan(1);

  attachInterrupt(BUTTON, interrupt_GPIO0, FALLING);
  LoRa.onReceive(onReceive);
  Heltec.send(counter);
  counter++;
  LoRa.receive();
  Heltec.displaySendReceive();
}

void loop()
{
  if (resendflag)
  {
    resendflag = false;
    Heltec.send(counter);
    counter++;
    LoRa.receive();
    Heltec.displaySendReceive(counter, packSize, packet, rssi);
  }
  if (receiveflag)
  {
    digitalWrite(LED, HIGH);
    Heltec.displaySendReceive(counter, packSize, packet, rssi);
    delay(10);
    receiveflag = false;
    Heltec.send(counter);
    counter++;
    LoRa.receive();
    Heltec.displaySendReceive(counter, packSize, packet, rssi);
    digitalWrite(LED, LOW);
  }
}

static void onReceive(int packetSize) // LoRa receiver interrupt service
{
  packet = "";
  packSize = packetSize;

  while (LoRa.available())
  {
    packet += (char)LoRa.read();
  }

  Serial.println(packet);
  rssi = LoRa.packetRssi();
  receiveflag = true;
}
