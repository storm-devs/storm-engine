// SetupDlg.cpp : implementation file
//
#include "stdafx.h"

#include "Setup.h"
#include "SetupDlg.h"

#include ".\setupdlg.h"
#include <ddraw.h>
#include <math.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

namespace
{
std::string ConvertWideToUtf8(const std::wstring &wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring ConvertUtf8ToWide(const std::string &str)
{
    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}
} // namespace

class CAboutDlg : public CDialog
{
  public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
  protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
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

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
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
// CSetupDlg dialog

CSetupDlg::CSetupDlg(CWnd *pParent /*=NULL*/) : CDialog(CSetupDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSetupDlg)
    m_Fullscreen = false;
    m_PostProcess = false;
    m_TexQualityInfo = _T("");
    //    m_bArcadeMode = FALSE;
    m_bSafeRender = FALSE;
    m_bSndMusOn = FALSE;
    m_iTexQuality = 2;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSetupDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetupDlg)
    DDX_Control(pDX, IDC_DIALOG_VOL, m_DialogVol);
    DDX_Control(pDX, IDC_TEXTURE_QUALITY, m_TexQuality);
    DDX_Control(pDX, IDC_SOUND_VOL, m_SoundVol);
    DDX_Control(pDX, IDC_MUSIC_VOL, m_MusicVol);
    DDX_Control(pDX, IDC_RESOLUTIONLIST, m_ResList);
    DDX_Control(pDX, IDC_DEVICELIST, m_DeviceList);
    DDX_Check(pDX, IDC_FULLSCREEN, m_Fullscreen);
    DDX_Check(pDX, IDC_GLOW, m_PostProcess);
    DDX_Text(pDX, IDC_TEXQUALITY_INFO, m_TexQualityInfo);
    // DDX_Check(pDX, IDC_ARCADE_MODE, m_bArcadeMode);
    DDX_Check(pDX, IDC_SAFE_RENDER, m_bSafeRender);
    DDX_Check(pDX, IDC_SND_MUS_ON, m_bSndMusOn);
    DDX_Slider(pDX, IDC_TEXTURE_QUALITY, m_iTexQuality);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
//{{AFX_MSG_MAP(CSetupDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_DEFAULTSETTINGS, &CSetupDlg::OnDefaultsettings)
ON_BN_CLICKED(IDOK, &CSetupDlg::OnSave)
ON_CBN_SELCHANGE(IDC_DEVICELIST, &CSetupDlg::OnSelchangeDevicelist)
ON_CBN_SELCHANGE(IDC_RESOLUTIONLIST, &CSetupDlg::OnSelchangeResolutionlist)
ON_WM_DESTROY()
ON_BN_CLICKED(ID_STARTGAME, &CSetupDlg::OnStartgame)
ON_WM_HSCROLL()
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &CSetupDlg::OnToolTipNotify)
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, &CSetupDlg::OnToolTipNotify)
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_FULLSCREEN, &CSetupDlg::OnBnClickedFullscreen)
ON_BN_CLICKED(IDC_GLOW, &CSetupDlg::OnBnClickedGlow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

void CSetupDlg::Trace(const char *data_PTR, ...)
{
    char buffer[4096];

    va_list args;
    va_start(args, data_PTR);
    _vsnprintf(buffer, sizeof(buffer) - 4, data_PTR, args);
    va_end(args);
    strcat(buffer, "\x0d\x0a");

    std::wstring wstr = ConvertUtf8ToWide(buffer);
    OutputDebugString(wstr.c_str());

    HANDLE file_h =
        CreateFile(TEXT("config.log"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    SetFilePointer(file_h, 0, 0, FILE_END);
    DWORD bytes;
    WriteFile(file_h, buffer, strlen(buffer), &bytes, 0);
    CloseHandle(file_h);
    _flushall();
}

BOOL CSetupDlg::OnInitDialog()
{
    wchar_t txt[256];

    DeleteFile(TEXT("config.log"));

    Trace("OnInitDialog::Start");
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    Trace("OnInitDialog::GetSystemMenu");
    CMenu *pSysMenu = GetSystemMenu(false);
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
    SetIcon(m_hIcon, true);  // Set big icon
    SetIcon(m_hIcon, false); // Set small icon

    Trace("OnInitDialog::EnableToolTips");
    // EnableToolTips(true);
    // TODO: Add extra initialization here

    Trace("OnInitDialog::EnumerateDevicesAndResolutions");
    if (!EnumerateDevicesAndResolutions())
    {
        LoadString(AfxGetResourceHandle(), IDS_INITERROR, &txt[0], 256);
        MessageBox(txt, TEXT("Error"), MB_OK);
        SendMessage(WM_CLOSE, 0, 0);
        return false;
    }

    Trace("OnInitDialog::LoadSettings");
    if (!LoadSettings())
    {
        LoadString(AfxGetResourceHandle(), IDS_INIMISSED, &txt[0], 256);
        MessageBox(txt, TEXT("Error"), MB_OK);
        SendMessage(WM_CLOSE, 0, 0);
        return false;
    }

    Trace("OnInitDialog::End");
    return true; // return true  unless you set the focus to a control
}

void CSetupDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    Trace("OnSysCommand::Start");
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
    Trace("OnSysCommand::End");
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSetupDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
HCURSOR CSetupDlg::OnQueryDragIcon()
{
    return (HCURSOR)m_hIcon;
}

BOOL CSetupDlg::LoadSettings()
{
    DWORD i, j;
    // HKEY    hKy;
    // DWORD    pData;
    int aTexQuality[3] = {IDS_TEXQUALITY_LOW, IDS_TEXQUALITY_MEDIUM, IDS_TEXQUALITY_HIGH};

    Trace("LoadSettings::Start");

    Trace("LoadSettings::GetCurrentDirectory");
    GetCurrentDirectory(sizeof(InstallLocationW) - 1, InstallLocationW);
    wsprintf(InstallLocationExeW, L"%s\\engine.exe", InstallLocationW);

    Files[0].pFile = "engine.ini";
    Files[0].file = NULL;
    Files[0].bNoSpaces = false;
    Files[0].bLn = true;
    Files[1].pFile = "options";
    Files[1].file = NULL;
    Files[1].bNoSpaces = true;
    Files[1].bLn = false;

    ZeroMemory(Strings, sizeof(Strings));
    numstrings = 0;

    std::string InstallLocation = ConvertWideToUtf8(InstallLocationW);
    for (DWORD kk = 0; kk < NUM_FILES; kk++)
    {
        char cFileName[2048];

        sprintf(cFileName, "%s\\%s", InstallLocation.c_str(), Files[kk].pFile);

        FILE *f = fopen(cFileName, "r");
        if (!f)
            return false;

        while (!feof(f))
        {
            char cName[2048], cValue[2048];
            char str[2048], *str1;

            str[0] = 0;
            fgets(str, 2048, f);
            string_t *pS = &Strings[numstrings];
            for (DWORD i = 0; i < strlen(str); i++)
                if (str[i] == 0x9)
                    str[i] = ' ';
            while (strlen(str) > 0 && str[strlen(str) - 1] <= 32)
                str[strlen(str) - 1] = 0;
            str1 = GetTokenWhile(str, cName, "=");
            while (strlen(cName) > 0 && cName[strlen(cName) - 1] <= 32)
                cName[strlen(cName) - 1] = 0;
            while (*str1 <= 32 && *str1 != 0)
                str1++;
            GetTokenWhile(str1, cValue, "\0");

            pS->pName = (char *)malloc(strlen(cName) + 1);
            strcpy(pS->pName, cName);
            pS->pValue = (char *)malloc(strlen(cValue) + 1);
            strcpy(pS->pValue, cValue);
            pS->dwFile = kk;
            numstrings++;
        }
        fclose(f);
    }
    if (!numstrings)
        return false;

    Trace("LoadSettings::ResetDialogItems");

    m_DeviceList.ResetContent();
    m_ResList.ResetContent();
    m_SoundVol.SetRange(0, VOLUME_MAX, true);
    m_MusicVol.SetRange(0, VOLUME_MAX, true);
    m_DialogVol.SetRange(0, VOLUME_MAX, true);
    m_TexQuality.SetRange(0, 2, true);

    for (i = 0; i < numdevices; i++)
    {
        m_DeviceList.InsertString(i, Devices[i].Name);
    }

    currentdevice = 0;

    DWORD dwBPP = 32;

    Trace("LoadSettings::FillDialogItems");
    DWORD dx, dy;
    for (i = 0; i < numstrings; i++)
    {
        Trace("LoadSettings::FillDialogItem %d", i);
        string_t *pS = &Strings[i];
        // if first symbol is ';' skip string
        for (j = 0; j < strlen(pS->pName); j++)
        {
            if (pS->pName[j] == ';')
                continue;
            if (pS->pName[j] != ' ')
                break;
        }

        if (stricmp(pS->pName, "PostProcess") == 0)
            sscanf(pS->pValue, "%d", &m_PostProcess);

        if (stricmp(pS->pName, "screen_x") == 0)
            sscanf(pS->pValue, "%d", &dx);

        if (stricmp(pS->pName, "screen_y") == 0)
            sscanf(pS->pValue, "%d", &dy);

        if (stricmp(pS->pName, "full_screen") == 0)
            sscanf(Strings[i].pValue, "%d", &m_Fullscreen);

        if (stricmp(Strings[i].pName, "texture_degradation") == 0)
        {
            sscanf(pS->pValue, "%d", &m_iTexQuality);
            if (m_iTexQuality > 2)
                m_iTexQuality = 2;
            if (m_iTexQuality < 0)
                m_iTexQuality = 0;
            m_iTexQuality = 2 - m_iTexQuality;
            m_TexQualityInfo.LoadString(aTexQuality[m_iTexQuality]);
            // m_TexQuality.SetPos(iQuality);
        }

        // if (stricmp(Strings[i].pName, "arcademode.arcade_on") == 0)
        //    sscanf(pS->pValue, "%d", &m_bArcadeMode);

        if (stricmp(Strings[i].pName, "safe_render") == 0)
            sscanf(pS->pValue, "%d", &m_bSafeRender);

        if (stricmp(pS->pName, "screen_bpp") == 0)
        {
            if (stricmp(pS->pValue, "D3DFMT_R5G6B5") == 0)
                dwBPP = 16;
            if (stricmp(pS->pValue, "D3DFMT_X8R8G8B8") == 0)
                dwBPP = 32;
        }

        if (stricmp(Strings[i].pName, "volume.sound") == 0)
        {
            float volume;
            sscanf(Strings[i].pValue, "%f", &volume);
            m_SoundVol.SetPos(int(volume * VOLUME_MAX));
        }

        if (stricmp(Strings[i].pName, "volume.dialog") == 0)
        {
            float volume;
            sscanf(Strings[i].pValue, "%f", &volume);
            m_DialogVol.SetPos(int(volume * VOLUME_MAX));
        }

        if (stricmp(Strings[i].pName, "volume.music") == 0)
        {
            float volume;
            sscanf(Strings[i].pValue, "%f", &volume);
            m_MusicVol.SetPos(int(volume * VOLUME_MAX));
        }

        if (stricmp(pS->pValue, "soundservice") == 0)
        {
            if (stricmp(pS->pName, ";offclass") == 0)
                m_bSndMusOn = false;
            if (stricmp(pS->pName, "offclass") == 0)
                m_bSndMusOn = true;
        }
    }

    device_t *pD = &Devices[currentdevice];

    // set device
    m_DeviceList.SetCurSel(currentdevice);
    // search resolution and set them
    UpdateResolutions();

    pD->currentresolution = 0;
    for (i = 0; i < pD->numresolutions; i++)
    {
        res_t *pR = &pD->Resolutions[i];
        if (pR->width == dx && pR->height == dy && pR->bpp == dwBPP)
        {
            pD->currentresolution = i;
            break;
        }
    }
    m_ResList.SetCurSel(pD->currentresolution);

    Trace("LoadSettings::UpdateData");
    UpdateData(false);

    Trace("LoadSettings::End");
    return true;
}

void CSetupDlg::UpdateResolutions()
{
    m_ResList.ResetContent();

    device_t *pD = &Devices[currentdevice];

    for (DWORD i = 0; i < pD->numresolutions; i++)
    {
        wchar_t str[256];
        res_t *pR = &pD->Resolutions[i];
        wsprintf(str, L"%dx%d %s bit", pR->width, pR->height, (pR->bpp == 16) ? L"16" : L"32");
        m_ResList.InsertString(i, str);
    }
}

BOOL CSetupDlg::EnumerateDevicesAndResolutions()
{
    ZeroMemory(Devices, sizeof(Devices));

    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);

    numdevices = 0;
    if (EnumDisplayDevices(NULL, numdevices, &dd, 0))
    {
        wcscpy(Devices[numdevices].Name, dd.DeviceName);

        DEVMODE dm = {0};
        dm.dmSize = sizeof(dm);
        for (int iModeNum = 0; EnumDisplaySettings(dd.DeviceName, iModeNum, &dm) != 0; iModeNum++)
        {
            if (dm.dmBitsPerPel >= 32 && dm.dmPelsWidth >= 800 && dm.dmPelsHeight >= 600 &&
                dm.dmDisplayFrequency == 60 && dm.dmDisplayFixedOutput == DMDFO_DEFAULT)
            {
                res_t *pR = &Devices[numdevices].Resolutions[Devices[numdevices].numresolutions];
                pR->bpp = dm.dmBitsPerPel;
                pR->width = dm.dmPelsWidth;
                pR->height = dm.dmPelsHeight;
                Devices[numdevices].numresolutions++;
            }
        }

        numdevices++;
    }

    return true;
}

void CSetupDlg::OnDefaultsettings()
{
    // TODO: Add your control notification handler code here
    m_Fullscreen = false;
    m_PostProcess = false;

    //    m_bArcadeMode = true;
    m_bSafeRender = false;
    m_bSndMusOn = false;

    m_SoundVol.SetPos(VOLUME_MAX);
    m_MusicVol.SetPos(VOLUME_MAX);
    m_DialogVol.SetPos(VOLUME_MAX);

    currentdevice = 0;
    UpdateResolutions();
    // set device
    m_DeviceList.SetCurSel(currentdevice);
    // search resolution and set them

    device_t *pD = &Devices[currentdevice];
    DWORD i = 0;
    for (i = 0; i < pD->numresolutions; i++)
    {
        res_t *pR = &pD->Resolutions[i];
        if (pR->width == 800 && pR->height == 600 && pR->bpp == 16)
        {
            pD->currentresolution = i;
            break;
        }
    }
    if (i == pD->numresolutions)
        Devices[currentdevice].currentresolution = 0;
    m_ResList.SetCurSel(Devices[currentdevice].currentresolution);

    int aTexQuality[3] = {IDS_TEXQUALITY_LOW, IDS_TEXQUALITY_MEDIUM, IDS_TEXQUALITY_HIGH};

    m_iTexQuality = 2;
    m_TexQualityInfo.LoadString(aTexQuality[m_iTexQuality]);
    m_TexQuality.SetPos(m_iTexQuality);

    UpdateData(false);
}

void CSetupDlg::SaveParameters()
{
    char sFmtEStr[256];
    DWORD i, kk;
    // TODO: Add your control notification handler code here

    std::string InstallLocation = ConvertWideToUtf8(InstallLocationW);
    for (kk = 0; kk < NUM_FILES; kk++)
    {
        char cFileName[1024];
        sprintf(cFileName, "%s\\%s", InstallLocation.c_str(), Files[kk].pFile);

        Files[kk].file = fopen(cFileName, "w+b");
    }

    UpdateData(true);

    device_t *pD = &Devices[currentdevice];
    res_t *pR = &pD->Resolutions[pD->currentresolution];

    for (i = 0; i < numstrings; i++)
    {
        char str[1024];
        ZeroMemory(str, sizeof(str));

        strcpy(sFmtEStr, (Files[Strings[i].dwFile].bNoSpaces) ? "=" : " = ");

        if (stricmp(Strings[i].pName, "screen_x") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, pR->width);

        if (stricmp(Strings[i].pName, "screen_y") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, pR->height);

        if (stricmp(Strings[i].pName, "full_screen") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, m_Fullscreen);

        if (stricmp(Strings[i].pName, "PostProcess") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, m_PostProcess);

        if (stricmp(Strings[i].pName, "texture_degradation") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, 2 - m_iTexQuality);

        // if (stricmp(Strings[i].pName,"arcademode.arcade_on") == 0)
        //    sprintf(str,"%s%s%d", Strings[i].pName, sFmtEStr, m_bArcadeMode);

        if (stricmp(Strings[i].pName, "safe_render") == 0)
            sprintf(str, "%s%s%d", Strings[i].pName, sFmtEStr, m_bSafeRender);

        if (stricmp(Strings[i].pName, "screen_bpp") == 0)
            sprintf(str, "%s%s%s", Strings[i].pName, sFmtEStr, (pR->bpp == 16) ? "D3DFMT_R5G6B5" : "D3DFMT_X8R8G8B8");

        if (stricmp(Strings[i].pName, "volume.sound") == 0)
            sprintf(str, "%s%s%.4f", Strings[i].pName, sFmtEStr, (float)m_SoundVol.GetPos() / VOLUME_MAX);

        if (stricmp(Strings[i].pName, "volume.music") == 0)
            sprintf(str, "%s%s%.4f", Strings[i].pName, sFmtEStr, (float)m_MusicVol.GetPos() / VOLUME_MAX);

        if (stricmp(Strings[i].pName, "volume.dialog") == 0)
            sprintf(str, "%s%s%.4f", Strings[i].pName, sFmtEStr, (float)m_DialogVol.GetPos() / VOLUME_MAX);

        if (stricmp(Strings[i].pValue, "soundservice") == 0)
        {
            sprintf(str, "%s%s%s", (m_bSndMusOn) ? "offclass" : ";offclass", sFmtEStr, Strings[i].pValue);
        }

        if (strlen(str) == 0)
        {
            if (strlen(Strings[i].pName) == 0)
            {
                sprintf(str, " ");
            }
            else
            {
                sprintf(str, "%s%s%s", Strings[i].pName, (Strings[i].pValue[0]) ? sFmtEStr : "", Strings[i].pValue);
            }
        }

        DWORD dwNumSpace = 0;
        for (DWORD h = 0; h < strlen(str); h++)
            if (str[h] == ' ')
                dwNumSpace++;
        if (dwNumSpace == strlen(str))
            str[0] = 0;

        if (((i + 1) < (numstrings - 1) && Strings[i].dwFile == Strings[i + 1].dwFile) || (i == numstrings - 2))
            if (i != numstrings - 1)
            {
                if (Strings[i].dwFile == 1)
                    Strings[i].dwFile = 1;
                strcat(str, (Files[Strings[i].dwFile].bLn) ? "\r\n" : "\n");
            }

        if (Files[Strings[i].dwFile].file)
            fwrite(str, strlen(str), 1, Files[Strings[i].dwFile].file);
    }

    for (kk = 0; kk < NUM_FILES; kk++)
    {
        if (Files[kk].file)
            fclose(Files[kk].file);
        Files[kk].file = 0;
    }
}

void CSetupDlg::OnSave()
{
    SaveParameters();
    SendMessage(WM_CLOSE, 0, 0);
}

BOOL CSetupDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    if (pTTT->uFlags & TTF_IDISHWND)
    {
        int nID = ::GetDlgCtrlID(pNMHDR->hwndFrom);
        if (nID)
        {
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
            return true;
        }
    }
    return false;
}

BOOL CSetupDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
    // TODO: Add your specialized code here and/or call the base class

    return CDialog::OnNotify(wParam, lParam, pResult);
}

