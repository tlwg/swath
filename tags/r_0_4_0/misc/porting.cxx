//
// porting.cpp - classes for code porting
// Created: 12 Jul 1996
// Author:  Theppitak Karoonboonyanan
//

#include "misc/porting.h"

bool IsHighEndianSystem()
{
    static uint16 tester = 0xff00U;
    return *(unsigned char*)&tester == 0xff;
}

uint32 ReverseBytes32(uint32 n)
{
    uint16* p = (uint16*)&n;
    *p = ReverseBytes16(*p);  p++;
    *p = ReverseBytes16(*p);
    return (n>>16) | (n<<16);
}

#ifdef PORTING_TEST

#include <iostream.h>

int main()
{
    if (IsHighEndianSystem()) {
        cout << "High-Endian!" << endl;
    } else {
        cout << "Low-Endian!" << endl;
    }

    uint16 i16 = 0xdeaf;
    cout << "ReverseBytes(" << hex << i16 << ") = ";
    cout << hex << ReverseBytes16(i16) << endl;

    uint32 i32 = 0xdeafcafe;
    cout << "ReverseBytes(" << hex << i32 << ") = ";
    cout << hex << ReverseBytes32(i32) << endl;

    return 0;
}

#endif // PORTING_TEST
