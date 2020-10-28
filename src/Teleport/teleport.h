#ifndef _TELEPORT_H_
#define _TELEPORT_H_

#include "dx8render.h"
#include "vmodule_api.h"

class TMPTELEPORT : public ENTITY
{
    VDX8RENDER *rs;

    struct TELEPORT_DESCR
    {
        int num;
        char *name;
    } * m_descrArray;

  public:
    TMPTELEPORT();
    ~TMPTELEPORT();
    bool Init();
    void Execute(dword Delta_Time);
    void Realize(dword Delta_Time);
    dword _cdecl ProcessMessage(MESSAGE &message);

  protected:
    void ReleaseAll();
    void SetShowData(ATTRIBUTES *pA);
    void SortShowData();
    void XChange(TELEPORT_DESCR &d1, TELEPORT_DESCR &d2);

  protected:
    long m_leftPos;
    long m_topPos;
    long m_deltaPos;
    long m_showStrQuantity;

    long m_nStrQuantity;
    long m_nCurStr;
    long m_nCurShowPos;

    long m_nShowType;
};

class FINDFILESINTODIRECTORY : public ENTITY
{
  public:
    FINDFILESINTODIRECTORY()
    {
    }
    ~FINDFILESINTODIRECTORY()
    {
    }
    bool Init();
};

class FINDDIALOGNODES : public ENTITY
{
  public:
    FINDDIALOGNODES()
    {
    }
    ~FINDDIALOGNODES()
    {
    }
    bool Init();
};

#endif
