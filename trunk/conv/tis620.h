//
// tis620.h - TIS-620 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef TIS620_INC
#define TIS620_INC

#include <iostream.h>
#include "convkit.h"

class TIS620Reader : public TextReader {
public:
    TIS620Reader(istream& input) : input(input) {}

    bool Read(unichar& c);

private:
    istream& input;
};

class TIS620Writer : public TextWriter {
public:
    TIS620Writer(ostream& output) : output(output) {}

    bool Write(unichar c);

private:
    ostream& output;
};

#endif // TIS620_INC

