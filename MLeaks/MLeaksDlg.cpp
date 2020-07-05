// MLeaksDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MLeaks.h"
#include "MLeaksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

char* SkipToken(char* str, char* cmp) 
{
	int flag;
	char *str1,*cmp1;

	if (str==0 || cmp==0) return 0;
	cmp1 = cmp;

	if ((str == NULL)||(cmp == NULL)) return NULL;
	if (strlen(str)==0) return NULL;

	int lens = strlen(str);
	int lenc = strlen(cmp);
//	if (strcmp(str,cmp) != 0) return NULL;
	for (int i=0;i<lens;i++)
	{
		flag = 0;
		str1 = str;
		if (*str == *cmp)
		{
			flag = 1;
			for (int j=0;j<lenc;j++)
				if (*str++ != *cmp++) {	flag = 0;	break;	}
			if (!flag)
			{
				cmp = cmp1;
				str = str1;
			}
		}
		if (flag) break;
		str++;
	}
	if (flag) 
	{
		while (*str == 32) str++;
		return str;
	}
	return NULL;
}

char* FindToken(char* str,char* cmp) 
{
	int flag;
	char *str1,*cmp1;

	if (str==0 || cmp==0) return 0;
	cmp1 = cmp;

	if ((str == NULL)||(cmp == NULL)) return NULL;
	if (strlen(str)==0) return NULL;

	int lens = strlen(str);
	int lenc = strlen(cmp);
//	if (strcmp(str,cmp) != 0) return NULL;
	for (int i=0;i<lens;i++)
	{
		flag = 0;
		str1 = str;
		if (*str == *cmp)
		{
			flag = 1;
			for (int j=0;j<lenc;j++)
				if (*str++ != *cmp++) {	flag = 0;	break;	}
			if (!flag)
			{
				cmp = cmp1;
				str = str1;
			}
		}
		if (flag) break;
		str++;
	}
	if (flag) return str;
	return NULL;
}

// Get token before symbol skip
char*	GetTokenWhile(char* src, char* dst, char* skip)
{
	if (!src || !dst || !skip) return 0;
	dst[0] = 0;
	char *dst1 = dst;
	long lensrc = strlen(src);
	long dstlen = 0;
	while (lensrc && src[0]==' ') { src++; lensrc--; };
	while ((lensrc--)&&(*src!=*skip))
	{
		*dst1++ = *src++;
		dstlen++;
	}
	*dst1++ = 0;
	char *pRet = ((lensrc>0) ? ++src : src); if (lensrc) lensrc--;
	while (lensrc && pRet[0]==32) { pRet++; lensrc--; };
	while (dstlen && dst[dstlen-1]==' ') { dstlen--; dst[dstlen] = 0; }
	return pRet;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMLeaksDlg dialog

CMLeaksDlg::CMLeaksDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMLeaksDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMLeaksDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMLeaksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMLeaksDlg)
	DDX_Control(pDX, IDC_LEAKS, m_Leaks);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMLeaksDlg, CDialog)
	//{{AFX_MSG_MAP(CMLeaksDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_COMPARE, OnCompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMLeaksDlg message handlers

