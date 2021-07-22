#include "ships_list.h"
#include <cassert>

#include "core.h"

#include "Entity.h"
#include "shared/battle_interface/msg_control.h"
//#include "battle_command.h"

#define LOADING_MODE_MY_SHIP 0
#define LOADING_MODE_ENEMY_SHIP 1

class TMP_LONG_STACK
{
    long sizeIncr;
    long *ldat;
    long datsize;
    long curidx;
    long defReturn;

  public:
    TMP_LONG_STACK()
    {
        sizeIncr = 100;
        ldat = nullptr;
        datsize = 0;
        curidx = 0;
        defReturn = -1;
    }

    ~TMP_LONG_STACK()
    {
        delete[] ldat;
        ldat = nullptr;
    }

    void Push(long data);
    long Pop();
    long GetFore();

    void SetSizeIncrement(long sizeinc)
    {
        sizeIncr = sizeinc;
    }

    void SetDefReturn(long defRet)
    {
        defReturn = defRet;
    }
};

void TMP_LONG_STACK::Push(long data)
{
    if (curidx >= datsize)
    {
        if (sizeIncr <= 0)
        {
            core.Trace("WARNING! push for TMP_LONG_STACK impossible - array grid <= 0");
            return;
        }
        auto *const pold = ldat;
        ldat = new long[datsize + sizeIncr];
        if (ldat == nullptr)
        {
            throw std::runtime_error("allocate memory error");
        }
        if (pold != nullptr)
        {
            memcpy(ldat, pold, sizeof(long) * datsize);
            delete pold;
        }
        datsize += sizeIncr;
    }
    ldat[curidx++] = data;
}

long TMP_LONG_STACK::GetFore()
{
    if (ldat == nullptr || curidx <= 0)
    {
        core.Trace("WARNING! GetFore from TMP_LONG_STACK is empty");
        return defReturn;
    }
    const auto retVal = ldat[0];
    curidx--;
    if (curidx == 0)
    {
        delete ldat;
        ldat = nullptr;
        datsize = 0;
    }
    else
        for (auto i = 0; i < curidx; i++)
            ldat[i] = ldat[i + 1];
    return retVal;
}

long TMP_LONG_STACK::Pop()
{
    if (ldat == nullptr || curidx <= 0)
    {
        core.Trace("WARNING! pop from TMP_LONG_STACK is empty");
        return defReturn;
    }
    const auto retVal = ldat[--curidx];
    if (curidx == 0)
    {
        delete ldat;
        ldat = nullptr;
        datsize = 0;
    }
    return retVal;
}

void SetNLongData(VDATA *pvd, int n, ...)
{
    va_list args;
    va_start(args, n);

    for (auto i = 0; i < n; i++)
    {
        auto *const pIVal = va_arg(args, long *);
        if (!pIVal)
            break;
        const auto nDefVal = va_arg(args, long);
        if (pvd)
            pvd->Get(*pIVal, i);
        else
            *pIVal = nDefVal;
    }

    va_end(args);
}

SHIP_DESCRIBE_LIST::SHIP_DESCRIBE_LIST()
{
    root = nullptr;
    mainCharacter = nullptr;
    pMainShipAttr = nullptr;
}

SHIP_DESCRIBE_LIST::~SHIP_DESCRIBE_LIST()
{
    ReleaseAll();
}

void SHIP_DESCRIBE_LIST::ShipSink(long charIdx)
{
    auto *const sd = FindShip(charIdx);
    if (sd == nullptr)
        return;
    sd->isDead = true;
}

void SHIP_DESCRIBE_LIST::Release(long charIdx)
{
    if (charIdx == -1L)
        return;
    if (root == nullptr)
        return;

    auto *sd = root;
    if (root->characterIndex == charIdx)
    {
        root = root->next;
        delete sd;
        return;
    }

    auto *pr = root->next;
    while (pr != nullptr)
    {
        if (pr->characterIndex == charIdx)
        {
            sd->next = pr->next;
            delete pr;
            return;
        }
        sd = pr;
        pr = pr->next;
    }

    mainCharacter = nullptr;
    pMainShipAttr = nullptr;
}

