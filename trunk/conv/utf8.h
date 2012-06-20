//
// utf8.h - UTF-8 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef UTF8_INC
#define UTF8_INC

#include <stdio.h>
#include "convkit.h"

class UTF8Reader : public TextReader {
public:
    UTF8Reader(const char* inText)
        : TextReader (inText) {}

    bool Read(unichar& c);
};

class UTF8Writer : public TextWriter {
public:
    UTF8Writer(char* outText, int outLen)
        : TextWriter (outText, outLen) {}

    bool Write(unichar c);
};

#endif  // UTF8_INC

