#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <Arduino.h>

constexpr int const LORA_BUF_LEN = 255; // see LoRa.cpp
struct LoRaMessage
{
  int len;
  int rssi;
  byte buf[LORA_BUF_LEN];
};

using LoRaNodeID = uint16_t;
using LoRaNonce = uint32_t;

enum LoRaCommand : uint8_t
{
  Invalid = 0,
  GetNonce,
  PutNonce,
  SensorData
};

enum RC : uint8_t
{
  SUCCESS = 0,
  FAILURE
};

struct LoRaPayload
{
  static RC fromByteStream(uint8_t const* buf, size_t len, LoRaPayload& payload)
  {
    if (len != size())
    {
      return FAILURE;
    }
    payload.signature[0] = buf[0];
    payload.signature[1] = buf[1];
    payload.signature[2] = buf[2];
    payload.nodeID = (static_cast<LoRaNodeID>(buf[4]) << 8) + buf[3];
    payload.cmd = static_cast<LoRaCommand>(buf[5]);
    for (size_t cnt = 0; cnt < sizeof(payload.data); cnt++)
    {
      payload.data[cnt] = buf[6 + cnt];
    }
    return SUCCESS;
  }
  static RC toByteStream(uint8_t* buf, size_t len, LoRaPayload const& payload)
  {
    if (len != size())
    {
      return FAILURE;
    }
    buf[0] = payload.signature[0];
    buf[1] = payload.signature[1];
    buf[2] = payload.signature[2];
    buf[3] = payload.nodeID & 0x00ff;
    buf[4] = payload.nodeID >> 8;
    buf[5] = payload.cmd;
    for (size_t cnt = 0; cnt < sizeof(payload.data); cnt++)
    {
      buf[6 + cnt] = payload.data[cnt];
    }
    return SUCCESS;
  }
  static constexpr size_t size()
  {
    return 16;
  }
  LoRaPayload() : LoRaPayload(0)
  {
  }
  LoRaPayload(LoRaNodeID nodeID) : signature{'R', 'P', 'O'}, nodeID{nodeID}, cmd{Invalid}, data{}
  {
  }
  LoRaPayload(LoRaNodeID nodeID, LoRaNonce nonce)
      : signature{'R', 'P', 'O'}, nodeID{nodeID}, cmd{PutNonce}, data{}
  {
    this->nonce = nonce;
  }
  LoRaPayload(LoRaNodeID nodeID, uint32_t value, LoRaNonce nonce)
      : signature{'R', 'P', 'O'}, nodeID{nodeID}, cmd{SensorData}, data{}
  {
    this->sensordata.value = value;
    this->sensordata.nonce = nonce;
  }

  bool signatureOK() const
  {
    return signature[0] == 'R' && signature[1] == 'P' && signature[2] == 'O';
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

#endif /* DATATYPES_H_ */
