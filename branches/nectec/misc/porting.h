//
// porting.h - definition of classes for code porting
// Created: 12 Jul 1996
// Author:  Theppitak Karoonboonyanan
//

#ifndef PORTING_INC
#define PORTING_INC

#include "misc/typedefs.h"

//
// IsHighEndianSystem() -  indicates if the running system is using
//    high-endian storage method,that is, high byte of an integer is
//    stored at low address.
//
// For example, an 2-byte integer of value 0xff00 will be stored in:
//    Low-Endian:  00 FF
//    High-Endian: FF 00
//
bool IsHighEndianSystem();

// Convertors between Low-Endian and High-Endian systems
uint16 ReverseBytes16(uint16 n);
uint32 ReverseBytes32(uint32 n);

inline uint16 ReverseBytes16(uint16 n)  { return (n<<8) | (n>>8); }

#endif  // PORTING_INC
