//
// conv.cc - Main Program for text converter program
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <theppitak@gmail.com>
//

#include "convfact.h"

//
// SYNOPSIS
//    conv ([-t|-u], [source], [target]);
// DESCRIPTION
//    'conv' converts text data from the file specified by 'source' and
//    writes the result to 'target' file. If either 'source' or 'target'
//    is omitted, standard input and standard output will be assumed.
// OPTIONS
//    -t  Converts UTF-8 to TIS-620
//    -u  Converts TIS-620 to UTF-8 (default)
//

static inline ETextFormat
TextFormatFromChar (char f)
{
  return (f == 't') ? TIS620 : UTF8;
}

int
conv (char inFormat, char outFormat, const char *inText,
      char *outText, int outLen)
{
    ETextFormat inputFormat = TextFormatFromChar (inFormat);
    ETextFormat outputFormat = TextFormatFromChar (outFormat);

    TextReader* pReader = CreateTextReader (inputFormat, inText);
    TextWriter* pWriter = CreateTextWriter (outputFormat, outText, outLen);

    if (pReader && pWriter) {
        unichar c;
        while (pReader->Read (c)) {
            pWriter->Write (c);
        }
    }
    pWriter->Write (0);

    delete pReader;
    delete pWriter;

    return 0;
}

