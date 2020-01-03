#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>

constexpr size_t const LORA_BUF_LEN = 255; // see LoRa.cpp
struct LoRaMessage
{
  size_t len;
  int rssi;
  byte buf[LORA_BUF_LEN];
};

using LoRaNodeID = uint16_t;
using LoRaNonce = uint32_t;

enum LoRaCommand : uint8_t
{
  Invalid,
  GetNonce,
  PutNonce,
  SensorData
};

#pragma pack(push)
#pragma pack(1)

struct LoRaPayload
{
  LoRaPayload() : LoRaPayload(0)
  {
  }
  LoRaPayload(uint16_t nodeID) : signature{'R', 'P', 'O'}, nodeID{nodeID}, cmd{Invalid}, data{0}
  {
  }
  uint8_t signature[3]; // RPO
  LoRaNodeID nodeID;
  LoRaCommand cmd;
  union {
    uint8_t data[10];
    LoRaNonce nonce;
    struct
    {
      uint32_t value;
      LoRaNonce nonce;
    } sensordata;
  };
};

#pragma pack(pop)

#endif /* DATATYPES_H_ */