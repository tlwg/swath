#ifndef __CONV_H
#define __CONV_H

// inFormat, outFormat: 't' = TIS-620; 'u' = UTF-8
int conv(char inFormat, char outFormat, const char *inText,
         char *outText, int outLen);

#endif  // __CONV_H

