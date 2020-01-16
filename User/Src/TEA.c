#include "main.h"
#include "stm32f1xx_hal.h"
#include "TEA.h"

/*************************************************TEA加密解密函数************************************************/

void EncryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key)
{
    unsigned int y = *firstChunk;
    unsigned int z = *secondChunk;
    unsigned int sum = 0;

    unsigned int delta = 0x9e3779b9;

    for (int i = 0; i < 32; i++)//8轮运算(需要对应下面的解密核心函数的轮数一样)
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

    sum = delta << 5; //32轮运算，所以是2的5次方；16轮运算，所以是2的4次方；8轮运算，所以是2的3次方

    for (int i = 0; i < 32; i++) //8轮运算
    {
        z -= ((y << 4) + key[2]) ^ (y + sum) ^ ((y >> 5) + key[3]);
        y -= ((z << 4) + key[0]) ^ (z + sum) ^ ((z >> 5) + key[1]);
        sum -= delta;
    }

    *firstChunk = y;
    *secondChunk = z;
}

//buffer：输入的待加密数据buffer，在函数中直接对元数据buffer进行加密；size：buffer长度；key是密钥；
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


//buffer：输入的待解密数据buffer，在函数中直接对元数据buffer进行解密；size：buffer长度；key是密钥；
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


