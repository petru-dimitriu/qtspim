// SPIM S20 MIPS simulator.
// Definitions for the SPIM S20.
//
// Copyright (c) 1990-2010, James R. Larus.
// Changes for DOS and Windows versions by David A. Carley (dac@cs.wisc.edu)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
//
// Neither the name of the James R. Larus nor the names of its contributors may be
// used to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


// PCSpim.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PCSpim.h"

#include "MainFrm.h"
#include "PCSpimDoc.h"
#include "PCSpimView.h"
#include "util.h"
#include "SpimReg.h"

#include "regman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPCSpimApp

BEGIN_MESSAGE_MAP(CPCSpimApp, CWinApp)
    //{{AFX_MSG_MAP(CPCSpimApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPCSpimApp construction

CPCSpimApp::CPCSpimApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CPCSpimApp object

CPCSpimApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CPCSpimApp initialization

BOOL CPCSpimApp::InitInstance()
{
    // SPIM is bad about cleaning up after itself...
    AfxEnableMemoryTracking(FALSE);

    // Standard initialization

    // Turn on registry (vs. INI) support, and set the "company" tag.
    SetRegistryKey("LarusStone");

    // If registry settings for this user do not exist, copy them from
    // the "Default Settings" tree in HKLM.
    HKEY hKeySrc, hKeySettings;
    HKEY hKeyDest = GetAppRegistryKey();
    if (S_OK != RegMan_OpenKey(hKeyDest, "Settings", &hKeySettings))
    {
        if (S_OK == RegMan_OpenKey(HKEY_LOCAL_MACHINE,
            SPIM_REG_DEFAULTSETTINGS,
            &hKeySrc))
        {
            RegMan_CopyTree(hKeySrc, hKeyDest);
            RegMan_CloseKey(hKeySrc);
        }
    }
    else
    {
        RegMan_CloseKey(hKeySettings);
    }
    RegMan_CloseKey(hKeyDest);


    LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    // serve as the connection between documents, frame windows and views.
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME,
        RUNTIME_CLASS(CPCSpimDoc),
        // main SDI frame window
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CPCSpimView));
    AddDocTemplate(pDocTemplate);


    // We do our own cmdline processing later.  Even though we aren't using
    // MFC's standard cmdline processing (we aren't calling ParseCommandLine),
    // we _must_ call ProcessShellCommand...
    CCommandLineInfo cmdInfo;
    if (!ProcessShellCommand(cmdInfo))
    {
        return FALSE;
    }
    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
    virtual BOOL OnInitDialog();
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
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// App command to run the dialog
void CPCSpimApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CPCSpimApp commands

void CPCSpimApp::RunMessageLoop()
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if (!PumpMessage())
        {
            force_break = true;
            ::PostQuitMessage(0);
            break;
        }
    }

    // let MFC do its idle processing
    LONG lIdle = 0;
    while (OnIdle(lIdle++));
}


int CPCSpimApp::GetSetting(LPCTSTR strName, int nDefVal)
{
    return (int)GetProfileInt("Settings", strName, nDefVal);
}


LPCTSTR CPCSpimApp::GetSetting(LPCTSTR strName, LPCTSTR strDefVal)
{
    static CString strRet;

    strRet = GetProfileString("Settings", strName, strDefVal);

    return strRet;
}


void CPCSpimApp::WriteSetting(LPCTSTR strName, int nValue)
{
    WriteProfileInt("Settings", strName, nValue);
}


void CPCSpimApp::WriteSetting(LPCTSTR strName, LPCTSTR strValue)
{
    WriteProfileString("Settings", strName, strValue);
}


BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    g_pView->SetMessageCapture(TRUE);
    write_startup_message();
    GetDlgItem(IDC_COPYRIGHT)->SetWindowText(g_pView->GetMessageCaptureBuf());
    g_pView->SetMessageCapture(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


#define UTIL_RECTPRINTFFORMAT	"%d, %d, %d, %d"


BOOL CPCSpimApp::GetSetting(LPCTSTR strName, LPRECT pr)
{
    CString strRect = GetProfileString("Settings", strName);

    return (4 == _stscanf(strRect,
        UTIL_RECTPRINTFFORMAT,
        &pr->left, &pr->top, &pr->right, &pr->bottom));
}


void CPCSpimApp::WriteSetting(LPCTSTR strName, LPRECT pr)
{
    CString strRect;
    strRect.Format(UTIL_RECTPRINTFFORMAT, pr->left, pr->top, pr->right, pr->bottom);
    WriteProfileString("Settings", strName, strRect);
}

