// SetupDlg.h : header file
//

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include <ddraw.h>

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

#define VOLUME_MAX 1000
#define MAX_DEVICES 8
#define MAX_RESOLUTIONS 256
#define MAX_STRINGS 2048
#define NUM_FILES 2

struct file_t
{
    const char *pFile;
    FILE *file;
    BOOL bNoSpaces, bLn;
};

typedef struct
{
    DWORD width;
    DWORD height;
    DWORD bpp;
} res_t;

typedef struct
{
    wchar_t Name[256];
    DWORD numresolutions;
    DWORD currentresolution;
    res_t Resolutions[MAX_RESOLUTIONS];
} device_t;

typedef struct
{
    char *pName;
    char *pValue;
    DWORD dwFile;
} string_t;

class CSetupDlg : public CDialog
{
    // Construction
  public:
    DWORD numstrings;
    string_t Strings[MAX_STRINGS];

    DWORD currentdevice;
    DWORD numdevices;
    device_t Devices[MAX_DEVICES];
    file_t Files[NUM_FILES];

    CSetupDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CSetupDlg)
    enum
    {
        IDD = IDD_SETUP_DIALOG
    };
    CSliderCtrl m_DialogVol;
    CSliderCtrl m_TexQuality;
    CSliderCtrl m_SoundVol;
    CSliderCtrl m_MusicVol;
    CComboBox m_ResList;
    CComboBox m_DeviceList;
    BOOL m_Fullscreen;
    BOOL m_PostProcess;
    CString m_TexQualityInfo;
    // BOOL    m_bArcadeMode;
    BOOL m_bSafeRender;
    BOOL m_bSndMusOn;
    int m_iTexQuality;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSetupDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
    //}}AFX_VIRTUAL

    // Implementation

  protected:
    BOOL OnToolTipNotify(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);

    BOOL EnumerateDevicesAndResolutions();
    BOOL LoadSettings();

    void UpdateResolutions();
    void SaveParameters();
    void Trace(const char *data_PTR, ...);

    HICON m_hIcon;
    wchar_t InstallLocationW[2048], InstallLocationExeW[2048];

    // Generated message map functions
    //{{AFX_MSG(CSetupDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDefaultsettings();
    afx_msg void OnSave();
    afx_msg void OnSelchangeDevicelist();
    afx_msg void OnSelchangeResolutionlist();
    afx_msg void OnDestroy();
    afx_msg void OnStartgame();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnTtnTooltipShowCustom1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedFullscreen();
    afx_msg void OnBnClickedGlow();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
