//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef CONVFACT_INC
#define CONVFACT_INC

#include <iostream.h>
#include "convkit.h"

enum ETextFormat {
    TIS620,
    UTF8,
};

TextReader* CreateTextReader(ETextFormat format, istream& input);
TextWriter* CreateTextWriter(ETextFormat format, ostream& output);

#endif // CONVFACT_INC

