# Include the nanopb provided Makefile rules
include ../../nanopb-0.2.6-linux-x86/extra/nanopb.mk

GLOWYBITS_ANDROID_DIR=/home/cconstantine/workspace/GlowyBits

all: message.pb.c $(GLOWYBITS_ANDROID_DIR)/src/com/example/glowybits/RpcMessage.java


# Build rule for the protocol
message.pb.c: message.proto
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. message.proto

$(GLOWYBITS_ANDROID_DIR)/src/com/example/glowybits/RpcMessage.java: message.proto
	java -jar ~/wire-wire-1.2.0/wire-compiler/target/wire-compiler-1.2.0-jar-with-dependencies.jar --java_out=$(GLOWYBITS_ANDROID_DIR)/src --proto_path=. message.proto

