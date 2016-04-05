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


// ConsoleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PCSpim.h"
#include "PCSpimView.h"
#include "ConsoleWnd.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsoleWnd

CConsoleWnd::CConsoleWnd()
{
    m_fActAsConsole = FALSE;
}

CConsoleWnd::~CConsoleWnd()
{
}


BEGIN_MESSAGE_MAP(CConsoleWnd, CWnd)
    //{{AFX_MSG_MAP(CConsoleWnd)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CConsoleWnd message handlers

BOOL CConsoleWnd::Create(CWnd* pParentWnd,
    LPCTSTR szCaption,
    LPRECT pr,
    DWORD dwREStyle,
    DWORD dwExStyle,
    DWORD dwStyleChildOrPopup)
{
    LPCTSTR strClass = AfxRegisterWndClass(
        CS_DBLCLKS,
        LoadCursor(NULL, IDC_ARROW),
        GetSysColorBrush(COLOR_WINDOW),
        LoadIcon(AfxGetInstanceHandle(),
        MAKEINTRESOURCE(IDI_PCSPIMTYPE)));

    // Style information for the RichEdit control.
    m_dwREStyle = dwREStyle;

    DWORD dwStyle = (dwStyleChildOrPopup
        | WS_OVERLAPPEDWINDOW
        | WS_CLIPSIBLINGS
        | WS_CLIPCHILDREN
        | WS_VISIBLE);

    RECT r = {0, 0, 500, 500};
    if (pr == NULL)
    {
        pr = & r;
    }

    HWND hWndParent;
    if (pParentWnd)
    {
        hWndParent = pParentWnd->m_hWnd;
    }
    else
    {
        hWndParent = NULL;
    }

    return CWnd::CreateEx(dwExStyle, strClass, szCaption, dwStyle,
        pr->left, pr->top,
        pr->right - pr->left, pr->bottom - pr->top,
        hWndParent, 0);
}


void CConsoleWnd::WriteText(LPCTSTR szText)
{
    // Make sure we continue to receive input (need those Ctrl+C's!)
    ((CPCSpimApp *)AfxGetApp())->RunMessageLoop();

    // We may have closed already.
    if (!::IsWindow(m_hWnd))
    {
        return;
    }

    TCHAR *szTemp = MakeCRLFValid(szText);
    m_reText.SetSel(-1, -1);
    m_reText.ReplaceSel(szTemp);
    delete [] szTemp;
}


void CConsoleWnd::Clear()
{
    m_reText.SetWindowText("");
}


void CConsoleWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    //m_reText.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOREPOSITION | SWP_NOZORDER);
    m_reText.MoveWindow(0, 0, cx, cy);
}


static LRESULT CALLBACK ConsoleRichEditProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
        BringWindowToTop(GetParent(hwnd));
        SetFocus(GetParent(hwnd));
        break;

        // Record keypresses in a buffer for later retrieval.
    case WM_CHAR:
        {
            if (g_fRunning)
            {
                // If CTRL is pressed, shift the byte code down.
                if ((wParam >= 'A') &&
                    (wParam <= 'Z') &&
                    (GetKeyState(VK_CONTROL) & 0x8000))
                {
                    wParam -= 'A';
                }

                if (wParam == '\r')
                {
                    wParam = '\n';
                }

                g_pView->GetConsole()->m_blKeys.AddTail((BYTE)wParam);
            }
        }
        return 0;
        break;

        // We want to pass the Alt-char combos on to the frame window.
    case WM_SYSCHAR:
        {
            PostMessage(GetParent(GetParent(hwnd)), WM_SYSCHAR, wParam, lParam);
            return 0;
        }
        break;
    }

    return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA),
        hwnd,
        uMsg,
        wParam,
        lParam);
}


int CConsoleWnd::OnCreate(LPCREATESTRUCT pcs)
{
    if (CWnd::OnCreate(pcs) == -1)
        return -1;

    m_reText.Create(m_dwREStyle
        | WS_VISIBLE
        | ES_READONLY
        | ES_MULTILINE
        | WS_HSCROLL
        | WS_VSCROLL,
        CRect(0, 0, pcs->cx, pcs->cy), this, 0);

    // Now, we need to turn off wrapping.
    // Don't ask me where they came up with this...
    // This is taken from the MFC source for CRichEditView
    m_reText.SetTargetDevice(NULL, 1);

    // Make sure we can have > 32K of text.
    m_reText.LimitText();

    // Set the font to use
    CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_FACE | CFM_COLOR | CFM_BOLD;
    cf.dwEffects = CFE_AUTOCOLOR;
    lstrcpy(cf.szFaceName, "Courier");
    m_reText.SetDefaultCharFormat(cf);

    // Make sure it continues to scroll even if the window does not have the
    // focus
    m_reText.SetOptions(ECOOP_OR, ECO_NOHIDESEL | ECO_SAVESEL);

    // We want to intercept keystrokes for getting input.
    SetWindowLongPtr(m_reText.m_hWnd,
        GWLP_USERDATA,
        (LONG_PTR)SetWindowLongPtr(m_reText.m_hWnd,
        GWLP_WNDPROC,
        (LONG_PTR)ConsoleRichEditProc));

    return 0;
}


BOOL CConsoleWnd::AreKeysAvailable()
{
    return (m_blKeys.GetCount() > 0);
}


BYTE CConsoleWnd::GetKeyPress()
{
    BYTE chRet = NULL;

    if (m_blKeys.GetCount() > 0)
    {
        chRet = m_blKeys.RemoveHead();
    }

    return chRet;
}


void CConsoleWnd::OnSetFocus(CWnd* pOldWnd)
{
    CWnd::OnSetFocus(pOldWnd);

    // We want the control to always have the focus.
    m_reText.SetFocus();
}


void CConsoleWnd::OnClose()
{
    ShowWindow(m_fActAsConsole ? SW_HIDE : SW_MINIMIZE);
}


BOOL CConsoleWnd::PreTranslateMessage(MSG* pMsg)
{
    if (!m_fActAsConsole)
    {
        return CWnd::PreTranslateMessage(pMsg);
    }

    // NOTE: We do not call CWnd's PreTranslateMessage
    // because we _do not_ want accelerators proccessed
    // in this window.

    // If it's a keydown/up, we'll translate/dispatch ourselves.
    switch (pMsg->message)
    {
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
        {
            if (g_fRunning)
            {
                TranslateMessage(pMsg);
                DispatchMessage(pMsg);
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}


LRESULT CConsoleWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
        BringWindowToTop();
        SetFocus();
        break;
    }

    return CWnd::WindowProc(message, wParam, lParam);
}


CRichEditCtrl & CConsoleWnd::GetRichEdit()
{
    return m_reText;
}
