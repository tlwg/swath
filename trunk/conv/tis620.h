//
// tis620.h - TIS-620 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef TIS620_INC
#define TIS620_INC

#include <stdio.h>
#include "convkit.h"

class TIS620Reader : public TextReader {
public:
    TIS620Reader(FILE* input) : input(input) {}

    bool Read(unichar& c);

private:
    FILE* input;
};

class TIS620Writer : public TextWriter {
public:
    TIS620Writer(FILE* output) : output(output) {}

    bool Write(unichar c);

private:
    FILE* output;
};

#endif // TIS620_INC

