//
// utf8.cc - UTF-8 Text Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <theppitak@gmail.com>
//

#include "utf8.h"
#include "unichar.h"

bool
UTF8Writer::Write (unichar uniChar)
{
    if (uniChar <= 0x007F) {
        writeChar ((unsigned char)(uniChar & 0x7F));
    } else if (uniChar <= 0x07FF) {
        if (spaceLeft() < 2)  return false;
        writeChar ((unsigned char)(0xC0 | (uniChar >> 6)));
        writeChar ((unsigned char)(0x80 | (uniChar & 0x3F)));
    } else if (uniChar <= 0xFFFF) {
        if (spaceLeft() < 3)  return false;
        writeChar ((unsigned char)(0xE0 | (uniChar >> 12)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 6) & 0x3F)));
        writeChar ((unsigned char)(0x80 | (uniChar & 0x3F)));
    } else if (uniChar <= 0x1FFFFF) {
        if (spaceLeft() < 4)  return false;
        writeChar ((unsigned char)(0xF0 | (uniChar >> 18)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 12) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 6) & 0x3F)));
        writeChar ((unsigned char)(0x80 | (uniChar & 0x3F)));
    } else if (uniChar <= 0x3FFFFFF) {
        if (spaceLeft() < 5)  return false;
        writeChar ((unsigned char)(0xF8 | (uniChar >> 24)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 18) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 12) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 6) & 0x3F)));
        writeChar ((unsigned char)(0x80 | (uniChar & 0x3F)));
    } else if (uniChar <= 0x7FFFFFFF) {
        if (spaceLeft() < 6)  return false;
        writeChar ((unsigned char)(0xFC | (uniChar >> 30)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 24) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 18) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 12) & 0x3F)));
        writeChar ((unsigned char)(0x80 | ((uniChar >> 6) & 0x3F)));
        writeChar ((unsigned char)(0x80 | (uniChar & 0x3F)));
    } else {
        // error
        return false;
    }
    return true;
}

bool
UTF8Reader::Read (unichar& uniChar)
{
    unsigned char c = getChar();
    if (!c) return false;

    if ((c & 0x80) == 0x00) {
        uniChar = c;
    } else {
        // count rest bytes
        unsigned char sig = (c << 1);
        int nBytes = 0;
        while (sig & 0x80) { nBytes++; sig <<= 1; }
        if (nBytes == 0) return false;  // undefined signature

        // get most significant bits of unicode data
        //    signature bits = nBytes+2 (MSB)
        // -> data bits = 8 - (nBytes+2) = 6 - nBytes (LSB)
        uniChar = (c & (0x3F >> nBytes));

        // get rest bits
        while (nBytes-- > 0) {
            c = getChar();
            if (!c) return false;
            c ^= 0x80;  // 10xx xxxx -> 00xx xxxx
            if (c & 0xC0) return false;  // not 10xx xxxx form
            uniChar = (uniChar << 6) | c;
        }
    }

    return true;
}

int
UTF8Bytes (unichar uc)
{
    if (uc <= 0x007F) {
        return 1;
    } else if (uc <= 0x07FF) {
        return 2;
    } else if (uc <= 0xFFFF) {
        return 3;
    } else if (uc <= 0x1FFFFF) {
        return 4;
    } else if (uc <= 0x3FFFFFF) {
        return 5;
    } else if (uc <= 0x7FFFFFFF) {
        return 6;
    } else {
        // error
        return -1;
    }
}

