//
// convfact.h - Text Agent Factory
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <theppitak@gmail.com>
//

#ifndef __CONVFACT_H
#define __CONVFACT_H

#include "convkit.h"

enum ETextFormat {
    TIS620,
    UTF8,
};

TextReader* CreateTextReader (ETextFormat format, const char* inText);
TextWriter* CreateTextWriter (ETextFormat format, char* outText, int outLen);

#endif // __CONVFACT_H

