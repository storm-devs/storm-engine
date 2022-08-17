#pragma once

#include "attributes.h"
#include "storm_assert.h"

inline int32_t GetIndex(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    Assert(pACharacter->FindAClass(pACharacter, "index"));
    return pACharacter->GetAttributeAsDword("index");
}

inline int32_t GetNation(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    Assert(pACharacter->FindAClass(pACharacter, "nation"));
    return pACharacter->GetAttributeAsDword("nation");
}

inline int32_t GetRank(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    Assert(pACharacter->FindAClass(pACharacter, "rank"));
    return pACharacter->GetAttributeAsDword("rank");
}

inline ATTRIBUTES *GetAShip(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    ATTRIBUTES *pAShip = pACharacter->FindAClass(pACharacter, "ship");
    Assert(pAShip);
    return pAShip;
}