BOOL CMLeaksDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Leaks.InsertColumn(0, "File", LVCFMT_LEFT, 296);
	m_Leaks.InsertColumn(1, "Blocks", LVCFMT_RIGHT, 80);
	m_Leaks.InsertColumn(2, "Memory", LVCFMT_RIGHT, 130);
	m_Leaks.InsertColumn(3, "Delta", LVCFMT_RIGHT, 70);

	m_Leaks.SetExtendedStyle(m_Leaks.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMLeaksDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMLeaksDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMLeaksDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMLeaksDlg::OnLoad() 
{
	char str[2048], temp_str[2048], temp_str2[2048];
	// TODO: Add your control notification handler code here
	m_Leaks.DeleteAllItems();	
	CFileDialog fd(true, NULL, NULL);
	fd.DoModal();
	CString sFile = fd.GetPathName();
	dwTmp = 0;
	FILE * f = fopen(sFile, "r");
	if (f)
	{
		while (!feof(f))
		{
			temp_str[0] = 0;
			fgets(temp_str, 2048, f);
			fgets(&temp_str[strlen(temp_str) - 1], 2048, f);
			if (temp_str[0] == 0) continue;
			GetTokenWhile(SkipToken(temp_str, "["), Tmp[dwTmp].sFile, "]");
			GetTokenWhile(SkipToken(temp_str, "memory  "), temp_str2, "k");
			sscanf(temp_str2, "%f", &Tmp[dwTmp].fSize);
			GetTokenWhile(SkipToken(temp_str, " in "), temp_str2, "b");
			sscanf(temp_str2, "%f", &Tmp[dwTmp].fBlocks);
			Tmp[dwTmp].fDelta = 0.0f;

			m_Leaks.InsertItem(dwTmp, Tmp[dwTmp].sFile, 0);
			
			sprintf(str, "%.0f", Tmp[dwTmp].fBlocks);
			m_Leaks.SetItem(dwTmp, 1, LVIF_TEXT, str, 0, 0, 0, 0);

			sprintf(str, "%.3f", Tmp[dwTmp].fSize);
			m_Leaks.SetItem(dwTmp, 2, LVIF_TEXT, str, 0, 0, 0, 0);

			m_Leaks.SetItem(dwTmp, 3, LVIF_TEXT, "0.0", 0, 0, 0, 0);

			dwTmp++;
		}
	} 
	fclose(f);
}

void CMLeaksDlg::OnCompare() 
{
	tmp_t tmp;
	// TODO: Add your control notification handler code here
	char str[2048], temp_str[2048], temp_str2[2048];
	// TODO: Add your control notification handler code here
	//m_Leaks.DeleteAllItems();	
	CFileDialog fd(true, NULL, NULL);
	fd.DoModal();
	CString sFile = fd.GetPathName();
	FILE * f = fopen(sFile, "r");
	if (f)
	{
		while (!feof(f))
		{
			temp_str[0] = 0;
			fgets(temp_str, 2048, f);
			fgets(&temp_str[strlen(temp_str) - 1], 2048, f);
			if (temp_str[0] == 0) continue;
			GetTokenWhile(SkipToken(temp_str, "["), tmp.sFile, "]");
			GetTokenWhile(SkipToken(temp_str, "memory  "), temp_str2, "k");
			sscanf(temp_str2, "%f", &tmp.fSize);
			GetTokenWhile(SkipToken(temp_str, " in "), temp_str2, "b");
			sscanf(temp_str2, "%f", &tmp.fBlocks);

			for (dword i=0; i<dwTmp; i++) if (stricmp(tmp.sFile, Tmp[i].sFile) == 0) break;

			if (i == dwTmp)
			{
				m_Leaks.InsertItem(dwTmp, tmp.sFile, 0);
				
				sprintf(str, "%.0f", tmp.fBlocks);
				m_Leaks.SetItem(dwTmp, 1, LVIF_TEXT, str, 0, 0, 0, 0);

				sprintf(str, "%.3f", tmp.fSize);
				m_Leaks.SetItem(dwTmp, 2, LVIF_TEXT, str, 0, 0, 0, 0);

				m_Leaks.SetItem(dwTmp, 3, LVIF_TEXT, "0.0", 0, 0, 0, 0);

				dwTmp++;
			}
			else
			{
				Tmp[i].fDelta = tmp.fSize - Tmp[i].fSize;
				sprintf(str, "%.0f / %.0f", Tmp[i].fBlocks, tmp.fBlocks);
				m_Leaks.SetItem(i, 1, LVIF_TEXT, str, 0, 0, 0, 0);
				sprintf(str, "%.3f / %.3f", Tmp[i].fSize, tmp.fSize);
				m_Leaks.SetItem(i, 2, LVIF_TEXT, str, 0, 0, 0, 0);
				sprintf(str, "%.3f", Tmp[i].fDelta);
				m_Leaks.SetItem(i, 3, LVIF_TEXT, str, 0, 0, 0, 0);
			}
		}
	} 
	fclose(f);
	
}
