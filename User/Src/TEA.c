#include "main.h"
#include "stm32f1xx_hal.h"
#include "TEA.h"

/*************************************************TEA���ܽ��ܺ���************************************************/

void EncryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key)
{
    unsigned int y = *firstChunk;
    unsigned int z = *secondChunk;
    unsigned int sum = 0;

    unsigned int delta = 0x9e3779b9;

    for (int i = 0; i < 32; i++)//8������(��Ҫ��Ӧ����Ľ��ܺ��ĺ���������һ��)
    {
        sum += delta;
        y += ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        z += ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
    }

    *firstChunk = y;
    *secondChunk = z;
}

void DecryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key)
{
    unsigned int  sum = 0;
    unsigned int  y = *firstChunk;
    unsigned int  z = *secondChunk;
    unsigned int  delta = 0x9e3779b9;

    sum = delta << 5; //32�����㣬������2��5�η���16�����㣬������2��4�η���8�����㣬������2��3�η�

    for (int i = 0; i < 32; i++) //8������
    {
        z -= ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
        y -= ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        sum -= delta;
    }

    *firstChunk = y;
    *secondChunk = z;
}

//buffer������Ĵ���������buffer���ں�����ֱ�Ӷ�Ԫ����buffer���м��ܣ�size��buffer���ȣ�key����Կ��
void EncryptBuffer(uint8_t* buffer, int size, unsigned int* key)
{
    uint8_t *p = buffer;

    uint32_t leftSize = size;

    while (p < buffer + size &&
        leftSize >= sizeof(unsigned int) * 2)
    {
        EncryptTEA((unsigned int *)p, (unsigned int *)(p + sizeof(unsigned int)), key);
        p += sizeof(unsigned int) * 2;

        leftSize -= sizeof(unsigned int) * 2;
    }
}


//buffer������Ĵ���������buffer���ں�����ֱ�Ӷ�Ԫ����buffer���н��ܣ�size��buffer���ȣ�key����Կ��
void DecryptBuffer(uint8_t* buffer, int size, unsigned int* key)
{
    uint8_t *p = buffer;

    uint32_t leftSize = size;

    while (p < buffer + size &&
        leftSize >= sizeof(unsigned int) * 2)
    {
        DecryptTEA((unsigned int *)p, (unsigned int *)(p + sizeof(unsigned int)), key);
        p += sizeof(unsigned int) * 2;

        leftSize -= sizeof(unsigned int) * 2;
    }
}


