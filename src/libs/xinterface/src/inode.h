#pragma once

#include "Nodes/xi_tooltips.h"
#include "VideoTexture.h"
#include "core.h"
#include "defines.h"
#include "dx9render.h"
#include "vfile_service.h"
#include "vmodule_api.h"
#include "vxservice.h"

class CXI_ToolTip;

#define MOUSE_LBUTTON 0
#define MOUSE_RBUTTON 1
#define MOUSE_MBUTTON 2

// determines which coordinates are used to convert to absolute values (can be added using or)
#define ABSOLUTE_LEFT 1
#define ABSOLUTE_TOP 2
#define ABSOLUTE_RIGHT 4
#define ABSOLUTE_BOTTOM 8

#define NODETYPE_BOUNDER 0
#define NODETYPE_BUTTON 1
#define NODETYPE_CHANGER 2
#define NODETYPE_CONTEXTHELP 3
#define NODETYPE_FOURIMAGE 4
#define NODETYPE_GLOWER 5
#define NODETYPE_IMAGECOLLECTION 6
#define NODETYPE_LINECOLLECTION 7
#define NODETYPE_PICTURE 8
#define NODETYPE_RECTANGLE 9
#define NODETYPE_SCROLLBAR 10
#define NODETYPE_SCROLLIMAGE 11
#define NODETYPE_SCROLLTEXT 12
#define NODETYPE_STATUSLINE 13
#define NODETYPE_STRINGCOLLECTION 14
#define NODETYPE_TEXTBUTTON 15
#define NODETYPE_TITLE 16
#define NODETYPE_VIDEO 17
#define NODETYPE_LRCHANGER 18
#define NODETYPE_TWOPICTURE 19
#define NODETYPE_SCROLLER 20
#define NODETYPE_QTITLE 21
#define NODETYPE_QTEXTS 22
#define NODETYPE_SLIDEPICTURE 23
#define NODETYPE_FORMATEDTEXTS 24
#define NODETYPE_EDITBOX 25
#define NODETYPE_SLIDELINE 26
#define NODETYPE_KEYCHANGER 27
#define NODETYPE_GLOWCURSOR 28
#define NODETYPE_VIDEORECT 29
#define NODETYPE_VIMGSCROLL 30
#define NODETYPE_PCEDITBOX 31
#define NODETYPE_SCROLLEDPICTURE 32
#define NODETYPE_WINDOW 33
#define NODETYPE_CHECKBUTTONS 34
#define NODETYPE_TABLE 35
#define NODETYPE_TABSECTION 36
#define NODETYPE_BORDER 37

#define INTERFACE_CONTROL_RIGHT "IRight"
#define INTERFACE_CONTROL_LEFT "ILeft"
#define INTERFACE_CONTROL_UP "IUp"
#define INTERFACE_CONTROL_DOWN "IDown"
#define INTERFACE_CONTROL_LSHIFT "ILeftShift"
#define INTERFACE_CONTROL_RSHIFT "IRightShift"
#define INTERFACE_CONTROL_ENTER "IAction"
#define INTERFACE_CONTROL_BACK "ICancel"
#define INTERFACE_CONTROL_BREAK "IAllCancel"

#define INTERFACE_CONTROL_LCLICK "ILClick"
#define INTERFACE_CONTROL_RCLICK "IRClick"

#define INTERFACE_MOUSE_VERT "ITurnV"
#define INTERFACE_MOUSE_HORZ "ITurnH"

class CINODE;

class XINTERFACE_BASE : public Entity
{
  public:
    virtual storm::QuestFileReader *QuestFileReader() = 0;
    virtual VXSERVICE *PictureService() = 0;
    virtual VSTRSERVICE *StringService() = 0;
    virtual VDX9RENDER *RenderService() = 0;
    virtual void *GetCurrentNode() = 0;
    virtual FXYPOINT GetMousePoint() = 0;
    virtual long PrintIntoWindow(long wl, long wr, long idFont, uint32_t dwFCol, uint32_t dwBCol, long align,
                                 bool shadow, float scale, long sxs, long sys, long left, long top, const char *str,
                                 int nWidthForScaleCorrecting = -1, int nSplit = 0) = 0;

