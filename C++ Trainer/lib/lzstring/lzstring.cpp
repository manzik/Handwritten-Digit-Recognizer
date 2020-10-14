// LZString.cpp : Defines the entry point for the console application.
//

#include "lzstring.h"

const wstring LZString::key_str = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

long ipow(double x, int n) {
    if (!n) return 1;
    if (n < 0) {
        n = -n, x = 1.0/x;
    }
    long r = ipow(x, n/2);
    return (long)(n&1 ? r*r*x:r*r);
}

inline wstring tostr(int val) {
    wstring str; str = wchar_t(val); return str;
}

inline void LZString::write_bit(int val, ContextCompressData &data) {
    data.val = (data.val << 1) | val;
    if (data.pos == 15) {
        data.pos = 0; data.str += wchar_t(data.val); data.val = 0;
    } else { ++data.pos; }
}

inline void LZString::write_bits(int numbits, int val, ContextCompressData &data) {
    for (int i = 0; i < numbits; ++i) {
        write_bit(val&1, data); 
        val >>= 1;
    }
}

int LZString::read_bit(DecompressData &data) {
    int rst = data.val & data.pos; 
    if ((data.pos >>= 1) == 0) {
        data.pos = 32768;
        if (data.indx < (int)data.str.length()) {
            data.val = data.str[data.indx++];
        }
    }
    return rst > 0 ? 1:0;
}

int LZString::read_bits(int numbits, DecompressData &data) {
    int rst = 0, maxp = ipow(2, numbits), p = 1;
    while (p != maxp) {
        rst |= read_bit(data)*p; p <<= 1;
    }
    return rst;
}

void LZString::produceW(ContextCompress &context) {
    /*< the index for W without C is retrieved and sent out for output */
    if (context.dict_to_create.find(context.W) != context.dict_to_create.end()) {
        if (context.W[0] < 256) {
            write_bits(context.num_bits, 0, context.data);
            write_bits(8, context.W[0], context.data);
        } else {
            write_bits(context.num_bits, 1, context.data);
            write_bits(16, context.W[0], context.data);
        }
        decrement_engarge_in(context);
        context.dict_to_create.erase(context.W);
    } else {
        write_bits(context.num_bits, context.dict[context.W], context.data);
    }
}

inline void LZString::decrement_engarge_in(ContextCompress &context) {
    if (--context.enlarge_in == 0) {
        context.enlarge_in = ipow(2, context.num_bits++);
    }
}

wstring LZString::ToWStr(const std::string& str) {
    if (str.empty()) return L"";
    try {
        wchar_t *wptr = new wchar_t[str.size()+1];
        size_t asize = mbstowcs(wptr, str.c_str(), (str.size()+1)*sizeof(wchar_t));
        wstring wstr(wptr);
        delete [] wptr; wptr = nullptr;
        return wstr;
    }catch(...){
        return L"";
    }
}

string LZString::ToStr(const std::wstring& wstr){
    if (wstr.empty()) return "";
    try {
        //setlocale(LC_ALL,".ACP");
        const wchar_t *wptr = wstr.c_str();
        size_t msize = (wstr.size()+1)*sizeof(wchar_t)/sizeof(char);
        char *ptr = new char[msize];
        int asize = wcstombs(ptr, wstr.c_str(), msize);
        string str(ptr);
        delete[] ptr;
        return str;
        //string str(wstr.size(), 0);
        //copy(wstr.begin(), wstr.end(), str.begin());
        //return str;
    } catch(...) {
        return "";
    }
}

wstring LZString::Compress(wstring &uncompressed) {
    ContextCompress context(L"", L"", 2, 3, 2);

    wstring ch;
    for (int i = 0, len = uncompressed.length(); i < len; ++i) {
        ch = uncompressed[i];
        /*< initialize the dictionary to contain all strings with length 1*/
        if (context.dict.find(ch) == context.dict.end()) {
            context.dict[ch] = context.dict_size++;
            context.dict_to_create[ch] = true;
        }
        /*< find the longest string W */
        context.Wc = context.W + ch;
        if (context.dict.find(context.Wc) != context.dict.end()) {
            context.W = context.Wc;
        } else {
            produceW(context); 
            decrement_engarge_in(context);
            /*< add substring with the new char to dictionary */
            context.dict[context.Wc] = context.dict_size++;
            /*< reset pointer as the new char for next scan */
            context.W = ch;
        }
    }
    if (!context.W.empty()) produceW(context);
    // mark the end of the stream
    write_bits(context.num_bits, 2, context.data);
    // flush the last char
    while (true) {
        context.data.val <<= 1;
        if (context.data.pos == 15) {
            context.data.str += wchar_t(context.data.val);
            break;
        } else { ++context.data.pos; }
    }
    return context.data.str;
}

