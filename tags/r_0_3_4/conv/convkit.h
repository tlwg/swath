//
// convkit.h - Text Converter Kit
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef CONVKIT_INC
#define CONVKIT_INC

#include "unichar.h"

class TextReader {
public:
    virtual bool Read(unichar& c) = 0;
};

class TextWriter {
public:
    virtual bool Write(unichar c) = 0;
};

void TransferText(TextReader* pReader, TextWriter* pWriter);

#endif  // CONVKIT

