//
// conv.cc - Main Program for text converter program
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "convfact.h"
#include "utf8.h"
#include "tis620.h"

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

int conv( char format, const char *inputFileName,const char *outputFileName)
{
    ETextFormat inputFormat = TIS620;
    ETextFormat outputFormat = UTF8;
    istream*    pInputFile = &cin;
    ostream*    pOutputFile = &cout;

	if (format=='t') {
		inputFormat = UTF8;
		outputFormat = TIS620;
	}else{
		inputFormat = TIS620;
		outputFormat = UTF8;
	}

    if (inputFileName) {
        pInputFile = new ifstream(inputFileName);
    }
    if (outputFileName) {
        pOutputFile = new ofstream(outputFileName);
    }

    TextReader* pReader = CreateTextReader(inputFormat, *pInputFile);
    TextWriter* pWriter = CreateTextWriter(outputFormat, *pOutputFile);

    if (pReader && pWriter) {
        TransferText(pReader, pWriter);
    }

    delete pReader;
    delete pWriter;
    if (inputFileName)  { delete pInputFile; }
    if (outputFileName) { delete pOutputFile; }

    return 0;
}

