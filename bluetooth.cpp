#include <Arduino.h> 
#include "bluetooth.h"

void Rpc::init() {
  Rpc::instance = new Rpc();
}

Rpc::Rpc(): to_read(0), lastRid(1), read_offset(0) {
  memset(in_buffer, 0, sizeof(in_buffer));
}

Rpc* Rpc::instance;

void Rpc::reset() {
  to_read = 0;
  read_offset = 0;
  memset(in_buffer, 0, sizeof(in_buffer));
}

int Rpc::next_message(com_example_glowybits_rcp_RpcMessage *msg) {
  
  if(to_read == 0 && read_offset == 0 && Serial1.available() >= 2) {
    to_read = Serial1.read();
    to_read = (to_read << 8) + Serial1.read();

    if (to_read > com_example_glowybits_rcp_RpcMessage_size) {
      while(Serial1.available()) {
        Serial1.read();
      }
      reset();
    }
    return 0;
  } else if (to_read == 0 && read_offset != 0) {
    pb_istream_t istream = pb_istream_from_buffer(in_buffer, read_offset);
    
    bool ret = pb_decode(&istream, com_example_glowybits_rcp_RpcMessage_fields, msg);
    reset();
    return ret;
  } else if (to_read > 0 && Serial1.available()) {
    in_buffer[read_offset] = Serial1.read();
    ++read_offset;
    to_read -= 1;
  }
  return 0;
}

int Rpc::send_message(com_example_glowybits_rcp_RpcMessage *msg) {
  uint8_t out_buffer[com_example_glowybits_rcp_RpcMessage_size + 2];

  pb_ostream_t ostream = pb_ostream_from_buffer(out_buffer+2, com_example_glowybits_rcp_RpcMessage_size);

  if (!pb_encode(&ostream, com_example_glowybits_rcp_RpcMessage_fields, msg)) {
    return 0;
  }

  out_buffer[0] = 0xFF & (ostream.bytes_written >> 8);
  out_buffer[1] = 0xFF & ostream.bytes_written;
  
  int to_write = ostream.bytes_written + 2;
  for(int i = 0;i < to_write;) {
    i += Serial1.write(out_buffer[i]);
  }
  return  ostream.bytes_written ;
}

void Rpc::debug(const char* desc) {
  com_example_glowybits_rcp_RpcMessage msg;
  memset(&msg, 0, sizeof(msg));
  
  msg.action = com_example_glowybits_rcp_RpcMessage_Action_DEBUG;
  msg.rid = lastRid++;
  msg.has_arg1 = false;
  
  msg.has_description = true;
  strncpy(msg.description, desc, sizeof(msg.description)-1);
  
  int delta = send_message(&msg);
}
  
  
