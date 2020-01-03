#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

#include <cstddef>

namespace LoRaGateway
{
constexpr uint8_t EEPROM_SIGNATURE[4]{0x5c, 0xa1, 0xab, 0x1e};

#pragma pack(push)
#pragma pack(1)
struct Config
{
  Config()
      : signature{EEPROM_SIGNATURE[0], EEPROM_SIGNATURE[1], EEPROM_SIGNATURE[2],
                  EEPROM_SIGNATURE[3]},
        enable_display{true}, enable_lora{true}, enable_serial{true},
        lora_band(868E6), aes_key{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                                  0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                  0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
        mqtt_broker{"192.168.88.241"}, mqtt_port{1883}, mqtt_topic{"/LoRa/Node1/msg"},
        mqtt_topic_other{"/LoRa/Node1/other"}, mqtt_clientid{"ESP LoRa2MQTT GW"},
        num_wifi_credentials{0}, wifi_credentials{}
  {
  }
  uint8_t signature[4];
  bool enable_display;
  bool enable_lora;
  bool enable_serial;
  double lora_band;
  uint8_t aes_key[32];
  char mqtt_broker[32];
  int mqtt_port;
  char mqtt_topic[32];
  char mqtt_topic_other[32];
  char mqtt_clientid[32];
  size_t num_wifi_credentials;
  struct
  {
    char ssid[32];
    char password[32];
  } wifi_credentials[3];
}; // namespace LoRaGateway
#pragma pack(pop)
} // namespace LoRaGateway

#endif /* CONFIG_H_ */