    virtual CINODE *FindNode(const char *sNodeName, CINODE *findRoot) = 0;
    virtual void ShowWindow(const char *pcWindowName, bool bShow) = 0;
    virtual void DisableWindow(const char *pcWindowName, bool bDisable) = 0;
    virtual void AddNodeToWindow(const char *pcNodeName, const char *pcWindowName) = 0;

    virtual void RegistryExitKey(const char *pcKeyName) = 0;

    // blind
    uint32_t GetBlendColor(uint32_t minCol, uint32_t maxCol, float fFactor);

    std::string m_sDialogFileName;
};

class CINODE
{
    long m_nPriority;
    bool m_bShowGlowCursor;
    bool m_bGlowCursorBack;
    bool m_bMouseWeelReaction;
    bool m_bUseUserGlowCursor;
    XYRECT m_rectUserGlowCursor;
    bool m_bUseUserGlowOffset;
    FXYPOINT m_rectUserGlowOffset;

  public:
    struct COMMAND_REDIRECT
    {
        char *sControlName;
        // CINODE *            pControl;
        int command;
        COMMAND_REDIRECT *next;

        COMMAND_REDIRECT()
        {
            sControlName = nullptr;
            next = nullptr;
        }

        ~COMMAND_REDIRECT()
        {
            STORM_DELETE(sControlName);
        }
    };

    struct COMMAND_ACTION
    {
        bool bUse;
        int nSound;
        char *sRetControl;
        // CINODE *         pRetControl;
        COMMAND_REDIRECT *pNextControl;
        char *sEventName;
        long nActionDelay;

        COMMAND_ACTION()
        {
            bUse = false;
            sRetControl = nullptr;
            pNextControl = nullptr;
            sEventName = nullptr;
        }

        ~COMMAND_ACTION()
        {
            STORM_DELETE(sRetControl);
            STORM_DELETE(sEventName);
            while (pNextControl)
            {
                auto *const pOld = pNextControl;
                pNextControl = pNextControl->next;
                delete pOld;
            }
        }
    };

