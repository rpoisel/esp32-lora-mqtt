#include "config.h"

#include <heltec.h>

#include <AES.h>
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
static AES256 aes256;

static void messageReceived(LoRaMessage const& msg);
static void buttonPressed(ButtonState state);
static void displayInfo(SSD1306Wire* display);

void setup()
{
  Heltec.begin(ENABLE_DISPLAY, ENABLE_LORA, ENABLE_SERIAL, LORA_BAND, &messageReceived,
               &buttonPressed, &displayInfo);
  for (auto const& p : WIFI_CREDENTIALS)
  {
    wiFiMulti.addAP(p.first, p.second);
  }
  pubSubClient.setServer(MQTT_BROKER, MQTT_PORT);
  aes256.setKey(&AES_KEY[0], aes256.keySize());
}

void loop()
{
  static bool connConfigured = false;
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
    Heltec.loop();
  }
  delay(20);
}

static void messageReceived(LoRaMessage const& msg)
{
  uint8_t decrypted[16];
  counterRecv++;
  lastPacket = msg;
  if (pubSubClient.connected())
  {
    if (msg.len != 16)
    {
      pubSubClient.publish(MQTT_TOPIC_OTHER, &msg.buf[0], msg.len);
      return;
    }
    aes256.decryptBlock(decrypted, &msg.buf[0]);
    if (decrypted[0] == 'R' && decrypted[1] == 'P' && decrypted[2] == 'O')
    {
      uint8_t payload[13];
      for (size_t cnt = 0; cnt < decrypted[3]; cnt++)
      {
        payload[cnt] = decrypted[cnt + 4 /* RPOx */];
      }
      pubSubClient.publish(MQTT_TOPIC, &payload[0], decrypted[3]);
    }
    else
    {
      pubSubClient.publish(MQTT_TOPIC_OTHER, &msg.buf[0], msg.len);
    }
  }
  else
  {
    Serial.print("MQTT disconnected. Message dropped: ");
    Serial.println(reinterpret_cast<char const*>(&msg.buf[0]));
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

  display->drawString(0, 30, pubSubClient.connected() ? "MQTT connected." : "MQTT disconnected.");

  if (counterSend == 0)
  {
    display->drawString(0, 50, "No packet sent yet.");
  }
  else
  {
    display->drawString(0, 50, "Packet " + String(counterSend, DEC) + " sent done");
  }
}
