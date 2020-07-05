// MLeaks.h : main header file for the MLEAKS application
//

#if !defined(AFX_MLEAKS_H__308492B9_3786_4E4B_9D66_6448BB984017__INCLUDED_)
#define AFX_MLEAKS_H__308492B9_3786_4E4B_9D66_6448BB984017__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMLeaksApp:
// See MLeaks.cpp for the implementation of this class
//

class CMLeaksApp : public CWinApp
{
public:
	CMLeaksApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMLeaksApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMLeaksApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MLEAKS_H__308492B9_3786_4E4B_9D66_6448BB984017__INCLUDED_)
