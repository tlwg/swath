//
// dataheap.cpp - variable-sized data heap on VirtualMem
// Created: 31 Jul 1996 (splitted from dict.cpp)
// Author:  Theppitak Karoonboonyanan
//

#include <string.h>
#include "vmem/dataheap.h"

#include <iostream>

using namespace std;

//////////////// Implementation of DataHeap class ////////////////

//////////////// Implementation of HeapAssociated class ////////////////
void HeapAssociated::Create(int size)
{
    Delete();
    entry = dataHeap.NewBlock(size);
}

void HeapAssociated::Delete()
{
    if (entry != 0) {
        dataHeap.DeleteBlock(entry);
        entry = 0;
    }
}

//////////////// Implementation of CharBlock class ////////////////

// size precalculation (static function)
int CharBlock::PrecalcSize(const char* str)
{
    return strlen(str) + 1;
}


// Default c-tor
CharBlock::CharBlock(DataHeap& aHeap)
: HeapAssociated(aHeap)
{
}

// Creating c-tor
CharBlock::CharBlock(DataHeap& aHeap, const char* str)
: HeapAssociated(aHeap)
{
    Create(PrecalcSize(str));
    SetString(str);
}

// Associating c-tor
CharBlock::CharBlock(DataHeap& aHeap, Pointer aEntry)
: HeapAssociated(aHeap, aEntry)
{
}

int CharBlock::BlockSize()
{
    ASSERT(GetEntryPtr() != 0);
    return int(GetDataHeap().Byte(GetEntryPtr())) + 1;
}

const char* CharBlock::GetString(char buff[], int buffSize)
{
    Pointer entry = GetEntryPtr();
    ASSERT(entry != 0);

    int len = int(GetDataHeap().Byte(entry));
    if (buffSize > len+2) {
        buffSize = len+2; // 1 length byte + len chars + '\0'
    }

    GetDataHeap().BlockRead(entry, buff, buffSize-1);
    buff[buffSize-1] = '\0';

    // eliminate the leading length byte
    memmove(buff, buff+1, buffSize-1);

    return buff;
}

void CharBlock::SetString(const char* str)
{
    ASSERT(GetEntryPtr() != 0);

    int    len = strlen(str);
    uint8* buffer = new uint8[len+1];

    // prepare data block
    buffer[0] = uint8(len);               // first byte is string length,
    strncpy((char*)(buffer+1), str, len); // then the string itself

    // write it down to virtual mem
    GetDataHeap().BlockWrite(GetEntryPtr(), (char*)buffer, len+1);

    delete buffer;
}
