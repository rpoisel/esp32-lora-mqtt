#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>
#include <cstddef>

constexpr size_t const LORA_BUF_LEN = 255; // see LoRa.cpp
struct LoRaMessage
{
  size_t len;
  int rssi;
  byte buf[LORA_BUF_LEN];
};

#endif /* DATATYPES_H_ */