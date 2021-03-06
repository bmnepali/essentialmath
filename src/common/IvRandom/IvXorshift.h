//===============================================================================
// @ IvXorshift.h
//
// Implementation of ?'s Xorshift1024*
// ------------------------------------------------------------------------------
// Copyright (C) 2014 by James M. Van Verth and Lars M. Bishop.
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
//===============================================================================

#pragma once

#include "IvTypes.h"
#include "IvAssert.h"

//-------------------------------------------------------------------------------
//-- Function Prototypes --------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvXorshift
{
public:
    IvXorshift( UInt64 seed );
    
    UInt32 Random();
    float  RandomFloat();
    inline UInt32 MaxRand() { return UInt32(0xffffffff); }
    
private:
    UInt64 mS[16];
    int    mP;
};

//-------------------------------------------------------------------------------
//-- Inline Functions -----------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Xorshift64
//-------------------------------------------------------------------------------
// Helper function for initialization
//-------------------------------------------------------------------------------
inline UInt64 Xorshift64(UInt64 x) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    
    return x;
}

//-------------------------------------------------------------------------------
// @ IvXorshift::IvXorshift()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
inline
IvXorshift::IvXorshift( UInt64 seed )
{
    ASSERT(0 != seed);
    // use 64-bit xorshift to set initial state
    mS[0] = seed;
    for (int i = 1; i < 16; ++i)
    {
        mS[i] = Xorshift64(mS[i-1]);
    }
    mP = 0;
}

//-------------------------------------------------------------------------------
// @ IvXorshift::Random()
//-------------------------------------------------------------------------------
// Return random integer
//-------------------------------------------------------------------------------
inline UInt32
IvXorshift::Random()
{
    UInt64 s0 = mS[mP];
    mP = (mP + 1) & 15;
    UInt64 s1 = mS[mP];
    s1 ^= s1 << 31;
    s1 ^= s1 >> 11;
    s0 ^= s0 >> 30;
    s1 ^= s0;
    mS[mP] = s1;
    return (s1*1181783497276652981LL) >> 32;
}

//-------------------------------------------------------------------------------
// @ IvXorshift::RandomFloat()
//-------------------------------------------------------------------------------
// Return random floating point number in range [0,1)
//-------------------------------------------------------------------------------
inline float
IvXorshift::RandomFloat()
{
    union
    {
        unsigned int mI;
        float        mF;
    } floatConv;
    floatConv.mI = 0x3f800000 | (Random() >> 9);
    float f = floatConv.mF - 1.0f;
    return f;
}

