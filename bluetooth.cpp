#include <Arduino.h> 
#include "bluetooth.h"

Rpc::Rpc() : in_buffer_size(sizeof(in_buffer))
{ }

bool Rpc::next_message(com_example_glowybits_rcp_RpcMessage *msg) {
  size_t bytesRead = next_packet();
  if (bytesRead) {
    pb_istream_t istream = pb_istream_from_buffer(in_buffer, bytesRead);
    return pb_decode(&istream, com_example_glowybits_rcp_RpcMessage_fields,   msg);
  }
  return false;
}

bool Rpc::send_message(com_example_glowybits_rcp_RpcMessage *msg) {
  uint8_t out_buffer[com_example_glowybits_rcp_RpcMessage_size];

  pb_ostream_t ostream = pb_ostream_from_buffer(out_buffer, com_example_glowybits_rcp_RpcMessage_size);

  if (!pb_encode(&ostream, com_example_glowybits_rcp_RpcMessage_fields, msg)) {
    return false;
  }
  return send_packet(out_buffer, ostream.bytes_written) == (ostream.bytes_written + 2);
}


SerialRpc::SerialRpc(Stream& s):
  Rpc(),
  serial(s),
  lastRid(0),
  to_read(0),
  read_offset(0)
{ }

void SerialRpc::reset() {
  to_read = 0;
  read_offset = 0;
  memset(in_buffer, 0, in_buffer_size);
}

bool SerialRpc::next_message(com_example_glowybits_rcp_RpcMessage *msg) {
  bool ret = Rpc::next_message(msg);
  if (ret) {
    reset();
  }
  return ret;
}

bool SerialRpc::send_message(com_example_glowybits_rcp_RpcMessage *msg) {
  return Rpc::send_message(msg);
}


size_t SerialRpc::next_packet() {  
  if(to_read == 0 && read_offset == 0 && serial.available() >= 2) {
    to_read = serial.read();
    to_read = (to_read << 8) + serial.read();

    if (to_read > in_buffer_size) {
      while(serial.available()) {
        serial.read();
      }
      reset();
    }
    return 0;
  } else if (to_read == 0 && read_offset != 0) {
    return read_offset;
  } else if (to_read > 0 && serial.available()) {
    in_buffer[read_offset] = serial.read();
    ++read_offset;
    to_read -= 1;
  }
  return 0;
}

size_t SerialRpc::send_packet(void* buf, size_t size) {

  serial.write((uint8_t)(0xFF & (size >> 8)));
  serial.write((uint8_t)(0xFF & size));
  
  for(size_t i = 0;i < size;) {
    i += serial.write(((char*)buf)[i]);
  }
  return  size;
}


RF24Rpc::RF24Rpc(BigRF24& r) : Rpc(), radio(r)
{ }
  

bool RF24Rpc::next_message(com_example_glowybits_rcp_RpcMessage *msg)
{
  return Rpc::next_message(msg);
}

bool RF24Rpc::send_message(com_example_glowybits_rcp_RpcMessage *msg)
{
  return Rpc::send_message(msg);
}

size_t RF24Rpc::next_packet()
{
  size_t ret = 0;
  if (radio.available()) {
    ret = radio.read(in_buffer, in_buffer_size);
  }
  
  return ret;
}

size_t RF24Rpc::send_packet(void* buf, size_t s)
{
  if(radio.write(buf, s)) {
    return s;
  }

  return 0;
}
