#include "config.h"

#include <heltec.h>

#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

using namespace LoRaGateway;

static WiFiMulti wiFiMulti;
static WiFiClient wiFiClient;
static PubSubClient pubSubClient(wiFiClient);
static size_t counterRecv;
static size_t counterSend;
static LoRaMessage lastPacket;
static bool connConfigured;

static void messageReceived(LoRaMessage const& msg);
static void buttonPressed(ButtonState state);
static void displayInfo(SSD1306Wire* display);

void setup()
{
  Heltec.begin(ENABLE_DISPLAY, ENABLE_LORA, ENABLE_SERIAL, ENABLE_LORA_BOOST, LORA_BAND,
               &messageReceived, &buttonPressed, &displayInfo);
  for (auto const& p : WIFI_CREDENTIALS)
  {
    wiFiMulti.addAP(p.first, p.second);
  }
  pubSubClient.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop()
{
  if (wiFiMulti.run() == WL_CONNECTED)
  {
    if (!connConfigured)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connConfigured = true;
    }
    if (!pubSubClient.connected())
    {
      if (pubSubClient.connect(MQTT_CLIENTID))
      {
        Serial.print("MQTT connected. Publishing to ");
        Serial.print(MQTT_TOPIC);
        Serial.println(".");
      }
    }
    Heltec.loop();
    if (pubSubClient.connected())
    {
      pubSubClient.loop();
    }
  }
  else
  {
    connConfigured = false;
  }
  delay(20);
}

static void messageReceived(LoRaMessage const& msg)
{
  counterRecv++;
  lastPacket = msg;
  if (pubSubClient.connected())
  {
    pubSubClient.publish(MQTT_TOPIC, &msg.buf[0], msg.len);
  }
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
    display->drawString(0, 20, "With RSSI: " + String(lastPacket.rssi, DEC));
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