void CSetupDlg::OnSelchangeDevicelist()
{
    // TODO: Add your control notification handler code here
    currentdevice = m_DeviceList.GetCurSel();
    UpdateResolutions();
    m_ResList.SetCurSel(Devices[currentdevice].currentresolution);
}

void CSetupDlg::OnSelchangeResolutionlist()
{
    // TODO: Add your control notification handler code here
    Devices[currentdevice].currentresolution = m_ResList.GetCurSel();
}

void CSetupDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    for (DWORD i = 0; i < numstrings; i++)
    {
        if (Strings[i].pName)
            free(Strings[i].pName);
        if (Strings[i].pValue)
            free(Strings[i].pValue);

        Strings[i].pName = 0;
        Strings[i].pValue = 0;
    }
}

void CSetupDlg::OnStartgame()
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    // Initialize the STARTUPINFO structure.
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    // TODO: Add your control notification handler code here
    SaveParameters();
    BOOL bProcess = CreateProcess(InstallLocationExeW, NULL, NULL, NULL, FALSE, NULL, NULL, InstallLocationW, &si, &pi);
    DWORD dw = GetLastError();
    SendMessage(WM_CLOSE, 0, 0);
}

void CSetupDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    // TODO: Add your message handler code here and/or call default

    int aTexQuality[3] = {IDS_TEXQUALITY_LOW, IDS_TEXQUALITY_MEDIUM, IDS_TEXQUALITY_HIGH};

    if (pScrollBar->GetSafeHwnd() == m_TexQuality.GetSafeHwnd())
    {
        nPos = m_TexQuality.GetPos();
        m_TexQuality.SetScrollPos(nPos, true);
        UpdateData(true);
        m_TexQualityInfo.LoadString(aTexQuality[nPos]);
        UpdateData(false);
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSetupDlg::OnTtnTooltipShowCustom1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;
}

void CSetupDlg::OnBnClickedFullscreen()
{
    wchar_t txt[2048];
    // TODO: Add your control notification handler code here
    UpdateData(true);
    LoadString(AfxGetResourceHandle(), IDS_FULLSCREEN_WARNING, &txt[0], sizeof(txt));
    if (m_Fullscreen)
        MessageBox(txt, TEXT("Warning!"));
}

void CSetupDlg::OnBnClickedGlow()
{
    // TODO: Add your control notification handler code here
}
