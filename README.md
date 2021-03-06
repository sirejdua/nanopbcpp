# NanoPB C++ Wrapper

NanoPB C++ Wrapper is a wrapper around the [NanoPB library]() that provides a simple interface to serialize and deserialize structs generated by NanoPB.

## Installation

Clone this repository and its submodule.
```bash
git clone --recurse-submodules https://github.com/sirejdua/nanopbcpp
```

## Usage

The API is shown below for clarity.

```cpp
template<class T, class Tfields>
std::string NanoPBSerialize(T const& a, Tfields fields);


template<class T, class Tfields>
T NanoPBParse(std::string serialized, Tfields fields);
```

An example of the usage from `simple.cpp` is reproduced below.
```cpp
// Construct SimpleMessage to be serialized
SimpleMessage message = SimpleMessage_init_zero;
message.lucky_number = 13;
// Using serialization API
std::string serialized_message = NanoPBSerialize(message, SimpleMessage_fields);

// Using parsing API
SimpleMessage message = NanoPBParse<SimpleMessage>(serialized_message, SimpleMessage_fields);
```

## Tests

We have included two tests that can be run by running the following make targets:
```bash
make test_image
make test_simple
```

`make test_simple` exhibits typical usage of the NanoPB wrapper API, and just serializes and parses a single object.

`make test_image` uses the `image.proto` protobuf generation and simulates an image transfer between a server thread and a device thread. It uses random data to fill in the firmware image file `binarytestdata`, and the server thread reads this file in 2048 byte chunks to fill a `FirmwareImagePage` protobuf, along with its crc. It communicates to the device thread over a pipe, and the device thread reads each `FirmwareImagePage`, checks the crc, and writes the result to the output file `binarytestdata.out`. When every chunk has been read, the process terminates and the test is verified for success by comparing the input `binarytestdata` with the output `binarytestdata.out`; these need to be the same.
