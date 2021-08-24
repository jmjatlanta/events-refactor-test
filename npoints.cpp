#include <iostream>
#include <cstdint>
#include <cstring>
#include "tinyformat.h"

#define BITS 256

/* utilstrencodings.cpp */
const signed char p_util_hexdigit[256] =
{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,0xa,0xb,0xc,0xd,0xe,0xf,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };

signed char HexDigit(char c)
{
    return p_util_hexdigit[(unsigned char)c];
}

union _bits256 {

    uint8_t bytes[32]; uint16_t ushorts[16]; uint32_t uints[8]; uint64_t ulongs[4]; uint64_t txid;

    enum { WIDTH=BITS/8 };
    std::string ToString()
    {
        std::string res = "";
        for (int i=0; i<32; i++)
            res = res + strprintf("%02x", bytes[i]);
        return res;
    }
    std::string ToStringRev()
    {
        std::string res = "";
        for (int i=0; i<32; i++)
            res = strprintf("%02x", bytes[i]) + res;
        return res;
    }
    void SetNull() {
        for (int i=0; i<32; i++) bytes[i] = 0;
    }

    void SetHex(const char* psz)
    {
        memset(bytes, 0, sizeof(bytes));

        // skip leading spaces
        while (isspace(*psz))
            psz++;

        // skip 0x
        if (psz[0] == '0' && tolower(psz[1]) == 'x')
            psz += 2;

        // hex string to uint
        const char* pbegin = psz;
        while (::HexDigit(*psz) != -1)
            psz++;
        psz--;
        unsigned char* p1 = (unsigned char*)bytes;
        unsigned char* pend = p1 + WIDTH;
        while (psz >= pbegin && p1 < pend) {
            *p1 = ::HexDigit(*psz--);
            if (psz >= pbegin) {
                *p1 |= ((unsigned char)::HexDigit(*psz--) << 4);
                p1++;
            }
        }
    }

    void SetHex(const std::string &s) {
        SetHex(s.c_str());
    }

    _bits256() {
        SetNull();
    }

    explicit _bits256(const std::string _bytes) {
        SetHex(_bytes);
    }

    _bits256& operator= (const std::string _bytes) {
        SetHex(_bytes);
        return *this;
    }

    _bits256(const char *psz) {
        SetHex(psz);
    }

    friend std::ostream& operator<<(std::ostream& out, const _bits256 &b);
};

std::ostream& operator<<(std::ostream& out, const _bits256 &b) {
    std::string res = "";
    for (int i=0; i<32; i++)
        res = strprintf("%02x", b.bytes[i]) + res;
    out << res;
    return out;
}

typedef union _bits256 bits256;
typedef union _bits256 uint256;

struct notarized_checkpoint /* komodo_structs.h */
    {
        uint256 notarized_hash,notarized_desttxid,MoM,MoMoM;
        int32_t nHeight,notarized_height,MoMdepth,MoMoMdepth,MoMoMoffset,kmdstarti,kmdendi;
        friend std::ostream& operator<<(std::ostream& out, const notarized_checkpoint &nc);
    };

std::ostream& operator<<(std::ostream& out, const notarized_checkpoint &nc) {
    out << "{"
        << "\"0x" << nc.notarized_hash << "\"" << ", "
        << "\"0x" << nc.notarized_desttxid << "\"" << ", "
        << "\"0x" << nc.MoM << "\"" << ", "
        << "\"0x" << nc.MoMoM << "\"" << ", "
        << nc.nHeight << ", " << nc.notarized_height << ", " << nc.MoMdepth << ", "
        << nc.MoMoMdepth << ", "
        << nc.MoMoMoffset << ", " << nc.kmdstarti << ", " << nc.kmdendi
        << "}";
    return out;
}
typedef struct notarized_checkpoint t_notarized_checkpoint;

#include "npoints-data.h"

int main() {

    /* uint256 a("0x0ef6e4eab67aab0c780e94afdbd35c4285c26897aa374434570e2925a5e98f57");
    std::cout << a << std::endl;
    uint256 b;
    b = a;
    std::cout << b << std::endl;

    std::vector<uint256> va = {"0x0", "0x1"};
    for (auto a : va) std::cout << a << std::endl;

    struct notarized_checkpoint nc = {"0x000000000000000000000000000000000000000000000000000000000000dead", "0x0000000000000000000000000000000000000000000000000000000000000000", "0x0000000000000000000000000000000000000000000000000000000000000000", "0x0000000000000000000000000000000000000000000000000000000000000000", 0, 0, 0, 0, 0, 0, 0};
    std::cout << nc.notarized_hash << std::endl;

    std::cout << nc << std::endl; */

    std::cout << "Size: " << sizeof(notarized_checkpoints)/sizeof(notarized_checkpoints[0]) << std::endl;

    return 0;
}