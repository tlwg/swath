//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#ifndef CONVFACT_INC
#define CONVFACT_INC

#include "convkit.h"

enum ETextFormat {
    TIS620,
    UTF8,
};

TextReader* CreateTextReader (ETextFormat format, const char* inText);
TextWriter* CreateTextWriter (ETextFormat format, char* outText, int outLen);

#endif // CONVFACT_INC

