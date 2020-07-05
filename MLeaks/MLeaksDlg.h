// MLeaksDlg.h : header file
//

#if !defined(AFX_MLEAKSDLG_H__B84D5551_80AB_465B_BF9D_3548487BD315__INCLUDED_)
#define AFX_MLEAKSDLG_H__B84D5551_80AB_465B_BF9D_3548487BD315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMLeaksDlg dialog

#include "..\common_h\templates\array.h"
#include "..\common_h\templates\string.h"

struct tmp_t
{
	char		sFile[512];
	float		fBlocks;
	float		fSize, fDelta;
};

class CMLeaksDlg : public CDialog
{
// Construction
public:
	DWORD			dwTmp;
	tmp_t			Tmp[2048];
	CMLeaksDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMLeaksDlg)
	enum { IDD = IDD_MLEAKS_DIALOG };
	CListCtrl	m_Leaks;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMLeaksDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMLeaksDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnCompare();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MLEAKSDLG_H__B84D5551_80AB_465B_BF9D_3548487BD315__INCLUDED_)
