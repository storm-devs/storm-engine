#pragma once

#include "matrix.h"
#include "nodes/xi_util.h"
#include "quest_file_reader/quest_file_reader.h"
#include "dx9render.h"
#include "editor/editor.h"
#include "inode.h"
#include "v_module_api.h"

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
    int32_t m_nInterfaceMode;
    uint32_t m_dwContHelpColor;
    int32_t m_idHelpTexture;
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
    int32_t m_nBlendStepMax;
    int32_t m_nBlendSpeed;
    XYRECT GlobalRect;
    FXYPOINT m_fpMouseOutZoneOffset;
    //-------------------------------------

    // mouse data
    bool m_bShowMouse;
    int32_t lock_x, lock_y; // center position for mouse pointer
    float fXMousePos, fYMousePos;
    int32_t m_lXMouse, m_lYMouse; // mouse pointer corrector for calculate active point
    POINT MouseSize;
    int32_t m_idTex;
    XI_ONLYONETEX_VERTEX vMouse[4];
    int32_t m_lMouseSensitive;
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

    void CreateNode(const char *sFileName, const char *sNodeType, const char *sNodeName, int32_t priority = 80);
    void SFLB_CreateNode(INIFILE *pOwnerIni, INIFILE *pUserIni, const char *sNodeType, const char *sNodeName,
                         int32_t priority);
    void DeleteNode(const char *pcNodeName);
    CINODE *NewNode(const char *pcNodType);

    void SetTooltip(const char *pcHeader, const char *pcText1, uint32_t dwTextColor1, const char *pcText2,
                    uint32_t dwTextColor2, const char *pcText3, uint32_t dwTextColor3, const char *pcText4,
                    uint32_t dwTextColor4, const char *pcPicTextureName, const char *pcPicGroupName,
                    const char *pcPicImageName, int32_t nPicWidth, int32_t nPicHeight);

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

    int32_t PrintIntoWindow(int32_t wl, int32_t wr, int32_t idFont, uint32_t dwFCol, uint32_t dwBCol, int32_t align, bool shadow,
                         float scale, int32_t sxs, int32_t sys, int32_t left, int32_t top, const char *str,
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
        int32_t nSaveCode;
        std::vector<CINODE *> aNode;

        LocksInfo()
        {
            nSaveCode = -1;
        }
    };

    std::vector<LocksInfo> m_aLocksArray;

    int32_t StoreNodeLocksWithOff();
    void RestoreNodeLocks(int32_t nStoreIdx);

    int32_t GetScreenWidth() const
    {
        return dwScreenWidth;
    }

    int32_t GetScreenHeight() const
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
    int32_t m_nPressDelay;
    int32_t m_nMaxPressDelay;

    int32_t m_nMouseLastClickTimeCur;
    int32_t m_nMouseLastClickTimeMax;

    bool m_bUse;

    // context help functions
    void GetContextHelpData();
    void ReleaseContextHelpData();
    void ShowContextHelp();

    // draw function
    void DrawNode(CINODE *nod, uint32_t Delta_Time, int32_t startPrior = 0, int32_t endPrior = 32000) const;
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
    CINODE *GetClickNode(CINODE *searchNod, int32_t xPos, int32_t yPos) const;
    // release function
    void ReleaseOld();
    void ReleaseDinamicPic(const char *sPicName);
    // save load functions
    bool SFLB_DoSaveFileData(const char *saveName, const char *saveData) const;
    bool SFLB_GetSaveFileData(const char *saveName, int32_t bufSize, char *buf);
    char *SaveFileFind(int32_t saveNum, char *buffer, size_t bufSize, int32_t &fileSize);
    bool NewSaveFileName(const char *fileName) const;
    void DeleteSaveFile(const char *fileName);
    // node control
    void AddNodeToList(CINODE *nod, int32_t priority = 80);
    // locked function
    void SetExclusiveNode(CINODE *nod);
    void AddExclusiveNode(CINODE *nod);
    void ExitFromExclusive();
    // Game time function
    void IncrementGameTime(uint32_t dwDeltaTime);
    // Options functions
    void SaveOptionsFile(const char *fileName, ATTRIBUTES *pAttr);
    void LoadOptionsFile(std::string_view filename, ATTRIBUTES *pAttr);
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
        int32_t file_size;
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
        int32_t idTexture;
        int32_t imageID;
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
        int32_t nCommandIndex;
        EVENT_Entity *next;

        EVENT_Entity()
            : next(nullptr)
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
    int32_t m_ChangeType;       // exchange type
    float m_fAngle;
    int32_t m_nBlendColor;
    IDirect3DTexture9 *m_pTexture;
    IDirect3DTexture9 *m_pPrevTexture;

    // vertex & index data
    int32_t vBuf, iBuf;
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
