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
    UTF8Reader(FILE* input) : input(input) {}

    bool Read(unichar& c);

private:
    FILE* input;
};

class UTF8Writer : public TextWriter {
public:
    UTF8Writer(FILE* output) : output(output) {}

    bool Write(unichar c);

private:
    FILE* output;
};

#endif  // UTF8_INC

