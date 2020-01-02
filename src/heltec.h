#ifndef HELTEC_H_
#define HELTEC_H_

#include "circular.h"
#include "datatypes.h"

#include <Arduino.h>
#include <SSD1306Wire.h>

using ButtonState = uint8_t;
using ReceiveCb = void (*)(LoRaMessage const& msg);
using ButtonCb = void (*)(ButtonState state);
using DrawCb = void (*)(SSD1306Wire* display);

void globalOnButton();

class Heltec_ESP32
{
  public:
  static constexpr uint8_t const BUTTON = 0;
  static constexpr uint8_t const LED = 25;
  static constexpr uint8_t const Vext = 21;
  static constexpr uint8_t const SDA_OLED = 4;
  static constexpr uint8_t const SCL_OLED = 15;
  static constexpr uint8_t const RST_OLED = 16;
  static constexpr uint8_t const RST_LoRa = 14;
  static constexpr uint8_t const DIO0 = 26;

  Heltec_ESP32();
  ~Heltec_ESP32();

  void begin(bool DisplayEnable = true, bool LoRaEnable = true, bool SerialEnable = true,
             long BAND = 868E6, ReceiveCb receiveCb = nullptr, ButtonCb buttonCb = nullptr,
             DrawCb drawCb = nullptr);
  void loop();

  void send(size_t cnt);

  /* wifi kit 32 and WiFi LoRa 32(V1) do not have vext */
  void VextON();
  void VextOFF();

  private:
  static constexpr size_t const LORA_QUEUE_LEN = 4;

  SSD1306Wire display;
  ReceiveCb onReceiveCb;
  ButtonCb onButtonCb;
  DrawCb onDrawCb;
  volatile bool flagButton;
  circular_buffer<LoRaMessage, 4> cBuffer;

  friend void globalOnButton();
};

extern Heltec_ESP32 Heltec;

#endif /* HELTEC_H_ */