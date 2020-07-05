#ifndef _XINTERFACE_H_
#define _XINTERFACE_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\matrix.h"
#include "inode.h"
#include "..\common_h\vmodule_api.h"
#include "QuestFileReader\\QuestFileReader.h"
#include "nodes\xi_util.h"
#include "..\common_h\templates\string.h"
#include "editor\editor.h"

class CXI_WINDOW;

class XINTERFACE : public XINTERFACE_BASE
{
public:
	static XINTERFACE * pThis;

protected:
	QUEST_FILE_READER *	pQuestService;
	VXSERVICE *			pPictureService;
	VSTRSERVICE *		pStringService;
	VDX8RENDER *		pRenderService;

	CXI_UTILS m_UtilContainer;

	bool bActive;
	bool bDisableControl;

	// context help
	long	m_nInterfaceMode;
	DWORD	m_dwContHelpColor;
	long	m_idHelpTexture;
	FXYRECT	m_frectHelpTextureUV;
	char *	m_strDefHelpTextureFile;
	FXYRECT	m_frectDefHelpTextureUV;

    // параметры получаемые из INI-файла //
    //-------------------------------------
	float   fScale;
	XYRECT  GlobalScreenRect;
	DWORD   dwScreenWidth;
	DWORD   dwScreenHeight;
	int		m_nColumnQuantity; // Column quantity for grid of the previouse texture
	float	m_fWaveAmplitude;
	float   m_fWavePhase;
	float	m_fWaveSpeed;
	long	m_nBlendStepMax;
	long	m_nBlendSpeed;
	XYRECT  GlobalRect;
	FXYPOINT m_fpMouseOutZoneOffset;
    //-------------------------------------

	// mouse data
	bool m_bShowMouse;
	long lock_x,lock_y; // center position for mouse pointer
	float fXMousePos, fYMousePos;
	long  m_lXMouse, m_lYMouse; // mouse pointer corrector for calculate active point
	POINT MouseSize;
	long m_idTex;
	XI_ONLYONETEX_VERTEX vMouse[4];
	long m_lMouseSensitive;
	bool m_bMouseClick;
	bool m_bDblMouseClick;
    int  m_idButton;
	CINODE * m_pMouseNode;
	VDATA * m_pMouseWeel;

	// save render state parameters
	DWORD m_dwStoreFlag_Fog;

public:
	 XINTERFACE();
	~XINTERFACE();

	void SetDevice();
	bool Init();
    void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);
	bool CreateState(ENTITY_STATE_GEN * state_gen);
	bool LoadState(ENTITY_STATE * state);
    dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES* patr);

	void CreateNode(char *sFileName, char *sNodeType, char *sNodeName, long priority=80);
	void __declspec(dllexport) __cdecl SFLB_CreateNode(INIFILE* pOwnerIni, INIFILE* pUserIni, char *sNodeType, char *sNodeName, long priority);
	void DeleteNode(const char *pcNodeName);
	CINODE* NewNode(const char* pcNodType);

	void SetTooltip( const char* pcHeader,
		const char* pcText1,dword dwTextColor1,
		const char* pcText2,dword dwTextColor2,
		const char* pcText3,dword dwTextColor3,
		const char* pcText4,dword dwTextColor4,
		const char* pcPicTextureName,const char* pcPicGroupName,const char* pcPicImageName,
		long nPicWidth,long nPicHeight );

	static QUEST_FILE_READER *	GetQuestFileReader() {return pThis->pQuestService;}
	static VXSERVICE *			GetPictureService() {return pThis->pPictureService;}
	static VSTRSERVICE *		GetStringService() {return pThis->pStringService;}
	static VDX8RENDER *			GetRenderService() {return pThis->pRenderService;}

	virtual QUEST_FILE_READER * QuestFileReader() {return pQuestService;}
	virtual VXSERVICE *			PictureService() {return pPictureService;}
	virtual VSTRSERVICE *		StringService() {return pStringService;}
	virtual VDX8RENDER *		RenderService() {return pRenderService;}

	void *						GetCurrentNode() {return m_pCurNode;}
	FXYPOINT					GetMousePoint() {return FXYPOINT(fXMousePos+m_lXMouse,fYMousePos+m_lYMouse);}
	long						PrintIntoWindow(long wl,long wr, long idFont, DWORD dwFCol, DWORD dwBCol, long align, bool shadow, float scale, long sxs, long sys, long left, long top, char * str, int nWidthForScaleCorrecting=-1, int nSplit=0);

	virtual CINODE *			FindNode(const char * sNodeName, CINODE * findRoot) {if(!findRoot) findRoot=m_pNodes; if(findRoot) return findRoot->FindNode(sNodeName); return 0;}
	virtual void				ShowWindow( const char* pcWindowName, bool bShow );
	virtual void				DisableWindow( const char* pcWindowName, bool bDisable );
	virtual void				AddNodeToWindow( const char* pcNodeName, const char* pcWindowName );
	virtual CXI_WINDOW *		FindWindow(const char* pcWindowName);
	virtual bool				IsWindowActive(const char* pcWindowName);

	virtual void				RegistryExitKey( const char* pcKeyName );

	array<string> m_asExitKey;

	struct LocksInfo
	{
		long nSaveCode;
		array<CINODE*> aNode;
		LocksInfo() : aNode(_FL) {nSaveCode=-1;}
	};
	array<LocksInfo> m_aLocksArray;

	long	StoreNodeLocksWithOff();
	void	RestoreNodeLocks(long nStoreIdx);

	long	GetScreenWidth() {return dwScreenWidth;}
	long	GetScreenHeight() {return dwScreenHeight;}

