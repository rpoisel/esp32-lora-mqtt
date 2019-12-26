#ifndef HELTEC_H_
#define HELTEC_H_

#include <Arduino.h>
#include <SSD1306Wire.h>

#include <functional>

void globalOnReceive(int pSize);
using ButtonState = uint8_t;
void globalOnButton();
using ReceiveCb = std::function<void(String const& packet, int rssi)>;
using ButtonCb = std::function<void(ButtonState state)>;
using DrawCb = std::function<void(SSD1306Wire* display)>;

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
             bool PABOOST = true, long BAND = 868E6);
  void loop();

  void onReceive(ReceiveCb const& cb);
  void onButton(ButtonCb const& cb);
  void onDraw(DrawCb const& cb);

  void send(size_t cnt);

  /* wifi kit 32 and WiFi LoRa 32(V1) do not have vext */
  void VextON();
  void VextOFF();

  private:
  SSD1306Wire display;
  ReceiveCb onReceiveCb;
  ButtonCb onButtonCb;
  DrawCb onDrawCb;
  volatile bool flagButton;

  friend void globalOnReceive(int pSize);
  friend void globalOnButton();
};

extern Heltec_ESP32 Heltec;

#endif /* HELTEC_H_ */