  public:
    CINODE();
    virtual ~CINODE();
    virtual void Draw(bool bSelected, uint32_t Delta_Time) = 0;
    virtual bool Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                      XYRECT &hostRect, XYPOINT &ScreenSize);
    virtual void ReleaseAll() = 0;
    CINODE *DoAction(int wActCode, bool &bBreakPress, bool bFirstPress);
    virtual int CommandExecute(int wActCode) = 0;
    static CINODE *FindNode(CINODE *pNod, const char *sNodName);
    static CINODE *FindNode(CINODE *pNod, int nNodType);
    static CINODE *FindNode(CINODE *pNod, float x, float y);

    CINODE *FindNode(const char *sNodName)
    {
        return FindNode(this, sNodName);
    }

    CINODE *FindNode(int nNodType)
    {
        return FindNode(this, nNodType);
    }

    CINODE *FindNode(float x, float y)
    {
        return FindNode(this, x, y);
    }

    virtual void SetUsing(bool bUsing)
    {
        m_bUse = bUsing;
    }

    virtual bool IsClick(int buttonID, long xPos, long yPos) = 0;
    virtual void FrameProcess(uint32_t DeltaTime);
    virtual void MouseThis(float fX, float fY) = 0;

    virtual long GetClickState()
    {
        return 0;
    }

    void SetPriority(long prior)
    {
        m_nPriority = prior;
    }

    long GetPriority() const
    {
        return m_nPriority;
    }

    bool CheckCommandUsed(int comCode) const;

    bool IsCurrentNode()
    {
        return ptrOwner->GetCurrentNode() == this;
    }

    void NotUsingTime(uint32_t Delta_Time)
    {
    }

    virtual XYRECT GetCursorRect()
    {
        if (m_bUseUserGlowCursor)
            return m_rectUserGlowCursor;
        return m_rect;
    }

    virtual bool IsShowGlowCursor()
    {
        return m_bShowGlowCursor;
    }

    virtual bool IsGlowCursorBack()
    {
        return m_bGlowCursorBack;
    }

    virtual bool IsGlowChanged()
    {
        return false;
    }

    virtual void LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2) = 0;

    virtual void MakeLClickPreaction()
    {
    }

    bool IsWeelActive() const
    {
        return m_bMouseWeelReaction;
    }

    virtual void ChangePosition(XYRECT &rNewPos) = 0;
    virtual void SaveParametersToIni() = 0;

    virtual bool GetInternalNameList(std::vector<std::string> &aStr)
    {
        return false;
    }

    virtual void SetInternalName(std::string &sName)
    {
    }

    virtual uint32_t MessageProc(long msgcode, MESSAGE &message);

    void SetGlowCursor(bool bShowFlag)
    {
        m_bShowGlowCursor = bShowFlag;
    }

    void SetGlowCursorToBack(bool bBackFlag)
    {
        m_bGlowCursorBack = bBackFlag;
    }

    void UpdateGlowOffsets(float &fx, float &fy) const
    {
        if (m_bUseUserGlowOffset)
        {
            fx = m_rectUserGlowOffset.x;
            fy = m_rectUserGlowOffset.y;
        }
    }

    static float GetIniFloat(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                             float fDefault = 0.f);
    static long GetIniLong(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                           long iDefault = 0);
    static bool ReadIniString(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                              char *buf, size_t bufSize, const char *strDef = nullptr);
    static bool GetIniBool(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                           bool bDefault = false);
    static XYRECT GetIniLongRect(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2,
                                 const char *keyName, const XYRECT &rectDefault);
    static FXYRECT GetIniFloatRect(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2,
                                   const char *keyName, const FXYRECT &rectDefault);
    static XYPOINT GetIniLongPoint(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2,
                                   const char *keyName, const XYPOINT &pntDefault);
    static FXYPOINT GetIniFloatPoint(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2,
                                     const char *keyName, const FXYPOINT &pntDefault);
    static uint32_t GetIniARGB(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, const char *keyName,
                               uint32_t dwDefColor = 0);

    void GetRelativeRect(XYRECT &rect) const;
    void GetAbsoluteRect(XYRECT &rect, int at) const;

    static const char *GetSubStr(const char *inStr, char *buf, size_t bufSize, char devChar = ',');
    static bool GetMidStr(const char *inStr, char *buf, size_t bufSize, const char *begStr, const char *endStr);
    static const char *GetDataStr(const char *inStr, const char *strOrder, ...);
    static uint32_t GetColorFromStr(const char *inStr, uint32_t dwDefColor);

    virtual void MoveMouseOutScreen(float fX, float fY)
    {
    }

    virtual bool CheckByToolTip(float fX, float fY);
    void ShowToolTip() const;

    XINTERFACE_BASE *ptrOwner;

    VDX9RENDER *m_rs;
    XYPOINT m_screenSize;
    XYRECT m_rect;
    XYRECT m_hostRect;

    bool m_bUse;
    bool m_bClickable;
    bool m_bLockStatus;
    bool m_bSelected;
    bool m_bBreakPress;
    bool m_bLockedNode;
    bool m_bMouseSelect;

    long m_nDoDelay;
    int m_nCurrentCommandNumber;

    int m_nNodeType;

    CINODE *m_next;
    CINODE *m_list;

    COMMAND_ACTION m_pCommands[COMMAND_QUANTITY];
    char *m_nodeName;

    XYPOINT m_MousePoint;

    VXSERVICE *pPictureService; // services pointer
    VSTRSERVICE *pStringService;

    // context help data
    char *m_strHelpTextureFile;
    FXYRECT m_frectHelpTextureUV;

    bool m_bInProcessingMessageForThisNode;

    bool m_bDeleting;

    CXI_ToolTip *m_pToolTip;

    bool m_bMakeActionInDeclick;
};
