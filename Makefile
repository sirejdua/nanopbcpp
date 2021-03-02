# Include the nanopb provided Makefile rules
include nanopb/extra/nanopb.mk

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0
CFLAGS += "-I$(NANOPB_DIR)"

# C source code files that are required
#CSRC  = simple.c                   # The main program
CSRC  = image.c                   # The main program
CSRC += simple.pb.c                # The compiled protocol definition
CSRC += image.pb.c                # The compiled protocol definition
CSRC += $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
CSRC += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
CSRC += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts

# Build rule for the main program
image: $(CSRC)
	$(CXX) $(CFLAGS) -oimage $(CSRC)

test:
	cat /dev/urandom | head -c 10000 &> binarytestdata
	./image binarytestdata

# Build rule for the protocol
image.pb.c:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. image.proto

simple.pb.c:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. simple.proto

clean:
	-rm *.o *.pb.c *.pb.h image simple binarytestdata
