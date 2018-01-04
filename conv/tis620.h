//
// tis620.h - TIS-620 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef TIS620_INC
#define TIS620_INC

#include "convkit.h"

class TIS620Reader : public TextReader {
public:
    TIS620Reader(const char*inText)
        : TextReader (inText) {}

    bool Read(unichar& c);
};

class TIS620Writer : public TextWriter {
public:
    TIS620Writer(char* outText, int outLen)
        : TextWriter (outText, outLen) {}

    bool Write(unichar c);
};

#endif // TIS620_INC

