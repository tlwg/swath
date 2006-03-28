//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef CONVFACT_INC
#define CONVFACT_INC

#include <iostream>
#include "convkit.h"

enum ETextFormat {
    TIS620,
    UTF8,
};

TextReader* CreateTextReader(ETextFormat format, std::istream& input);
TextWriter* CreateTextWriter(ETextFormat format, std::ostream& output);

#endif // CONVFACT_INC

