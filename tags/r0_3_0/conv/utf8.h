//
// utf8.h - UTF-8 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef UTF8_INC
#define UTF8_INC

#include <iostream.h>
#include "convkit.h"

class UTF8Reader : public TextReader {
public:
    UTF8Reader(istream& input) : input(input) {}

    bool Read(unichar& c);

private:
    istream& input;
};

class UTF8Writer : public TextWriter {
public:
    UTF8Writer(ostream& output) : output(output) {}

    bool Write(unichar c);

private:
    ostream& output;
};

#endif  // UTF8_INC

