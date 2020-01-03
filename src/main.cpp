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
  if (config.signature[0] != EEPROM_SIGNATURE[0] || config.signature[1] != EEPROM_SIGNATURE[1] ||
      config.signature[2] != EEPROM_SIGNATURE[2] || config.signature[3] != EEPROM_SIGNATURE[3])
  {
    config = Config();
  }
#endif
  Heltec.begin(config.enable_display, config.enable_lora, config.enable_serial, config.lora_band,
               &messageReceived, &buttonPressed, &displayInfo);
  for (size_t cnt = 0; cnt < config.num_wifi_credentials; cnt++)
  {
    wiFiMulti.addAP(config.wifi_credentials[cnt].ssid, config.wifi_credentials[cnt].password);
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
  if (pubSubClient.connected())
  {
    if (msg.len != 16)
    {
      pubSubClient.publish(config.mqtt_topic_other, &msg.buf[0], msg.len);
      return;
    }

    uint8_t decrypted[16];
    aes256.decryptBlock(decrypted, &msg.buf[0]);
    LoRaPayload const* payload = reinterpret_cast<LoRaPayload const*>(decrypted);
    if (payload->signature[0] == 'R' && payload->signature[1] == 'P' &&
        payload->signature[2] == 'O')
    {
      if (payload->cmd == GetNonce)
      {
        LoRaPayload response(LORA_GW_ID);
        uint8_t encrypted[sizeof(response)];
        response.cmd = PutNonce;
        response.nonce = esp_random();
        // TODO: store nonce for given payload->nodeID
        aes256.encryptBlock(encrypted, reinterpret_cast<uint8_t const*>(&response));
        Heltec.send(encrypted, sizeof(encrypted));
      }
      else if (payload->cmd == SensorData)
      {

        String mqttPayload("Node ");
        // TODO: payload->sensordata.nonce
        mqttPayload += payload->nodeID;
        mqttPayload += ": ";
        mqttPayload += payload->sensordata.value;
        pubSubClient.publish(config.mqtt_topic, mqttPayload.c_str(), mqttPayload.length() + 1);
        ::strncpy(reinterpret_cast<char*>(&lastMessage.buf[0]), mqttPayload.c_str(),
                  mqttPayload.length() + 1 /* TODO: check */);
        // TODO: invalidate nonce
      }
    }
    else
    {
      pubSubClient.publish(config.mqtt_topic_other, &msg.buf[0], msg.len);
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