void SHIP_DESCRIBE_LIST::Add(long mainChrIndex, long chIdx, ATTRIBUTES *pChAttr, ATTRIBUTES *pShipAttr, bool bMyShip,
                             long relation, uint32_t dwShipColor)
{
    assert(pChAttr != NULL);
    assert(pShipAttr != NULL);
    auto *pr = new SHIP_DESCR;
    if (pr == nullptr)
    {
        throw std::runtime_error("Can`t allocate memory");
    }
    pr->characterIndex = chIdx;
    pr->maxCrew = pShipAttr->GetAttributeAsDword("MaxCrew");
    pr->maxHP = pShipAttr->GetAttributeAsDword("HP");
    pr->maxSP = pShipAttr->GetAttributeAsDword("SP");
    pr->isMyShip = bMyShip;
    pr->relation = relation;
    pr->isDead = false;
    pr->pShip = nullptr;
    pr->dwShipColor = dwShipColor;

    auto *const pAttr = pChAttr->GetAttributeClass("Ship");
    assert(pAttr != NULL);
    pr->pAttr = pAttr;
    long lTmp;
    SetNLongData(core.Event(BI_EVENT_GET_DATA, "ll", BIDT_SHIPPICTURE, chIdx), 4, &pr->pictureNum, 0,
                 &pr->selectPictureNum, 0, &pr->textureNum, -1, &lTmp, 0); //&pr->isDead,false );
    pr->isDead = lTmp != 0;

    pr->next = nullptr;

    // find this ship
    const auto &entities = EntityManager::GetEntityIdVector("ship");
    for (auto ship : entities)
    {
        auto *vob = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(ship));
        if (vob == nullptr)
            continue;
        auto *pA = vob->GetACharacter();
        if (static_cast<long>(pA->GetAttributeAsDword("index")) == chIdx)
        {
            pr->pShip = vob;
            break;
        }
    }
    /*if( NetFindClass(false,&ei,"netship") ) do
    {
      VAI_OBJBASE * vob = (VAI_OBJBASE*)EntityManager::GetEntityPointer(ei);
      if(vob== nullptr) continue;
      ATTRIBUTES *pA = vob->GetACharacter();
      if((long)pA->GetAttributeAsDword("id")==chIdx)
      {
        pr->pShip = vob;
        break;
      }
    } while( NetFindClassNext(false,&ei) );*/

    auto *ptmp = root;
    if (ptmp == nullptr)
        root = pr;
    else
    {
        while (ptmp->next != nullptr)
            ptmp = ptmp->next;
        ptmp->next = pr;
    }

    // if(pChAttr->GetAttributeAsDword("MainCharacter",0)!=0)
    if (mainChrIndex == chIdx)
    {
        mainCharacter = pr;
        pMainShipAttr = pShipAttr;
    }
}

void SHIP_DESCRIBE_LIST::ReleaseAll()
{
    while (root != nullptr)
    {
        auto *const ptmp = root;
        root = root->next;
        delete ptmp;
    }
    root = nullptr;
    mainCharacter = nullptr;
    pMainShipAttr = nullptr;
}

SHIP_DESCRIBE_LIST::SHIP_DESCR *SHIP_DESCRIBE_LIST::FindShip(long idxCharacter) const
{
    for (auto *ptmp = root; ptmp != nullptr; ptmp = ptmp->next)
        if (ptmp->characterIndex == idxCharacter)
            return ptmp;
    return nullptr;
}

void SHIP_DESCRIBE_LIST::Refresh()
{
    ReleaseAll();

    TMP_LONG_STACK tls;

    const auto &entities = EntityManager::GetEntityIdVector("ship");
    for (auto ship : entities)
    {
        auto *vob = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(ship));
        if (vob == nullptr)
            continue;
        auto *pA = vob->GetACharacter();
        if (pA == nullptr)
            continue;
        tls.Push(static_cast<long>(pA->GetAttributeAsDword("index")));
    }

    /*if( NetFindClass(false,&ei,"NetShip") ) do
    {
      VAI_OBJBASE * vob = (VAI_OBJBASE*)EntityManager::GetEntityPointer(ei);
      if(vob== nullptr) continue;
      ATTRIBUTES * pA= vob->GetACharacter();
      if(pA== nullptr) continue;
      tls.Push((long)pA->GetAttributeAsDword("id"));
    } while( NetFindClassNext(false,&ei) );*/
    tls.Push(-1);

    for (auto chrIdx = tls.GetFore(); chrIdx >= 0; chrIdx = tls.GetFore())
    {
        core.Event("BI_CallUpdateShip", "l", chrIdx);
    }

    // BATTLE_COMMAND::m_bMakeModeUpdate = true;
}
