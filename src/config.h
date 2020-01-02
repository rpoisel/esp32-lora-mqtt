#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

#include <cstddef>
#include <vector>

namespace LoRaGateway
{
constexpr bool const ENABLE_DISPLAY = true;
constexpr bool const ENABLE_LORA = true;
constexpr bool const ENABLE_SERIAL = true;
constexpr double const LORA_BAND = 868E6;
constexpr uint8_t const AES_KEY[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

constexpr size_t const TASK_DEFAULT_STACK = 2048;
constexpr char const* WORKER_NAME = "LoRaMsgReader";

constexpr char const* MQTT_BROKER = "192.168.88.241";
constexpr int const MQTT_PORT = 1883;
constexpr char const* MQTT_TOPIC = "/LoRa/Node1/msg";
constexpr char const* MQTT_TOPIC_OTHER = "/LoRa/Node1/other";
constexpr char const* MQTT_CLIENTID = "ESP LORA2MQTT GW";

using PairCredentials = std::pair<char const*, char const*>;
using VectorCredentials = std::vector<PairCredentials>;
VectorCredentials const WIFI_CREDENTIALS = {
    // {"SSID", "PASSWORD"},
};
} // namespace LoRaGateway

#endif /* CONFIG_H_ */