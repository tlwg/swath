//
// utf8.h - UTF-8 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef UTF8_INC
#define UTF8_INC

#include <iostream>
#include "convkit.h"

class UTF8Reader : public TextReader {
public:
    UTF8Reader(std::istream& input) : input(input) {}

    bool Read(unichar& c);

private:
    std::istream& input;
};

class UTF8Writer : public TextWriter {
public:
    UTF8Writer(std::ostream& output) : output(output) {}

    bool Write(unichar c);

private:
    std::ostream& output;
};

#endif  // UTF8_INC

