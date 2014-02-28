#include <Arduino.h>
#include "bluetooth.h"

void Rpc::init() {
  Rpc::instance = new Rpc();
}

Rpc::Rpc(): to_read(0), lastRid(1) {
  memset(in_buffer, 0, sizeof(in_buffer));
}

Rpc* Rpc::instance;

int Rpc::next_message(com_example_glowybits_rcp_RpcMessage *msg) {
  uint32_t msg_size = 0;
  if( Serial1.available() >= 2) {   
    msg_size = Serial1.read();
    msg_size = (msg_size << 8) + Serial1.read();
    
    for(int i = 0;i < msg_size;++i) {
      while(!Serial1.available() );

      in_buffer[i] = Serial1.read();
    }
    pb_istream_t istream = pb_istream_from_buffer(in_buffer, msg_size);
    
    if (pb_decode(&istream, com_example_glowybits_rcp_RpcMessage_fields, msg)) {
      return msg_size;
    }
  }
  return 0;
}

int Rpc::send_message(com_example_glowybits_rcp_RpcMessage *msg) {
  uint8_t out_buffer[com_example_glowybits_rcp_RpcMessage_size + 2];

  pb_ostream_t ostream = pb_ostream_from_buffer(out_buffer+2, com_example_glowybits_rcp_RpcMessage_size);
  pb_encode(&ostream, com_example_glowybits_rcp_RpcMessage_fields, msg);

  out_buffer[0] = 0xFF & (ostream.bytes_written >> 8);
  out_buffer[1] = 0xFF & ostream.bytes_written;
  
  int to_write = ostream.bytes_written + 2;
  for(int i = 0;i < to_write;) {
    i += Serial1.write(out_buffer[i]);
  }
  return to_write;
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
  
  
