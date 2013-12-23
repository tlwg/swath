//
// tis620.cc - TIS-620 Agents
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include "tis620.h"
#include "tischar.h"

static unichar tis2unicode(tischar c)
{
    if (c < 0x80) {
        return c;
    } else if (
        (0xA1 <= c && c <= 0xDA) ||
        (0xDF <= c && c <= 0xFB)
    ) {
        return 0x0E00 + c - 0xA0;
    } else {
        return BAD_WCHAR;
    }
}

static tischar unicode2tis(unichar u)
{
    if (u < 0x0080) {
        return u;
    } else if (
        (0x0E01 <= u && u <= 0x0E3A) ||
        (0x0E3F <= u && u <= 0x0E5B)
    ) {
        return 0xA0 + u - 0x0E00;
    } else {
        return BAD_TISCHAR;
    }
}

bool TIS620Reader::Read(unichar& c)
{
    tischar t = tischar (getChar());
    if (!t) return false;
    c = tis2unicode (t);
    return true;
}

bool TIS620Writer::Write(unichar c)
{
    return writeChar (unicode2tis (c));
}

