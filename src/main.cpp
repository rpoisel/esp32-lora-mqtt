#include "config.h"

#include <heltec.h>

#include <AES.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <esp_system.h>

using namespace LoRaGateway;

constexpr LoRaNodeID const LORA_GW_ID = 0xffff;

static Config config;
static WiFiMulti wiFiMulti;
static WiFiClient wiFiClient;
static PubSubClient pubSubClient(wiFiClient);
static size_t counterRecv;
static size_t counterSend;
static LoRaMessage lastMessage;
static AES256 aes256;

static void messageReceived(LoRaMessage const& msg);
static void buttonPressed(ButtonState state);
static void displayInfo(SSD1306Wire* display);

void setup()
{
  EEPROM.begin(512);
#if 0
  EEPROM.put(0, config);
  EEPROM.commit();
#else
  EEPROM.get(0, config);
  if (!config.signatureOK())
  {
    config = Config();
  }
#endif
  Heltec.begin(&messageReceived, &buttonPressed, &displayInfo);
  Serial.println("WiFi configuration: ");
  for (auto const& cred : config.wifi_credentials)
  {
    if (cred.ssid[0] == '\0')
    {
      break;
    }
    Serial.print("  Adding AP ssid=");
    Serial.print(cred.ssid);
    Serial.print(", pass=");
    Serial.println(cred.password);
    wiFiMulti.addAP(cred.ssid, cred.password);
  }
  pubSubClient.setServer(config.mqtt_broker, config.mqtt_port);
  aes256.setKey(&config.aes_key[0], sizeof(config.aes_key));
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
      if (pubSubClient.connect(config.mqtt_clientid))
      {
        Serial.print("MQTT connected. Publishing to ");
        Serial.print(config.mqtt_topic);
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
  counterRecv++;
  lastMessage = msg;
  if (msg.len != LoRaPayload::size())
  {
    Serial.print("Message with length != ");
    Serial.print(LoRaPayload::size(), DEC);
    Serial.print(" received: '");
    Serial.write(msg.buf, msg.len);
    Serial.println("'");
    if (pubSubClient.connected())
    {
      pubSubClient.publish(config.mqtt_topic_other, &msg.buf[0], msg.len);
    }
    return;
  }

  uint8_t cleartext[LoRaPayload::size()];
  LoRaPayload payload;

  aes256.decryptBlock(cleartext, &msg.buf[0]);
  LoRaPayload::fromByteStream(cleartext, sizeof(cleartext), payload);
  if (!payload.signatureOK())
  {
    if (pubSubClient.connected())
    {
      pubSubClient.publish(config.mqtt_topic_other, &msg.buf[0], msg.len);
    }
    Serial.println("Message with incorrect signature received.");
    return;
  }
  if (payload.cmd == GetNonce)
  {
    LoRaPayload response(LORA_GW_ID);
    response.cmd = PutNonce;
    response.nonce = esp_random();

    // TODO: store nonce for given payload->nodeID

    uint8_t encrypted[LoRaPayload::size()];
    uint8_t cleartext[LoRaPayload::size()];
    LoRaPayload::toByteStream(cleartext, sizeof(cleartext), response);
    aes256.encryptBlock(encrypted, cleartext);
    delay(1000);
    Serial.print("Sending nonce: ");
    Serial.println(response.nonce, HEX);
    Heltec.send(encrypted, sizeof(encrypted));
  }
  else if (payload.cmd == SensorData)
  {
    Serial.println("Received sensor data.");
    String mqttPayload("Node ");
    // TODO: check whether payload->sensordata.nonce is valid
    mqttPayload += payload.nodeID;
    mqttPayload += ": ";
    mqttPayload += payload.sensordata.value;
    if (pubSubClient.connected())
    {
      pubSubClient.publish(config.mqtt_topic, mqttPayload.c_str(), mqttPayload.length() + 1);
    }
    ::strncpy(reinterpret_cast<char*>(&lastMessage.buf[0]), mqttPayload.c_str(),
              mqttPayload.length() + 1 /* TODO: check */);
    // TODO: invalidate nonce
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
    display->drawString(0, 0,
                        "Packet " + String(counterRecv, DEC) + ", size " +
                            String(lastMessage.len, DEC) + ":");
    display->drawString(0, 10, reinterpret_cast<char const*>(lastMessage.buf));
    display->drawString(0, 20, "With RSSI: " + String(lastMessage.rssi, DEC));
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
