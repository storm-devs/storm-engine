#include "xdefines.h"
#include <defines.h>

#include "storm_assert.h"

//////////////////////////////////////////////
///    DEFINE GLOBAL DATA                 ////
////////////////////////////////////////////////////////////////////////////////////////////////////
entid_t g_idInterface;

COMMANDDESCR pCommandsList[COMMAND_QUANTITY] = {
    {"activate", ACTION_ACTIVATE},      {"deactivate", ACTION_DEACTIVATE}, {"select", ACTION_SELECT},
    {"rightstep", ACTION_RIGHTSTEP},    {"leftstep", ACTION_LEFTSTEP},     {"upstep", ACTION_UPSTEP},
    {"downstep", ACTION_DOWNSTEP},      {"speedleft", ACTION_SPEEDLEFT},   {"speedright", ACTION_SPEEDRIGHT},
    {"speedup", ACTION_SPEEDUP},        {"speeddown", ACTION_SPEEDDOWN},   {"click", ACTION_MOUSECLICK},
    {"dblclick", ACTION_MOUSEDBLCLICK}, {"rclick", ACTION_MOUSERCLICK},    {"dblrclick", ACTION_MOUSERDBLCLICK}};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////  END GLOBAL DATA                   ////
//////////////////////////////////////////////

int FindCommand(const char *comName)
{
    int i;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (!_stricmp(comName, pCommandsList[i].sName))
            break;
    if (i == COMMAND_QUANTITY)
        return -1;
    return i;
}

int FindCommand(int comID)
{
    int i;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (pCommandsList[i].code == comID)
            break;
    if (i == COMMAND_QUANTITY)
        return -1;
    return i;
}

uint32_t ColorInterpolate(uint32_t sCol, uint32_t dCol, float m)
{
    int a, r, g, b;
    a = ALPHA(sCol);
    r = RED(sCol);
    g = GREEN(sCol);
    b = GREEN(sCol);
    int ad, rd, gd, bd;
    ad = ALPHA(dCol);
    rd = RED(dCol);
    gd = GREEN(dCol);
    bd = BLUE(dCol);

    a += static_cast<int>((ad - a) * m);
    r += static_cast<int>((rd - r) * m);
    g += static_cast<int>((gd - g) * m);
    b += static_cast<int>((bd - b) * m);
    /*    a = a>ad ? int(a-(a-ad)*m) : int(ad-(ad-a)*m);
      r = r>rd ? int(r-(r-rd)*m) : int(rd-(rd-r)*m);
      g = g>gd ? int(g-(g-gd)*m) : int(gd-(gd-g)*m);
      b = b>bd ? int(b-(b-bd)*m) : int(bd-(bd-b)*m);*/
    return ARGB(a, r, g, b);
}

void DublicateString(char *&pDstStr, const char *pSrcStr)
{
    if (!pSrcStr || pSrcStr[0] == 0) // boal fix
    {
        pDstStr = nullptr;
    }
    else
    {
        delete pDstStr;
        const auto len = strlen(pSrcStr) + 1;
        pDstStr = new char[len];
        Assert(pDstStr);
        memcpy(pDstStr, pSrcStr, len);
    }
    /*
    if( pSrcStr )
    {
      pDstStr = new char[strlen(pSrcStr)+1];
      Assert(pDstStr);
      strcpy_s(pDstStr,pSrcStr);
    } else {
        pDstStr = 0;
    } */
}