wstring LZString::Compress(string &uncompressed) {
    wstring wstr = LZString::ToWStr(uncompressed);
    return LZString::Compress(wstr);
}

wstring LZString::Decompress(wstring &compressed) {
    if (compressed.empty()) return L"";

    DecompressData data(compressed, compressed[0], 32768, 1);
    vector<wstring> dictionary; dictionary.reserve(compressed.size());
    wstring entry, w, sc, rst;
    int next = 0, enlarge_in = 4, numbits = 3, c = 0, err_cnt = 0;
    for (int i = 0; i < 3; ++i) dictionary.push_back(tostr(i));
    next = read_bits(2, data);
    switch(next) {
    case 0: sc = wchar_t(read_bits(8, data)); break;
    case 1: sc = wchar_t(read_bits(16, data)); break;
    case 2: return L"";
    }
    dictionary.push_back(sc);
    w = sc, rst += sc;

    while (true) {
        c = read_bits(numbits, data);
        int cc = c;
        switch(cc) {
        case 0:
        case 1:
            if (!cc && ++err_cnt > 10000) return L""; /*! too many errors */
            sc = wchar_t(read_bits(1<<(cc+3), data));
            dictionary.push_back(sc);
            c = dictionary.size()-1; --enlarge_in;
            break;
        case 2: return rst;
        }

        if (!enlarge_in) enlarge_in = ipow(2, numbits++);
        if ((int)dictionary.size() > c && !dictionary[c].empty()) {
            entry = dictionary[c];
        }
        else {
            if (c == dictionary.size()) entry = w + wchar_t(w[0]);
            else return L"";
        }

        rst += entry;
        dictionary.push_back(w + wchar_t(entry[0]));
        w = entry; 
        if (!--enlarge_in) enlarge_in = ipow(2, numbits++);
    }
}

wstring LZString::Decompress(string &compressed) {
    wstring wstr = LZString::ToWStr(compressed);
    return LZString::Decompress(wstr);
}

inline void LZString::encode_utf16_char(wstring &rst, int &cur, int c, int i) {
    rst += (i != 14 ? tostr((i?cur:0)+(c>>(i+1))+32) : 
                      tostr((i?cur:0)+(c>>(i+1))+32) + tostr((c&32767)+32)); 
    if(i!=14) cur = (c&((1<<(i+1))-1)) << (14-i);
}

wstring LZString::CompressToUTF16(wstring &uncompressed) {
    if (uncompressed.empty()) return uncompressed;

    wstring rst = L"", input = Compress(uncompressed);
    if (input.empty()) return input;

    int status = 0, current = 0;
    for (int i = 0, len = input.length(); i < len; ++i, ++status %= 15) {
        encode_utf16_char(rst, current, input[i], status);
    }
    rst += wchar_t(current + 32);
    return rst;
}

wstring LZString::CompressToUTF16(string &uncompressed) {
    wstring wstr = LZString::ToWStr(uncompressed);
    return LZString::CompressToUTF16(wstr);
}

inline void LZString::decode_utf16_char(wstring &rst, int &cur, int c, int i) {
    int k = 15-i;
    if (i) rst += wchar_t(cur | (c >> k));
    if (i != 15) cur = !i ? c << 1 : (c& ((1<<k)-1)) << (i+1);
}

wstring LZString::DecompressFromUTF16(wstring &compressed) {
    wstring rst = L"";
    if (compressed.empty()) return rst;

    int status = 0, current = 0;
    for (int i = 0, len = compressed.length(); i < len; ++i, ++status %= 16) {
        decode_utf16_char(rst, current, compressed[i] - 32, status);
    }
    return Decompress(rst);
}

