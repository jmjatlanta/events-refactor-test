#include <iostream>
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include "tinyformat.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

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

    bool operator != (const _bits256 &other) const {
        bool fEqual = true;
        for (size_t i=0; i<32; i++)
            if (this->bytes[i] != other.bytes[i])
                { fEqual = false; break; }
        return !fEqual;
    }

    bool operator == (const _bits256 &other) const {
        return !(*this != other);
    }

    friend std::ostream& operator<<(std::ostream& out, const _bits256 &b);
};

std::ostream& operator<<(std::ostream& out, const _bits256 &b) {
    std::string res = "";
    bool fIsNull = true;
    for (int i=0; i<32; i++) if (b.bytes[i]) { fIsNull=false; break; }

    if (!fIsNull) {
        for (int i=0; i<32; i++)
            res = strprintf("%02x", b.bytes[i]) + res;
    } else
            res = "0";

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

#define portable_mutex_lock pthread_mutex_lock
#define portable_mutex_unlock pthread_mutex_unlock
#define KOMODO_ASSETCHAIN_MAXLEN 65 /* util.h, bitcoind.cpp, komodo_defs.h, komodo_globals.h, komodo_structs.h */
char ASSETCHAINS_SYMBOL[] = {0};
// char ASSETCHAINS_SYMBOL[] = {'M','C','L', 0}

typedef boost::multi_index::multi_index_container<
        notarized_checkpoint,
        boost::multi_index::indexed_by<
                boost::multi_index::sequenced<>, // sorted by insertion order
                boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<
                                notarized_checkpoint, int32_t, &notarized_checkpoint::nHeight
                        >
                >, // sorted by nHeight
                boost::multi_index::ordered_non_unique<
                        boost::multi_index::member<
                                notarized_checkpoint, int32_t, &notarized_checkpoint::notarized_height
                        >
                > // sorted by notarized_height
        > > notarized_checkpoint_container;

namespace old_space {

    struct komodo_state
    {
        uint256 NOTARIZED_HASH,NOTARIZED_DESTTXID,MoM;
        int32_t SAVEDHEIGHT,CURRENT_HEIGHT,NOTARIZED_HEIGHT,MoMdepth;
        uint32_t SAVEDTIMESTAMP;
        uint64_t deposited,issued,withdrawn,approved,redeemed,shorted;
        struct notarized_checkpoint *NPOINTS; int32_t NUM_NPOINTS,last_NPOINTS;
        //std::list<std::shared_ptr<komodo::event>> events;
        uint32_t RTbufs[64][3]; uint64_t RTmask;
        //bool add_event(const std::string& symbol, const uint32_t height, std::shared_ptr<komodo::event> in);
    };

    struct komodo_state sp;
    pthread_mutex_t komodo_mutex,staked_mutex;

    /* komodo_notary.h */
    void komodo_notarized_update(struct komodo_state *sp,int32_t nHeight,int32_t notarized_height,uint256 notarized_hash,uint256 notarized_desttxid,uint256 MoM,int32_t MoMdepth)
    {
        struct notarized_checkpoint *np;
        if ( notarized_height >= nHeight )
        {
            fprintf(stderr,"komodo_notarized_update REJECT notarized_height %d > %d nHeight\n",notarized_height,nHeight);
            return;
        }
        if ( 0 && ASSETCHAINS_SYMBOL[0] != 0 )
            fprintf(stderr,"[%s] komodo_notarized_update nHeight.%d notarized_height.%d\n",ASSETCHAINS_SYMBOL,nHeight,notarized_height);
        portable_mutex_lock(&komodo_mutex);
        sp->NPOINTS = (struct notarized_checkpoint *)realloc(sp->NPOINTS,(sp->NUM_NPOINTS+1) * sizeof(*sp->NPOINTS));
        np = &sp->NPOINTS[sp->NUM_NPOINTS++];
        memset(np,0,sizeof(*np));
        np->nHeight = nHeight;
        sp->NOTARIZED_HEIGHT = np->notarized_height = notarized_height;
        sp->NOTARIZED_HASH = np->notarized_hash = notarized_hash;
        sp->NOTARIZED_DESTTXID = np->notarized_desttxid = notarized_desttxid;
        sp->MoM = np->MoM = MoM;
        sp->MoMdepth = np->MoMdepth = MoMdepth;
        portable_mutex_unlock(&komodo_mutex);
    }
    struct notarized_checkpoint *komodo_npptr_for_height(int32_t height, int *idx)
    {
        char symbol[KOMODO_ASSETCHAIN_MAXLEN],dest[KOMODO_ASSETCHAIN_MAXLEN]; int32_t i; struct komodo_state *sp; struct notarized_checkpoint *np = 0;
        //if ( (sp= komodo_stateptr(symbol,dest)) != 0 )
        sp = &old_space::sp;

        {
            for (i=sp->NUM_NPOINTS-1; i>=0; i--)
            {
                *idx = i;
                np = &sp->NPOINTS[i];
                if ( np->MoMdepth != 0 && height > np->notarized_height-(np->MoMdepth&0xffff) && height <= np->notarized_height )
                    return(np);
            }
        }
        *idx = -1;
        return(0);
    }

    struct notarized_checkpoint *komodo_npptr(int32_t height)
    {
        int idx;
        return komodo_npptr_for_height(height, &idx);
    }

struct notarized_checkpoint *komodo_npptr_at(int idx)
{
    char symbol[KOMODO_ASSETCHAIN_MAXLEN],dest[KOMODO_ASSETCHAIN_MAXLEN]; struct komodo_state *sp;
    //if ( (sp= komodo_stateptr(symbol,dest)) != 0 )
    sp = &old_space::sp;

        if (idx < sp->NUM_NPOINTS)
            return &sp->NPOINTS[idx];
    return(0);
}

int32_t komodo_prevMoMheight()
{
    static uint256 zero;
    zero.SetNull();

    char symbol[KOMODO_ASSETCHAIN_MAXLEN],dest[KOMODO_ASSETCHAIN_MAXLEN]; int32_t i; struct komodo_state *sp; struct notarized_checkpoint *np = 0;
    //if ( (sp= komodo_stateptr(symbol,dest)) != 0 )
    sp = &old_space::sp;
    {
        for (i=sp->NUM_NPOINTS-1; i>=0; i--)
        {
            np = &sp->NPOINTS[i];
            if ( np->MoM != zero )
                return(np->notarized_height);
        }
    }
    return(0);
}

};

namespace new_space {

};

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

    size_t npoints_max =  sizeof(notarized_checkpoints)/sizeof(notarized_checkpoints[0]);
    std::cout << "Size: " << npoints_max;

    // set the MoMdepth for tests
    notarized_checkpoints[npoints_max-1].MoMdepth = 1;
    notarized_checkpoints[npoints_max-3].MoMdepth = 20;

    notarized_checkpoints[777].MoM = uint256("0xdead");
    notarized_checkpoints[777].MoMdepth = 1;

    // fill the old structures
    for (size_t idx = 0; idx < npoints_max; idx++)
    {
        old_space::komodo_notarized_update(&old_space::sp,
        notarized_checkpoints[idx].nHeight,
        notarized_checkpoints[idx].notarized_height,
        notarized_checkpoints[idx].notarized_hash,
        notarized_checkpoints[idx].notarized_desttxid,
        notarized_checkpoints[idx].MoM,
        notarized_checkpoints[idx].MoMdepth);
    }

    std::cout << " (" << old_space::sp.NUM_NPOINTS << ")" << std::endl;

    int idx;
    int32_t arr_heights[] = { 2524224, 2524223, 2524210, 2524209, 2524200, 2524190, 2524171, 2524170 };

    struct notarized_checkpoint *np_old;

    for (size_t i = 0; i < sizeof(arr_heights)/sizeof(arr_heights[0]); i++) {
        np_old = old_space::komodo_npptr_for_height(arr_heights[i], &idx);
        std::cout << "komodo_npptr_for_height(" << arr_heights[i] << "): ";
        if (np_old)
            std::cout << np_old - old_space::sp.NPOINTS;
        else
            std::cout << 0;
        std::cout << " (idx = " << idx << ")" << std::endl;
    }

    np_old = old_space::komodo_npptr_at(0);
    std::cout << "komodo_npptr_at(" << (0) << ") = " << (*np_old) << std::endl;
    np_old = old_space::komodo_npptr_at(npoints_max-1);
    std::cout << "komodo_npptr_at(" << (npoints_max-1) << ") = " << (*np_old) << std::endl;
    np_old = old_space::komodo_npptr_at(777);
    std::cout << "komodo_npptr_at(" << (777) << ") = " << (*np_old) << std::endl;

    std::cout << "komodo_prevMoMheight() = " << old_space::komodo_prevMoMheight() << std::endl;

    return 0;
}