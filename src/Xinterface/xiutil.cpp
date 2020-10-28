#include "stringService.h"
#include "vxservice.h" // pictures & string lists control
#include "xi_defines.h"
#include <stdio.h>

//////////////////////////////////////////////
///    DEFINE GLOBAL DATA                 ////
////////////////////////////////////////////////////////////////////////////////////////////////////
ENTITY_ID g_idInterface;

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
    int i = 0;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (!stricmp(comName, pCommandsList[i].sName))
            break;
    if (i == COMMAND_QUANTITY)
        return -1;
    else
        return i;
}

int FindCommand(int comID)
{
    int i = 0;
    for (i = 0; i < COMMAND_QUANTITY; i++)
        if (pCommandsList[i].code == comID)
            break;
    if (i == COMMAND_QUANTITY)
        return -1;
    else
        return i;
}

DWORD ColorInterpolate(DWORD sCol, DWORD dCol, float m)
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

    a += int((ad - a) * m);
    r += int((rd - r) * m);
    g += int((gd - g) * m);
    b += int((bd - b) * m);

    return ARGB(a, r, g, b);
}

void DublicateString(char *&pDstStr, const char *pSrcStr)
{
    if (!pSrcStr || pSrcStr[0] == 0) // boal fix
    {
        pDstStr = 0;
    }
    else
    {
        pDstStr = NEW char[strlen(pSrcStr) + 1];
        Assert(pDstStr);
        strcpy(pDstStr, pSrcStr);
    }
}