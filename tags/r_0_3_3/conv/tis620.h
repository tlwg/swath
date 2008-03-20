//
// tis620.h - TIS-620 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef TIS620_INC
#define TIS620_INC

#include <iostream>
#include "convkit.h"

class TIS620Reader : public TextReader {
public:
    TIS620Reader(std::istream& input) : input(input) {}

    bool Read(unichar& c);

private:
    std::istream& input;
};

class TIS620Writer : public TextWriter {
public:
    TIS620Writer(std::ostream& output) : output(output) {}

    bool Write(unichar c);

private:
    std::ostream& output;
};

#endif // TIS620_INC

