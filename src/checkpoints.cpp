// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
		(0, uint256("0x00009bf5332f4a3706a58da9a4534758bdc2a07edcfae4f079d8823f047445f5"))
		(1, uint256("0x4a45fe58c3472f4eeab543ecebe79d536e97d3f46b2d7c4383babeb8ff2685fa"))
		(2, uint256("0xae22f674820b3c2d60aff2643294b4da3c1aba9fd00e6346b34d130cb72134a0"))
		(5, uint256("0x3f4441e850faff4b2f88252d8e96d791a850f58fe349fee43f7a61eaf2f49fa1"))
		(189, uint256("0xed9577d9739bd79151d2bf15c1243f68891b5cf5200e80b726779bfd2f102ef1"))
		(324, uint256("0x1f90a465537c4b4116b3e2e08ce25e14769eff1230a7bb7df3c90eaa028675ed"))
		(463, uint256("0x17426e4b5c07dbfcee9013cec8c13b2998224d8115d532dac2c5a18d6c85fda1"))
		(501, uint256("0x78843407ac6df30335c459097eb3c6a6166c2c038e133d643b738b38b63a4cd3"))
		(548, uint256("0x73a0bbc8dfb8fe2dbf0cf6e757b50320e9ab26c0c06b98065cbd1582c28bf8b6"))
		(681, uint256("0x044a427dee2095fa487614808ec6c7a67c0ec6f07bb31195f9edaf6b271444a8"))
		(799, uint256("0xdce9b941ba615311d68e16fc745d29e4b698453c9fff0c36194842184bb93a56"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
