#pragma once

#include <cstddef>
#include <vector>

namespace LoRaGateway
{
constexpr bool const ENABLE_DISPLAY = true;
constexpr bool const ENABLE_LORA = true;
constexpr bool const ENABLE_LORA_BOOST = true;
constexpr bool const ENABLE_SERIAL = true;
constexpr double const LORA_BAND = 868E6;

constexpr size_t const TASK_DEFAULT_STACK = 2048;
constexpr char const* WORKER_NAME = "LoRaMsgReader";

constexpr char const* MQTT_BROKER = "192.168.88.241";
constexpr int const MQTT_PORT = 1883;
constexpr char const* MQTT_TOPIC = "/LoRa/Node1/msg";
constexpr char const* MQTT_CLIENTID = "ESP LORA2MQTT GW";

std::vector<std::pair<char const*, char const*>> const WIFI_CREDENTIALS = {
    // {"SSID", "PASSWORD"},
};
} // namespace LoRaGateway
