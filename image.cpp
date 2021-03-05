#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include <pb_encode.h>
#include <pb_decode.h>
#include "image.pb.h"
#include "pb_wrapper.h"
#include "crc.h"

std::string read_string_from_fd(int fd)
{
    size_t size;
    read(fd, reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string result;
    result.reserve(size);
    char next_byte = 65;
    for (size_t i = 0; i < size; i++)
    {
        read(fd, &next_byte, 1);
        result.push_back(next_byte);
    }
    return result;
}

void write_string_to_fd(int fd, std::string s)
{
    size_t size = s.size();
    write(fd, reinterpret_cast<char const*>(&size), sizeof(size_t));
    write(fd, s.c_str(), s.size());
}

void server(int fd_send, int fd_receive, char* binfile)
{
    std::cout << "S Launching server thread\n";
    std::ifstream binarydata;
    binarydata.open(binfile);
    while(!binarydata.eof())
    {
        std::cout << "S reading from file\n";
        gridware_FirmwareImagePage image;
        binarydata.read(reinterpret_cast<char*>(&image.page.bytes), 2048);
        std::streamsize s = binarydata.gcount();
        if (s == 0)
        {
            std::cout << "S breaking\n";
            break;
        }
        image.page.size = s;
        std::cout << "S read 1 chunk\n";
        crc(reinterpret_cast<unsigned char*>(&image.page.bytes), s, reinterpret_cast<unsigned char*>(&image.crc.bytes));
        image.crc.size = 2;
        image.last = binarydata.eof();
        std::string serialized_image = NanoPBSerialize(image, gridware_FirmwareImagePage_fields);
        std::cout << "S putting " << serialized_image.size() << " bytes on pipe\n";
        write_string_to_fd(fd_send, serialized_image);
        std::string device_response_serialized = read_string_from_fd(fd_receive);
        auto device_response = NanoPBParse<gridware_DeviceResponse>(device_response_serialized, gridware_DeviceResponse_fields);
        if (!device_response.verified)
        {
            std::cout << "S verification error. terminating\n";
            return;
        }
    }
    binarydata.close();
}

void device(int fd_send, int fd_receive, std::string output)
{
    std::cout << "D Launching device thread\n";
    std::ofstream outputf;
    outputf.open(output);
    bool last_page;
    do
    {
        std::cout << "D Reading page\n";
        std::string image_page_serialized = read_string_from_fd(fd_receive);
        std::cout << "D Parse page\n";
        auto image = NanoPBParse<gridware_FirmwareImagePage>(image_page_serialized, gridware_FirmwareImagePage_fields);

        gridware_DeviceResponse device_response{false};

        char recomputed_crc[2];
        crc(reinterpret_cast<unsigned char*>(&image.page.bytes), image.page.size, reinterpret_cast<unsigned char*>(recomputed_crc));
        if (memcmp(recomputed_crc, &image.page.bytes, 2) != 0)
        {
            device_response.verified = true;
        }
        last_page = image.last;
        std::string serialized_response = NanoPBSerialize(device_response, gridware_DeviceResponse_fields);
        std::cout << "D Putting " << serialized_response.size() << " bytes on pipe\n";
        write_string_to_fd(fd_send, serialized_response);

        if (device_response.verified)
        {
            outputf.write(reinterpret_cast<char*>(&image.page.bytes), image.page.size);
        }
    } while (!last_page);
    outputf.close();

}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./image binarydata" << std::endl;
        return 1;
    }
    std::cout << argv[1] << std::endl;
    auto output_file = argv[1] + std::string(".out");
    // create pipe
    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);
    // branch into threads
    std::thread server_t(server, fd1[1], fd2[0], argv[1]);
    std::thread device_t(device, fd2[1], fd1[0], output_file.c_str());

    server_t.join();
    device_t.join();
    return 0;
}

