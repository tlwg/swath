//
// utf8.cc - UTF-8 Text Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include "utf8.h"
#include "unichar.h"

int writeUTF8(std::ostream& output, unsigned unicode)
{
    if (unicode <= 0x007F) {
        output.put((unsigned char)(unicode & 0x7F));
    } else if (unicode <= 0x07FF) {
        output.put((unsigned char)(0xC0 | (unicode >> 6)));
        output.put((unsigned char)(0x80 | (unicode & 0x3F)));
    } else if (unicode <= 0xFFFF) {
        output.put((unsigned char)(0xE0 | (unicode >> 12)));
        output.put((unsigned char)(0x80 | ((unicode >> 6) & 0x3F)));
        output.put((unsigned char)(0x80 | (unicode & 0x3F)));
    } else if (unicode <= 0x1FFFFF) {
        output.put((unsigned char)(0xF0 | (unicode >> 18)));
        output.put((unsigned char)(0x80 | ((unicode >> 12) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 6) & 0x3F)));
        output.put((unsigned char)(0x80 | (unicode & 0x3F)));
    } else if (unicode <= 0x3FFFFFF) {
        output.put((unsigned char)(0xF8 | (unicode >> 24)));
        output.put((unsigned char)(0x80 | ((unicode >> 18) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 12) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 6) & 0x3F)));
        output.put((unsigned char)(0x80 | (unicode & 0x3F)));
    } else if (unicode <= 0x7FFFFFFF) {
        output.put((unsigned char)(0xFC | (unicode >> 30)));
        output.put((unsigned char)(0x80 | ((unicode >> 24) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 18) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 12) & 0x3F)));
        output.put((unsigned char)(0x80 | ((unicode >> 6) & 0x3F)));
        output.put((unsigned char)(0x80 | (unicode & 0x3F)));
    } else {
        // error
        return -1;
    }
    return 0;
}

int readUTF8(std::istream& input, unsigned* pUnicode)
{
    unsigned char c;
    if (!input.get(reinterpret_cast<char&>(c))) { return -1; }

    if ((c & 0x80) == 0x00) {
        *pUnicode = c;
    } else {
        // count rest bytes
        unsigned char sig = (c << 1);
        int nBytes = 0;
        while (sig & 0x80) { nBytes++; sig <<= 1; }
        if (nBytes == 0) { return -1; }  // undefined signature

        // get most significant bits of unicode data
        //    signature bits = nBytes+2 (MSB)
        // -> data bits = 8 - (nBytes+2) = 6 - nBytes (LSB)
        *pUnicode = (c & (0x3F >> nBytes));

        // get rest bits
        while (nBytes-- > 0) {
            if (!input.get(reinterpret_cast<char&>(c))) { return -1; }
            c ^= 0x80;  // 10xx xxxx -> 00xx xxxx
            if (c & 0xC0) { return -1; }  // not 10xx xxxx form
            *pUnicode = (*pUnicode << 6) | c;
        }
    }

    return 0;
}

bool UTF8Reader::Read(unichar& c)
{
    return readUTF8(input, &c) == 0;
}

bool UTF8Writer::Write(unichar c)
{
    return writeUTF8(output, c) == 0;
}

