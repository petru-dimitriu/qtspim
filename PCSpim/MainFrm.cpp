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


// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PCSpim.h"
#include "PCSpimView.h"
#include "SpimReg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_SIMULATOR_SETFONT, OnSimulatorSetFont)
    //}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_REGS, UpdateStatusRegs)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_BARE, UpdateStatusBare)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_DELAY_BR, UpdateStatusDelayBr)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_DELAY_LD, UpdateStatusDelayLd)
    // Global help commands
    ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
    ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_REGS,
    ID_INDICATOR_BARE,
    ID_INDICATOR_DELAY_BR,
    ID_INDICATOR_DELAY_LD
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here

}


CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    if (!m_wndToolBar.Create(this) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    m_wndStatusBar.SetPaneStyle(1, SBPS_STRETCH);

    // TODO: Remove this if you don't want tool tips or a resizeable toolbar
    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()
        | CBRS_TOOLTIPS
        | CBRS_FLYBY
        | CBRS_SIZE_DYNAMIC);

    // TODO: Delete these three lines if you don't want the toolbar to be
    // dockable
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    g_pView->PostMessage(WM_COMMAND, MAKEWPARAM(ID_WINDOW_ARRANGEICONS, 0), 0);

    return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    // Turn off the filename portion of the window title.
    cs.style &= ~(FWS_ADDTOTITLE | FWS_PREFIXTITLE);

    CPCSpimApp *pApp = (CPCSpimApp *)AfxGetApp();

    g_fSaveWinPos = pApp->GetSetting(SPIM_REG_SAVEWINPOS, TRUE);
    if (g_fSaveWinPos)
    {
        RECT r;
        if (pApp->GetSetting(SPIM_REG_MAINWINPOS, &r))
        {
            cs.x = r.left;
            cs.y = r.top;
            cs.cx = r.right - r.left;
            cs.cy = r.bottom - r.top;
        }
    }

    return CFrameWnd::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}


void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LPCTSTR CMainFrame::GetTitleBase()
{
    return m_strTitle;
}

void CMainFrame::UpdateStatusRegs(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    CString strTxt;

    strTxt.Format("PC=0x%08x  EPC=0x%08x  Cause=0x%08x", PC, CP0_EPC, CP0_Cause);
    pCmdUI->SetText(strTxt);
}


void CMainFrame::UpdateStatusBare(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    if (bare_machine) pCmdUI->SetText("BARE"); else pCmdUI->SetText("");
}


void CMainFrame::UpdateStatusDelayBr(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    if (delayed_branches) pCmdUI->SetText("DELAY BR"); else pCmdUI->SetText("");
}


void CMainFrame::UpdateStatusDelayLd(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    if (delayed_loads) pCmdUI->SetText("DELAY LD"); else pCmdUI->SetText("");
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    if (nType == SIZE_MINIMIZED)
    {
        g_pView->m_fConsoleMinimized = TRUE;
        g_pView->GetConsole()->ShowWindow(SW_HIDE);
    }
    else if (nType == SIZE_RESTORED)
    {
        g_pView->m_fConsoleMinimized = FALSE;
        g_pView->GetConsole()->ShowWindow(SW_RESTORE);
    }

    RECT r;
    g_pView->GetClientRect(&r);
    g_pView->TileWindows(r.right - r.left, r.bottom - r.top, r.bottom);
}

void CMainFrame::OnSimulatorSetFont()
{
    CHOOSEFONT choiceFont;

    // Initialize the font structure:
    //
    memset(&choiceFont, 0, sizeof(choiceFont));
    choiceFont.lStructSize = sizeof(choiceFont);
    choiceFont.hwndOwner = g_pView->m_hWnd ;
    LOGFONT lFont;
    CFont* cFont = g_pView->GetSpimFont();
    cFont->GetLogFont(&lFont);
    choiceFont.lpLogFont = &lFont;
    choiceFont.Flags = CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY | CF_SCREENFONTS;

    if (ChooseFont(&choiceFont))
    {
        g_pView->SetSpimFont(choiceFont.lpLogFont);
    }
}