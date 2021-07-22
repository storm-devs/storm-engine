#pragma once

#include "controls.h"
#include "vmodule_api.h"
#include <string>
#include <vector>

struct ControlTree
{
  public:
    struct ControlChild
    {
        ControlChild();
        ~ControlChild();

        void Process(float fDeltaTime, ControlTree *pControlTree);
        bool ControlInAction(ControlTree *pControlTree, const char *pcControlName, long _nLayer);

        long index;
        std::string sControlName;
        std::vector<ControlChild> aChild;
        bool bActive;
        bool bWaitReleaseControl;
        float fTimeOut;
        float fCurTime;
        std::string sOutControlName;
        long nLayer;
    };

    struct OutControlInfo
    {
        std::string sControlName;
        CONTROL_STATE_TYPE state;
    };

  public:
    ControlTree();
    ~ControlTree();

    void Process();

    long AddControlChild(long nParentIdx, const char *pcControlName, const char *pcOutControlName, float fTimeOut);
    CONTROL_STATE_TYPE CheckControlActivated(const char *pcControlName);

    bool AddOutControl(const char *pcOutControlName, bool isActive);
    void ControlInAction(const char *pcControlName, long nLayer);
    bool ExcludeControlFromActive(const char *pcControlName);

  protected:
    void Init();
    void Release();
    ControlChild *FindControlChild(long idx);
    ControlChild *FindControlChild(long idx, ControlChild *pParent);

  protected:
    long m_nControlsNum;
    ControlChild m_RootControl;

    std::vector<OutControlInfo> m_aOutControlList;
};
