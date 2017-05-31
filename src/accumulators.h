#ifndef PIVX_ACCUMULATORS_H
#define PIVX_ACCUMULATORS_H

#include "libzerocoin/Zerocoin.h"
#include "primitives/zerocoin.h"
#include "uint256.h"

class CAccumulators
{
public:
    static CAccumulators& getInstance()
    {
        static CAccumulators instance;
        return instance;
    }
private:
    std::map<int, std::unique_ptr<libzerocoin::Accumulator> > mapAccumulators;
    std::map<uint256, int> mapPubCoins;
    std::map<uint32_t, CBigNum> mapAccumulatorValues;

    CAccumulators() { Setup(); }
    void Setup();

public:
    CAccumulators(CAccumulators const&) = delete;
    void operator=(CAccumulators const&) = delete;

    libzerocoin::Accumulator Get(libzerocoin::CoinDenomination denomination);
    bool AddPubCoinToAccumulator(libzerocoin::PublicCoin publicCoin);
    bool IntializeWitnessAndAccumulator(const CZerocoinMint &zerocoinSelected, const libzerocoin::PublicCoin &pubcoinSelected, libzerocoin::Accumulator& accumulator, libzerocoin::AccumulatorWitness& witness);

    //checksum/checkpoint
    void AddAccumulatorChecksum(const uint32_t nChecksum, const CBigNum &bnValue);
    void AddAccumulatorCheckpoint(const uint256 nCheckpoint);
    CBigNum GetAccumulatorValueFromChecksum(uint32_t nChecksum);
    uint32_t GetChecksum(const CBigNum &bnValue);
    uint32_t GetChecksum(const libzerocoin::Accumulator &accumulator);
    uint256 GetCheckpoint();
    uint256 GetCheckpoint(int nHeight);
    CBigNum GetAccumulatorValueFromCheckpoint(const uint256 nCheckpoint, libzerocoin::CoinDenomination denomination);
    bool ResetToCheckpoint(uint256 nCheckpoint);
};

uint32_t ParseChecksum(uint256 nChecksum, libzerocoin::CoinDenomination denomination);


#endif //PIVX_ACCUMULATORS_H
