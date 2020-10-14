#ifndef _HEAD_LZSTRING_H_
#define _HEAD_LZSTRING_H_

/************************************************************************
implementation of LZW (Lempel–Ziv–Welch) compression algorithm based on the
following repository:
https://github.com/pieroxy/lz-string 
(author website: http://pieroxy.net/blog/pages/lz-string/index.html)

https://github.com/jawa-the-hutt/lz-string-csharp

LZW algorithm:
https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

/**
    inner data structure for compression  
    each characters are encoded/decoded as Unicode, so wide chars are used here (wstring)
 */
struct ContextCompressData {
    wstring str;
    int val, pos;
    ContextCompressData():str(L""), val(0), pos(0){}
    ContextCompressData(wstring &s_, int v_, int p_):str(s_), val(v_), pos(p_){}
};

/**
    inner data structure; to encode original data (string)
 */

struct ContextCompress {
    unordered_map<wstring, int> dict;
    unordered_map<wstring, bool> dict_to_create; 
    ContextCompressData data;
    wstring Wc, W;
    int enlarge_in, dict_size, num_bits;
    ContextCompress():Wc(L""), W(L""),enlarge_in(0), dict_size(0), num_bits(0){}
    ContextCompress(wstring wc_, wstring w_, int en_, int ds_, int nb_):
    Wc(wc_), W(w_),enlarge_in(en_), dict_size(ds_), num_bits(nb_){}
};

/**
    inner data structure for decompression
*/
struct DecompressData {
    wstring str;
    int val, pos, indx;
    DecompressData():str(L""), val(0), pos(0), indx(0){}
    DecompressData(wstring &s_, int v_, int p_, int i_):str(s_), val(v_), pos(p_), indx(i_){}
};

/**
    class LZString; wrapped APIs for outer calls;
    each method are provided with 2 APIs: string and wstring;
    TODO:
        implement CompressToEncodedURIComponent/DecompressFromEncodedURIComponent
*/
class LZString {
public:
    static wstring  Compress(wstring &uncompressed);
    static wstring  Compress(string &uncompressed);
    static wstring  Decompress(wstring &compressed);
    static wstring  Decompress(string &compressed);
    static wstring  CompressToUTF16(wstring &uncompressed);
    static wstring  CompressToUTF16(string &uncompressed);
    static wstring  DecompressFromUTF16(wstring& compressed);
    static wstring  DecompressFromUTF16(string& compressed);
    static wstring  CompressToBase64(wstring &uncompressed);
    static wstring  CompressToBase64(string &uncompressed);
    static wstring  DecompressFromBase64(wstring &compressed);
    static wstring  DecompressFromBase64(string &compressed);
    //static wstring  CompressToEncodedURIComponent(wstring &uncompressed);
    //static wstring  CompressToEncodedURIComponent(string &uncompressed);
    //static wstring  DecompressFromEncodedURIComponent(wstring &compressed);
    //static wstring  DecompressFromEncodedURIComponent(string &compressed);

    static wstring  ToWStr(const string& str);
    static string   ToStr(const wstring &wstr);

private:
    static void write_bit(int val, ContextCompressData &data);
    static void write_bits(int numbits, int val, ContextCompressData &data);
    static int  read_bit(DecompressData &data);
    static int  read_bits(int numbits, DecompressData &data);
    static void produceW(ContextCompress &context);
    static void decrement_engarge_in(ContextCompress &context);
    static void encode_utf16_char(wstring &rst, int &cur, int c, int i);
    static void decode_utf16_char(wstring &rst, int &cur, int c, int i);
private:
    static const wstring key_str; /**< string used for base64 */
};

#endif 