wstring LZString::DecompressFromUTF16(string& compressed) {
    wstring wstr = LZString::ToWStr(compressed);
    return LZString::DecompressFromUTF16(wstr);
}

wstring LZString::CompressToBase64(wstring &uncompressed) {
    wstring rst = L"", compress = L"";
    if (uncompressed.empty()) return rst;

    compress = Compress(uncompressed);
    if (compress.empty()) return rst;

    int ch[3] = {0}, enc[4] = {0};
    for (int i = 0, sz = compress.size(); i < sz*2; i += 3) {
        if (i&1) {
            ch[0] = compress[(i-1)>>1] & 255;
            int k = (i+1) >> 1;
            if (k < sz) {
                ch[1] = compress[k] >> 8; ch[2] = compress[k] & 255;
            } else {
                ch[1] = ch[2] = INT_MIN;
            }
        } else {
            int k = i>>1;
            ch[0] = compress[k] >> 8;
            ch[1] = compress[k] & 255;
            ch[2] = (k+1 < sz ? compress[k+1] >> 8 : INT_MIN);
        }
        
        memset(enc, 0, sizeof(int)*4);
        enc[0] = ch[0] >> 2;

        if (ch[1] != INT_MIN) {
            enc[1] = ((ch[0]&3) << 4) | (ch[1] >> 4);
        }
        enc[2] = ch[1] != INT_MIN && ch[2] != INT_MIN ? (ch[1]&15)<<2 | (ch[2])>>6 : 0;
        
        if (ch[2] != INT_MIN) {
            enc[3] = ch[2]&63;
        }

        if (ch[1] == INT_MIN) {
            enc[2] = enc[3] = 64;
        } else if (ch[2] == INT_MIN) {
            enc[3] = 64;
        }

        rst += key_str[enc[0]]; rst += key_str[enc[1]];
        rst += key_str[enc[2]]; rst += key_str[enc[3]];
    }
    return rst;
}

wstring LZString::CompressToBase64(string &uncompressed) {
    wstring wstr = LZString::ToWStr(uncompressed);
    return LZString::CompressToBase64(wstr);
}

wstring LZString::DecompressFromBase64(wstring &compressed) {
    wstring rst = L"";
    if (compressed.empty()) return rst;

    int key_index[256] = {0};
    for(int i = 0; i < key_str.size(); ++i) key_index[key_str[i]] = i;

    int out = 0, out_len = 0, ch[3] = {0}, enc[4] = {0};
    for (int i = 0, sz = compressed.size(); i < sz; ) {
        int c = compressed[i];
        if (!(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' ||
            c == '-' || c == '+' || c == '/' || c == '=')) {++i; continue;}
        
        for (int k = 0; k < 4; ++k, ++i) enc[k] = key_index[int(compressed[i])];
        ch[0] = (enc[0] << 2) | (enc[1] >> 4);
        ch[1] = ((enc[1] & 15) << 4) | (enc[2] >> 2);
        ch[2] = ((enc[2] & 3) << 6)  | enc[3];

        if (out_len&1) {
            rst += wchar_t(out | ch[0]);
            if (enc[2] != 64) out = ch[1] << 8;
            if (enc[3] != 64) rst += wchar_t(out | ch[2]);
        } else {
            out = ch[0] << 8;
            if (enc[2] != 64) rst += wchar_t(out | ch[1]);
            if (enc[3] != 64) out = ch[2] << 8;
        }
        out_len += 3;
    }
    return Decompress(rst);
}

wstring LZString::DecompressFromBase64(string &compressed) {
    wstring wstr = LZString::ToWStr(compressed);
    return LZString::DecompressFromBase64(wstr);
}

//wstring LZString::CompressToEncodedURIComponent(wstring &uncompressed) {
//
//}
//
//wstring LZString::CompressToEncodedURIComponent(string &uncompressed) {
//
//}
//
//wstring LZString::DecompressFromEncodedURIComponent(wstring &compressed) {
//
//}
//
//wstring LZString::DecompressFromEncodedURIComponent(string &compressed) {
//
//}