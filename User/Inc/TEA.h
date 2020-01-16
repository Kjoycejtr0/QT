#ifndef __TEA_H
#define __TEA_H
#include "main.h"

void EncryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key);
void DecryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key);
void EncryptBuffer(uint8_t* buffer, int size, unsigned int* key);
void DecryptBuffer(uint8_t* buffer, int size, unsigned int* key);
u32 DecrypFromForward(u8 *buff,u32 size);

#endif
