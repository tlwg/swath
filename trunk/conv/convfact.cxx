//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include "convfact.h"
#include "utf8.h"
#include "tis620.h"

TextReader* CreateTextReader(ETextFormat format, istream& input)
{
    switch (format) {
        case TIS620:  return new TIS620Reader(input);
        case UTF8:    return new UTF8Reader(input);
        default:      return 0;
    }
}

TextWriter* CreateTextWriter(ETextFormat format, ostream& output)
{
    switch (format) {
        case TIS620:  return new TIS620Writer(output);
        case UTF8:    return new UTF8Writer(output);
        default:      return 0;
    }
}

