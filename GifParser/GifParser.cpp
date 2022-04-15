#include <iostream>
#include <fstream>
#include <sstream>

unsigned createMask(unsigned a, unsigned b)
{
    unsigned r = 0;
    for (unsigned i = a; i <= b; i++)
        r |= 1 << i;

    return r;
}

int main(int argc, char* argv[])
{
    std::ifstream gifFile;
    if (argc < 2 || argc > 2)
    {
        std::cout << "Invalid Syntax.";
        return -1;
    }

    gifFile.open(argv[1], std::ios::binary);

    if (!gifFile.is_open())
    {
        std::cout << "File can't be open\n";
        return -1;
    }

    int countLayers = 0;

    uint8_t data = 0x00;
    uint8_t buffer[768];
    const uint8_t extensionBlock = 0x21;
    const uint8_t imageBlock = 0x2c;
    const uint8_t graphicsControlExtension = 0xF9;
    const uint8_t plainTextExtension = 0x01;
    const uint8_t applicationExtension = 0xFF;
    const uint8_t commentExtension = 0xFE;
    const uint8_t trailer = 0x3B;

    const uint8_t endBlock = 0x00;

    gifFile.read((char*)buffer, 10);
    gifFile.read((char*)&data, 1);
    unsigned skipBytes = createMask(0, 3);
    unsigned colorTableSize = 3L * (1L << (skipBytes & data) + 1);

    // skip first colorTableSize bytes
    gifFile.read((char*)&buffer, colorTableSize);

    while (data != trailer)
    {
        gifFile.read((char*)& data, 1);
        if (data == extensionBlock)
        {
            gifFile.read((char*)&data, 1);
            if (data == graphicsControlExtension)
                gifFile.read((char*)buffer, 6);
            
            if (data == plainTextExtension)
            {
                while (data != endBlock)
                {
                    gifFile.read((char*)&data, 1);
                    gifFile.read((char*)buffer, data);
                }
            }
            if (data == applicationExtension)
                gifFile.read((char*)buffer, 17);

            if (data == commentExtension)
            {
                while (data != endBlock)
                {
                    gifFile.read((char*)&data, 1);
                    gifFile.read((char*)buffer, data);
                }
            }
        }
        if (data == imageBlock)
        {
            gifFile.read((char*)buffer, 10);
            while (data != endBlock)
            {
                gifFile.read((char*)&data, 1);
                gifFile.read((char*)buffer, data);
            }
            ++countLayers;
        }

    }
   

    std::cout << "Layer Count: " << countLayers << std::endl;
    gifFile.close();

    return 0;
}

