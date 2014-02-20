#include <pb_decode.h>
#include <pb_encode.h>
#include <message.pb.h>

#include "message.pb.h"

class Rpc {
 public:
  static void init();
  
  int next_message(com_example_glowybits_rcp_RpcMessage *msg);
  
  int send_message(com_example_glowybits_rcp_RpcMessage *msg);
  
  void debug(const char* msg);
 
  static Rpc* instance;
  
 private:
  Rpc();

  uint8_t to_read;
  uint8_t in_buffer[com_example_glowybits_rcp_RpcMessage_size];

};
