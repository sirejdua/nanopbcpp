#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <pb_encode.h>
#include <pb_decode.h>
#include "image.pb.h"
#include "pb_wrapper.h"
#include "crc.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./image binarydata" << std::endl;
        return 1;
    }

    std::ifstream binarydata;
    binarydata.open(argv[1]);
    gridware_FirmwareImagePage image;
    while(!binarydata.eof())
    {
        binarydata.read(reinterpret_cast<char*>(&image.page.bytes), 2048);
        std::streamsize s = binarydata.gcount();
        if (s == 0)
        {
            break;
        }
        image.page.size = s;
        std::cout << "read 1 chunk size = " << s << "\n";
        crc(reinterpret_cast<unsigned char*>(&image.page.bytes), s, reinterpret_cast<unsigned char*>(&image.crc.bytes));
        image.crc.size = 2;
        image.last = binarydata.eof();
        std::string serialized_image = NanoPBSerialize(image, gridware_FirmwareImagePage_fields);
        std::cout << serialized_image;
    }
    
    return 0;
}

