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

public:
    TextReader (const char* inText)
        : inText (inText) {}

    const char* curPos () const;

protected:
    char getChar ();

private:
    const char* inText;
};

inline char
TextReader::getChar ()
{
    char c = *inText;
    if (c)
        ++inText;
    return c;
}

inline const char*
TextReader::curPos () const
{
  return inText;
}

class TextWriter {
public:
    virtual bool Write(unichar c) = 0;

public:
    TextWriter (char *outText, int outLen)
        : outText (outText), outLen (outLen) {}

protected:
    bool writeChar (char c);
    int  spaceLeft () const;

private:
    char* outText;
    int   outLen;
};

inline bool
TextWriter::writeChar (char c)
{
    if (outLen > 0) {
        *outText++ = c;
        --outLen;
        return true;
    }
    return false;
}

inline int
TextWriter::spaceLeft () const
{
    return outLen;
}

#endif  // CONVKIT

