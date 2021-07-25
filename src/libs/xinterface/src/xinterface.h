#pragma once

#include "Matrix.h"
#include "Nodes/xi_util.h"
#include "QuestFileReader//QuestFileReader.h"
#include "dx9render.h"
#include "editor/editor.h"
#include "inode.h"
#include "vmodule_api.h"

#include <filesystem>

class CXI_WINDOW;

class XINTERFACE : public XINTERFACE_BASE
{
  public:
    static XINTERFACE *pThis;

  protected:
    storm::QuestFileReader *pQuestService;
    VXSERVICE *pPictureService;
    VSTRSERVICE *pStringService;
    VDX9RENDER *pRenderService;

    CXI_UTILS m_UtilContainer;

    bool bActive;
    bool bDisableControl;

    // context help
    long m_nInterfaceMode;
    uint32_t m_dwContHelpColor;
    long m_idHelpTexture;
    FXYRECT m_frectHelpTextureUV;
    char *m_strDefHelpTextureFile;
    FXYRECT m_frectDefHelpTextureUV;

    // parameters obtained from INI file //
    // -------------------------------------
    float fScale;
    XYRECT GlobalScreenRect;
    uint32_t dwScreenWidth;
    uint32_t dwScreenHeight;
    int m_nColumnQuantity; // Column quantity for grid of the previouse texture
    float m_fWaveAmplitude;
    float m_fWavePhase;
    float m_fWaveSpeed;
    long m_nBlendStepMax;
    long m_nBlendSpeed;
    XYRECT GlobalRect;
    FXYPOINT m_fpMouseOutZoneOffset;
    //-------------------------------------

    // mouse data
    bool m_bShowMouse;
    long lock_x, lock_y; // center position for mouse pointer
    float fXMousePos, fYMousePos;
    long m_lXMouse, m_lYMouse; // mouse pointer corrector for calculate active point
    POINT MouseSize;
    long m_idTex;
    XI_ONLYONETEX_VERTEX vMouse[4];
    long m_lMouseSensitive;
    bool m_bMouseClick;
    bool m_bDblMouseClick;
    int m_idButton;
    CINODE *m_pMouseNode;
    VDATA *m_pMouseWeel;

    // save render state parameters
    uint32_t m_dwStoreFlag_Fog;

  public:
    XINTERFACE(XINTERFACE &&) = delete;
    XINTERFACE(const XINTERFACE &) = delete;
    XINTERFACE();
    ~XINTERFACE() override;

    void SetDevice();
    bool Init() override;
    void Execute(uint32_t Delta_Time);
    void Realize(uint32_t Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *patr) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    void CreateNode(const char *sFileName, const char *sNodeType, const char *sNodeName, long priority = 80);
    void SFLB_CreateNode(INIFILE *pOwnerIni, INIFILE *pUserIni, const char *sNodeType, const char *sNodeName,
                         long priority);
    void DeleteNode(const char *pcNodeName);
    CINODE *NewNode(const char *pcNodType);

    void SetTooltip(const char *pcHeader, const char *pcText1, uint32_t dwTextColor1, const char *pcText2,
                    uint32_t dwTextColor2, const char *pcText3, uint32_t dwTextColor3, const char *pcText4,
                    uint32_t dwTextColor4, const char *pcPicTextureName, const char *pcPicGroupName,
                    const char *pcPicImageName, long nPicWidth, long nPicHeight);

    static storm::QuestFileReader *GetQuestFileReader()
    {
        return pThis->pQuestService;
    }

    static VXSERVICE *GetPictureService()
    {
        return pThis->pPictureService;
    }

    static VSTRSERVICE *GetStringService()
    {
        return pThis->pStringService;
    }

    static VDX9RENDER *GetRenderService()
    {
        return pThis->pRenderService;
    }

    storm::QuestFileReader *QuestFileReader() override
    {
        return pQuestService;
    }

    VXSERVICE *PictureService() override
    {
        return pPictureService;
    }

    VSTRSERVICE *StringService() override
    {
        return pStringService;
    }