protected:
	// WORLD,VIEW and PROJECTION matrixes
    CMatrix matw,matv,matp;

	CINODE		*m_pCurNode;
	CINODE		*m_pNodes;
    CINODE      *m_pContHelp;
    CINODE      *m_pMonocracyNode;
	GIEditor	*m_pEditor;
	bool		m_bNotFirstPress;
	long		m_nPressDelay;
	long		m_nMaxPressDelay;

	long		m_nMouseLastClickTimeCur;
	long		m_nMouseLastClickTimeMax;

    bool        m_bUse;

	// context help functions
	void GetContextHelpData();
	void ReleaseContextHelpData();
	void ShowContextHelp();

	// draw function
	void		DrawNode(CINODE* nod,dword Delta_Time, long startPrior=0, long endPrior=32000);
	void		ShowPrevTexture();
	// initialisation function
    void		LoadIni();
	void		LoadDialog(char *sFileName);
	// control function
	CINODE*		GetActivingNode(CINODE *findRoot);
	void		DoControl();
    bool        SetCurNode(CINODE *pNod);
	// mouse function
	void		MouseMove();
	void		MouseClick(bool bFirstClick);
	void		MouseDeClick();
	CINODE *    GetClickNode(CINODE * searchNod, long xPos, long yPos);
	// release function
	void		ReleaseOld();
	void		ReleaseDinamicPic(char * sPicName);
	// save load functions
	bool __declspec(dllexport) __cdecl SFLB_DoSaveFileData(char * saveName, char * saveData);
	bool __declspec(dllexport) __cdecl SFLB_GetSaveFileData(char * saveName, long bufSize, char * buf);
	char *		SaveFileFind(long saveNum, char * buffer, size_t bufSize, long & fileSize);
	bool		NewSaveFileName(char * fileName);
	void		DeleteSaveFile(char * fileName);
	// node control
	void		AddNodeToList(CINODE * nod,long priority=80);
	// locked function
	void		SetExclusiveNode(CINODE * nod);
	void		AddExclusiveNode(CINODE * nod);
	void		ExitFromExclusive();
	// cript function
	void		SetOtherData(char * cDat);
	// Game time function
	void		IncrementGameTime(DWORD dwDeltaTime);
	// Options functions
	void		SaveOptionsFile(char * fileName, ATTRIBUTES * pAttr);
	void		LoadOptionsFile(char * fileName, ATTRIBUTES * pAttr);
	//
	int			LoadIsExist();
	//
	void		PrecreateDirForFile(const char* pcFullFileName);

	DWORD	m_dwCurDeltaTime;
	int		m_dwGameTimeSec;
	int		m_dwGameTimeMin;
	int		m_dwGameTimeHour;

	union KEYSTATE
	{
		int dwKeyCode;
		struct
		{
			unsigned rightButton	: 1;
			unsigned leftButton		: 1;
			unsigned upButton		: 1;
			unsigned downButton		: 1;
			unsigned enterButton	: 1;
			unsigned backButton		: 1;
			unsigned shiftButton	: 1;
		};
	};
	KEYSTATE	oldKeyState;

	// save find data
	struct SAVE_FIND_DATA
	{
		char *		save_file_name;
		long		file_size;
		FILETIME	time;

		SAVE_FIND_DATA * next;
	};
	SAVE_FIND_DATA * m_pSaveFindRoot;
	void ReleaseSaveFindList();
	void AddFindData(char * sSaveFileName, long file_size, FILETIME create_time);
	void Sorting_FindData();
	SAVE_FIND_DATA * GetSaveDataByIndex(int n);

	// dinamic strings data
	struct STRING_ENTITY
	{
		bool	bUsed;
		int		fontNum;
		DWORD   dwColor;
		char *	sStringName;
		int		x,y;
		int		eAlignment;
		float	fScale;
	};
	STRING_ENTITY *m_stringes;
	int		m_nStringQuantity;

	// dinamic images data
	struct IMAGE_ENTITY
	{
		XYRECT			position;
		char			*sImageListName;
		char			*sImageName;
		char			*sPicture;
		char			*sTechniqueName;
		long			idTexture;
		long			imageID;
		bool			doBlind;
		DWORD			argbBlindMin;
		DWORD			argbBlindMax;
		IMAGE_ENTITY	*next;
	};
	IMAGE_ENTITY *	m_imgLists;

    // send events data
    struct EVENT_ENTITY
    {
        char* sEventName;
		char* sNodeName;
		long nCommandIndex;
        EVENT_ENTITY * next;

		EVENT_ENTITY() {sEventName=0; sNodeName=0; nCommandIndex=0;}
		~EVENT_ENTITY() {PTR_DELETE(sEventName); PTR_DELETE(sNodeName); nCommandIndex=0;}
    };
    EVENT_ENTITY * m_pEvents;

	// previouse texture & draw to texturer data
	bool	m_bShowPrevTexture; // exchange one interface to other
	long	m_ChangeType;		// exchange type
	float	m_fAngle;
	long    m_nBlendColor;
	IDirect3DTexture8* m_pTexture;
	IDirect3DTexture8* m_pPrevTexture;

	// vertex & index data
    long vBuf,iBuf;
    DWORD nVert,nIndx;

	// blind parameters
	float	m_fBlindFactor;
	float	m_fBlindSpeed;
	bool	m_bBlindUp;

	CINODE * m_pGlowCursorNode;

	CINODE * m_pCurToolTipNode;
};

class CONTROLS_CONTAINER : public ENTITY
{
	struct CONTEINER_DESCR {
		float fMaxVal;
		char * resultName;

		struct CONTROL_DESCR {
			float fValLimit;
			char * controlName;

			CONTROL_DESCR * next;
		} * pControls;

		CONTEINER_DESCR * next;

		CONTROL_DESCR * FindControl(char * cntrlName);
	} * pContainers;

public:
	CONTROLS_CONTAINER();
	~CONTROLS_CONTAINER();
	bool Init();
	void Execute(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	bool CreateConteinerList(ATTRIBUTES * pA);
	void AddContainer( char * container );
	void SetContainerLimitVal( char * container, float fLimitVal );
	void AddControlsToContainer( char * container, char * controlName, float fValLimit );

	CONTEINER_DESCR * FindContainer(char * sContainer);
};

/*#ifndef _XBOX
API_MODULE_START("xinterface")
	CREATE_CLASS(0,XINTERFACE)
API_MODULE_END
#endif*/

#endif