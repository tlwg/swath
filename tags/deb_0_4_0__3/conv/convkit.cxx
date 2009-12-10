//
// convkit.cc - Text Converter Kit
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include "convkit.h"

void TransferText(TextReader* pReader, TextWriter* pWriter)
{
    unichar c;
    while (pReader->Read(c)) {
        pWriter->Write(c);
    }
}
 
