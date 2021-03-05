# Include the nanopb provided Makefile rules
include nanopb/extra/nanopb.mk

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -Werror -g -O0 -std=c++11 -pthread
CFLAGS += "-I$(NANOPB_DIR)"

# C source code files that are required
CSRC_COMMON  = $(NANOPB_DIR)/pb_encode.c  # The nanopb encoder
CSRC_COMMON += $(NANOPB_DIR)/pb_decode.c  # The nanopb decoder
CSRC_COMMON += $(NANOPB_DIR)/pb_common.c  # The nanopb common parts

CSRC_SIMPLE  = $(CSRC_COMMON)
CSRC_SIMPLE += simple.cpp
CSRC_SIMPLE += simple.pb.c

CSRC_IMAGE  = $(CSRC_COMMON)
CSRC_IMAGE += image.cpp
CSRC_IMAGE += image.pb.c

# Build rule for the main program
image: $(CSRC_IMAGE)
	$(CXX) $(CFLAGS) -oimage $(CSRC_IMAGE)

simple: $(CSRC_SIMPLE)
	$(CXX) $(CFLAGS) -osimple $(CSRC_SIMPLE)

all: image simple binarytestdata

test_image: image binarytestdata
	./image binarytestdata; \
	diff binarytestdata binarytestdata.out; \
	if [ $$? -eq 0 ]; then \
		echo "Test passed"; \
	else \
		echo "Test failed"; \
	fi

test_simple: simple
	./simple

valgrind: image binarytestdata
	valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=valgrind-out.txt \
		./image binarytestdata

binarytestdata:
	cat /dev/urandom | head -c 12345 > binarytestdata

# Build rule for the protocol
image.pb.c:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. image.proto

simple.pb.c:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=. simple.proto

clean:
	-rm *.o *.pb.c *.pb.h image simple valgrind-out.txt binarytestdata binarytestdata.out
