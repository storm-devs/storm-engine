#pragma once

#include "Attributes.h"
#include "storm_assert.h"

inline char *GetFirstName(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    char *pName = pACharacter->GetAttribute("name");
    Assert(pName);
    return pName;
}

inline char *GetLastName(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    char *pName = pACharacter->GetAttribute("name");
    Assert(pName);
    return pName;
}

inline long GetIndex(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    Assert(pACharacter->FindAClass(pACharacter, "index"));
    return pACharacter->GetAttributeAsDword("index");
}

inline long GetNation(ATTRIBUTES *pACharacter)
{
    Assert(pACharacter);
    Assert(pACharacter->FindAClass(pACharacter, "nation"));
    return pACharacter->GetAttributeAsDword("nation");
}

inline long GetRank(ATTRIBUTES *pACharacter)
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
