//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <theppitak@gmail.com>
//

#include "convfact.h"
#include "utf8.h"
#include "tis620.h"

TextReader*
CreateTextReader (ETextFormat format, const char* inText)
{
    switch (format) {
        case TIS620:  return new TIS620Reader (inText);
        case UTF8:    return new UTF8Reader (inText);
        default:      return 0;
    }
}

TextWriter*
CreateTextWriter (ETextFormat format, char* outText, int outLen)
{
    switch (format) {
        case TIS620:  return new TIS620Writer (outText, outLen);
        case UTF8:    return new UTF8Writer (outText, outLen);
        default:      return 0;
    }
}

