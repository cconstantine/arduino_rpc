#include <pb_decode.h>
#include <pb_encode.h>
#include <message.pb.h>
#include <Stream.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "big_rf24.h"

class Rpc {
 public:
  Rpc();
  virtual bool   next_message(com_example_glowybits_rcp_RpcMessage *msg);
  virtual size_t next_packet() = 0;
 
  virtual bool   send_message(com_example_glowybits_rcp_RpcMessage *msg);
  virtual size_t send_packet(void* buf, size_t s) = 0;
 protected:

  size_t in_buffer_size;
  uint8_t in_buffer[com_example_glowybits_rcp_RpcMessage_size];
};

class SerialRpc : Rpc {
 public:
  SerialRpc(Stream&);
    
  virtual bool   next_message(com_example_glowybits_rcp_RpcMessage *msg);
  virtual bool   send_message(com_example_glowybits_rcp_RpcMessage *msg);

  virtual size_t next_packet();
  virtual size_t send_packet(void* buf, size_t s);
  
 private:

  void state();
  void reset();

  Stream&  serial;
  uint32_t lastRid;
  uint16_t to_read;
  uint16_t read_offset;

};


class RF24Rpc : Rpc { 
 public:
  RF24Rpc(BigRF24& radio);

  virtual bool   next_message(com_example_glowybits_rcp_RpcMessage *msg);
  virtual bool   send_message(com_example_glowybits_rcp_RpcMessage *msg);

  virtual size_t next_packet();
  virtual size_t send_packet(void* buf, size_t s);

 private:
  BigRF24& radio;
};