    VDX9RENDER *RenderService() override
    {
        return pRenderService;
    }

    void *GetCurrentNode() override
    {
        return m_pCurNode;
    }

    FXYPOINT GetMousePoint() override
    {
        return FXYPOINT(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse);
    }

    long PrintIntoWindow(long wl, long wr, long idFont, uint32_t dwFCol, uint32_t dwBCol, long align, bool shadow,
                         float scale, long sxs, long sys, long left, long top, const char *str,
                         int nWidthForScaleCorrecting = -1, int nSplit = 0) override;

    CINODE *FindNode(const char *sNodeName, CINODE *findRoot) override
    {
        if (!findRoot)
            findRoot = m_pNodes;
        if (findRoot)
            return findRoot->FindNode(sNodeName);
        return nullptr;
    }

    void ShowWindow(const char *pcWindowName, bool bShow) override;
    void DisableWindow(const char *pcWindowName, bool bDisable) override;
    void AddNodeToWindow(const char *pcNodeName, const char *pcWindowName) override;
    virtual CXI_WINDOW *FindWindow(const char *pcWindowName);
    virtual bool IsWindowActive(const char *pcWindowName);

    void RegistryExitKey(const char *pcKeyName) override;

    std::vector<std::string> m_asExitKey;

    struct LocksInfo
    {
        long nSaveCode;
        std::vector<CINODE *> aNode;

        LocksInfo()
        {
            nSaveCode = -1;
        }
    };

    std::vector<LocksInfo> m_aLocksArray;

    long StoreNodeLocksWithOff();
    void RestoreNodeLocks(long nStoreIdx);

    long GetScreenWidth() const
    {
        return dwScreenWidth;
    }

    long GetScreenHeight() const
    {
        return dwScreenHeight;
    }

  protected:
    // WORLD,VIEW and PROJECTION matrixes
    CMatrix matw, matv, matp;

    CINODE *m_pCurNode;
    CINODE *m_pNodes;
    CINODE *m_pContHelp;
    CINODE *m_pMonocracyNode;
    GIEditor *m_pEditor;
    bool m_bNotFirstPress;
    long m_nPressDelay;
    long m_nMaxPressDelay;

    long m_nMouseLastClickTimeCur;
    long m_nMouseLastClickTimeMax;

    bool m_bUse;

    // context help functions
    void GetContextHelpData();
    void ReleaseContextHelpData();
    void ShowContextHelp();

    // draw function
    void DrawNode(CINODE *nod, uint32_t Delta_Time, long startPrior = 0, long endPrior = 32000) const;
    void ShowPrevTexture();
    // initialisation function
    void LoadIni();
    void LoadDialog(const char *sFileName);
    // control function
    CINODE *GetActivingNode(CINODE *findRoot);
    void DoControl();
    bool SetCurNode(CINODE *pNod);
    // mouse function
    void MouseMove();
    void MouseClick(bool bFirstClick);
    void MouseDeClick();
    CINODE *GetClickNode(CINODE *searchNod, long xPos, long yPos) const;
    // release function
    void ReleaseOld();
    void ReleaseDinamicPic(const char *sPicName);
    // save load functions
    bool SFLB_DoSaveFileData(const char *saveName, const char *saveData) const;
    bool SFLB_GetSaveFileData(const char *saveName, long bufSize, char *buf);
    char *SaveFileFind(long saveNum, char *buffer, size_t bufSize, long &fileSize);
    bool NewSaveFileName(const char *fileName) const;
    void DeleteSaveFile(const char *fileName);
    // node control
    void AddNodeToList(CINODE *nod, long priority = 80);
    // locked function
    void SetExclusiveNode(CINODE *nod);
    void AddExclusiveNode(CINODE *nod);
    void ExitFromExclusive();
    // Game time function
    void IncrementGameTime(uint32_t dwDeltaTime);
    // Options functions
    void SaveOptionsFile(const char *fileName, ATTRIBUTES *pAttr);
    void LoadOptionsFile(const char *fileName, ATTRIBUTES *pAttr);
    //
    int LoadIsExist();
    //
    void PrecreateDirForFile(const char *pcFullFileName);

