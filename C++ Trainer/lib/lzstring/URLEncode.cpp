/*****************************************************************************
Module :     URLEncode.cpp
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: CPP URL Encoder
*****************************************************************************/
//#define _CRTDBG_MAP_ALLOC


#include "URLEncode.h"

#define MAX_BUFFER_SIZE 4096

// HEX Values array
char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

static const int tableHexToDec[] = {-1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, 
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, 
    -1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15};

// UNSAFE String
string CURLEncode::csUnsafeString= "\"<>%\\^[]`+$,@:;/!#?=&";

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
string CURLEncode::convert(char val) 
{
    string csRet;
    csRet += "%";
    csRet += decToHex(val, 16);	
    return  csRet;
}
string CURLEncode::unconvert(char val[]) {
    string csRet;
    csRet = hexToCh(val);
    return csRet;
}

char CURLEncode::hexToCh(char val[]) {
    int ch1 = tableHexToDec[val[0]];
    int ch0 = tableHexToDec[val[1]];
    char ch =0;

    ch = ch0 + (16 * ch1);
    if (-1 == ch1 || -1 == ch0)
        ch = 0;
    return ch;
}

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
string CURLEncode::decToHex(char num, int radix)
{	
    int temp=0;	
    string csTmp;
    int num_char;
    num_char = (int) num;

    // ISO-8859-1 
    // IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
    // PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
    if (num_char < 0)
        num_char = 256 + num_char;

    while (num_char >= radix)
    {
        temp = num_char % radix;
        num_char = (int)floor((double)num_char / radix);
        csTmp = hexVals[temp];
    }

    csTmp += hexVals[num_char];

    if(csTmp.size() < 2)
    {
        csTmp += '0';
    }

    string strdecToHex(csTmp);
    // Reverse the String
    //strdecToHex.MakeReverse(); 
    reverse(strdecToHex.begin(), strdecToHex.end());

    return strdecToHex;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
bool CURLEncode::isUnsafe(char compareChar)
{
    bool bcharfound = false;
    char tmpsafeChar;
    int m_strLen = 0;

    m_strLen = csUnsafeString.size();
    for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
    {
        tmpsafeChar = csUnsafeString[ichar_pos]; 
        if(tmpsafeChar == compareChar)
        { 
            bcharfound = true;
            break;
        } 
    }
    int char_ascii_value = 0;
    //char_ascii_value = __toascii(compareChar);
    char_ascii_value = (int) compareChar;

    if(bcharfound == false &&  char_ascii_value > 32 && char_ascii_value < 123)
    {
        return false;
    }
    // found no unsafe chars, return false		
    else
    {
        return true;
    }

    return true;
}
// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
// TO URL ENCODE FORM.
string CURLEncode::URLEncode(const string & pcsEncode)
{	
    int ichar_pos;
    string csEncode;
    string csEncoded;	
    int m_length;
    //	int ascii_value;

    csEncode = pcsEncode;
    m_length = csEncode.size();

    for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
    {
        char ch = csEncode[ichar_pos];
        if (ch < ' ') 
        {
            ch = ch;
        }		
        if(!isUnsafe(ch))
        {
            // Safe Character				
            csEncoded += ch;
        }
        else
        {
            // get Hex Value of the Character
            csEncoded += convert(ch);
        }
    }


    return csEncoded;

}
string CURLEncode::URLDecode(const string & pcsDecode) {
    int ichar_pos;
    string csDecode;
    string csDecoded;	
    int m_length;
    //	int ascii_value;

    csDecode = pcsDecode;
    m_length = csDecode.size();

    for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
    {
        char ch = csDecode[ichar_pos];
        if ('%' == ch) {
            char hex[2];
            hex[0] = csDecode[++ichar_pos];
            hex[1] = csDecode[++ichar_pos];
            csDecoded += unconvert(hex);
        } else {
            csDecoded += ch;
        }
    }

    return csDecoded;

}

//void TEST(CURLEncode,test)
//{
//    CURLEncode url;
//    string string("abc 123 !@#$%^&*()_+=\\|+_)(*&^%$#@!~`{}][';\":/.,<>?");
//    string += "\r\n\t";
//    string encoded = url.URLEncode(string);
//    string decoded = url.URLDecode(encoded);
//    //	logger.logd("URL         "+string);
//    //	logger.logd("URL encoded "+encoded);
//    //	logger.logd("URL decoded "+decoded);
//    CHECK(string == decoded);
//
//}


