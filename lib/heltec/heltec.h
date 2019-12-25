#ifndef HELTEC_H_
#define HELTEC_H_

#if defined(ESP32)

#include <Arduino.h>
#if defined(WIFI_Kit_32) || defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) ||                   \
    defined(Wireless_Stick)
#include <SSD1306Wire.h>
#include <Wire.h>
#endif

#if defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) || defined(Wireless_Stick) ||                \
    defined(Wireless_Stick_Lite)
#include <LoRa.h>
#endif

constexpr uint8_t const BUTTON = 0;
constexpr uint8_t const LED = 25;
constexpr uint8_t const Vext = 21;
constexpr uint8_t const SDA_OLED = 4;
constexpr uint8_t const SCL_OLED = 15;
constexpr uint8_t const RST_OLED = 16;
constexpr uint8_t const RST_LoRa = 14;
constexpr uint8_t const DIO0 = 26;

class Heltec_ESP32
{
  public:
  Heltec_ESP32();
  ~Heltec_ESP32();

  void begin(bool DisplayEnable = true, bool LoRaEnable = true, bool SerialEnable = true,
             bool PABOOST = true, long BAND = 868E6);
  void logo();
  void displaySendReceive(size_t cnt, int packSize, String const& packet, int rssi);
  void displaySendReceive(String const& cnt = String("--"), String const& packSize = String("--"),
                          String const& packet = String(), String const& rssi = String("--"));
  void send(size_t cnt);
#if defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) || defined(Wireless_Stick) ||                \
    defined(Wireless_Stick_Lite)
#endif

#if defined(WIFI_Kit_32) || defined(WIFI_LoRa_32) || defined(WIFI_LoRa_32_V2) ||                   \
    defined(Wireless_Stick)
  SSD1306Wire display;
#endif

  /* wifi kit 32 and WiFi LoRa 32(V1) do not have vext */
  void VextON();
  void VextOFF();
};

extern Heltec_ESP32 Heltec;

#else
#error This library only supports boards with ESP32 processor.
#endif

#endif /* HELTEC_H_ */