    uint32_t m_dwCurDeltaTime;
    int m_dwGameTimeSec;
    int m_dwGameTimeMin;
    int m_dwGameTimeHour;

    union KEYSTATE {
        int dwKeyCode;

        struct
        {
            unsigned rightButton : 1;
            unsigned leftButton : 1;
            unsigned upButton : 1;
            unsigned downButton : 1;
            unsigned enterButton : 1;
            unsigned backButton : 1;
            unsigned shiftButton : 1;
        };
    };

    KEYSTATE oldKeyState;

    // save find data
    struct SAVE_FIND_DATA
    {
        char *save_file_name;
        long file_size;
        std::filesystem::file_time_type time;

        SAVE_FIND_DATA *next;
    };

    SAVE_FIND_DATA *m_pSaveFindRoot;
    void ReleaseSaveFindList();
    void AddFindData(std::filesystem::path filePath);
    void Sorting_FindData();
    SAVE_FIND_DATA *GetSaveDataByIndex(int n) const;

    // dinamic strings data
    struct STRING_Entity
    {
        bool bUsed;
        int fontNum;
        uint32_t dwColor;
        char *sStringName;
        int x, y;
        int eAlignment;
        float fScale;
    };

    std::vector<STRING_Entity> m_stringes;
    int m_nStringQuantity;

    // dynamic images data
    struct IMAGE_Entity
    {
        XYRECT position;
        char *sImageListName;
        char *sImageName;
        char *sPicture;
        char *sTechniqueName;
        long idTexture;
        long imageID;
        bool doBlind;
        uint32_t argbBlindMin;
        uint32_t argbBlindMax;
        IMAGE_Entity *next;
    };

    IMAGE_Entity *m_imgLists;

    // send events data
    struct EVENT_Entity
    {
        char *sEventName;
        char *sNodeName;
        long nCommandIndex;
        EVENT_Entity *next;

        EVENT_Entity()
        {
            sEventName = nullptr;
            sNodeName = nullptr;
            nCommandIndex = 0;
        }

        ~EVENT_Entity()
        {
            STORM_DELETE(sEventName);
            STORM_DELETE(sNodeName);
            nCommandIndex = 0;
        }
    };

    EVENT_Entity *m_pEvents;

    // previouse texture & draw to texturer data
    bool m_bShowPrevTexture; // exchange one interface to other
    long m_ChangeType;       // exchange type
    float m_fAngle;
    long m_nBlendColor;
    IDirect3DTexture9 *m_pTexture;
    IDirect3DTexture9 *m_pPrevTexture;

    // vertex & index data
    long vBuf, iBuf;
    uint32_t nVert, nIndx;

    // blind parameters
    float m_fBlindFactor;
    float m_fBlindSpeed;
    bool m_bBlindUp;

    CINODE *m_pGlowCursorNode;

    CINODE *m_pCurToolTipNode;
};

class CONTROLS_CONTAINER : public Entity
{
    struct CONTEINER_DESCR
    {
        float fMaxVal;
        char *resultName;

        struct CONTROL_DESCR
        {
            float fValLimit;
            char *controlName;

            CONTROL_DESCR *next;
        } * pControls;

        CONTEINER_DESCR *next;

        CONTROL_DESCR *FindControl(const char *cntrlName);
    } * pContainers;

  public:
    CONTROLS_CONTAINER();
    ~CONTROLS_CONTAINER() override;
    bool Init() override;
    void Execute(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
            // case Stage::realize:
            //    Realize(delta); break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

  protected:
    bool CreateConteinerList(ATTRIBUTES *pA);
    void AddContainer(const char *container);
    void SetContainerLimitVal(const char *container, float fLimitVal);
    void AddControlsToContainer(const char *container, const char *controlName, float fValLimit);

    CONTEINER_DESCR *FindContainer(const char *sContainer);
};
