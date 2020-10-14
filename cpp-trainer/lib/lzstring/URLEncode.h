/*****************************************************************************
Module :     URLEncode.H
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: H URL Encoder
*****************************************************************************/
#ifndef __CURLENCODE_H_
#define __CURLENCODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

using namespace std;

class CURLEncode {
private:
    static string csUnsafeString;
    string decToHex(char num, int radix);
    char hexToCh(char val[]);
    bool isUnsafe(char compareChar);
    string convert(char val);
    string unconvert(char val[]);

public:
    CURLEncode() { };
    virtual ~CURLEncode() { };
    string URLEncode(const string & vData);
    string URLDecode(const string & vData);
};

#endif //__CURLENCODE_H_