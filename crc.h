#ifndef INCLUDE_CRC_H
#define INCLUDE_CRC_H

void crc(unsigned char* x, int len, unsigned char* result)
{
    unsigned short rem = 0;
    for (int i = 0; i < len; i++)
    {
        rem = rem ^ (x[i] << 8);
        for (int j = 1; j <= 8; j++)
        {
            if (rem & 0x8000)
            {
                rem = (rem << 1) ^ 0x1021;
            }
            else
            {
                rem = rem << 1;
            }
            rem = rem & 0xffff;
        }
    }
    result[0] = (rem >> 4) & 0xff;
    result[1] = rem & 0xff;
}


#endif
