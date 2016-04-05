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


// RunDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCSpim.h"
#include "RunDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRunDlg dialog


CRunDlg::CRunDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CRunDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CRunDlg)
    m_strAddress = _T("");
    m_checkUndefinedSymbols = g_checkUndefinedSymbols;
    //}}AFX_DATA_INIT
}


void CRunDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRunDlg)
    DDX_Text(pDX, IDC_ADDRESS, m_strAddress);
    DDX_Check(pDX, IDC_CHECK1, m_checkUndefinedSymbols);
    DDX_Text(pDX, IDC_COMMANDLINE, g_strCmdLine);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRunDlg, CDialog)
    //{{AFX_MSG_MAP(CRunDlg)
    ON_BN_CLICKED(IDC_CHECK1, OnCheckUndefined)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunDlg message handlers

void CRunDlg::OnOK()
{
    char * eptr = NULL;
    mem_addr addr = 0;

    // Make sure they've specified a valid address.
    UpdateData();

    if (!m_strAddress.IsEmpty())
    {
        if (isdigit(m_strAddress[0]))
            addr = (mem_addr)strtoul(m_strAddress, &eptr, 0);
        else
        {
            char *szAddress = m_strAddress.GetBuffer(0);
            addr = find_symbol_address(szAddress);
            m_strAddress.ReleaseBuffer();
        }

        if (addr == 0)
        {
            CString strMsg;

            strMsg.Format("Please enter an address or symbol at which to begin execution.\n"
                "Or, leave blank to begin at the default address of 0x%08x", starting_address());
            MessageBox(strMsg);
            return;
        }

        char *undefSyms;
        if (m_checkUndefinedSymbols && (undefSyms = undefined_symbol_string ()) != NULL)
        {
            CString strMsg;

            strMsg.Format("The following symbols are undefined:\n%s\n", undefSyms);
            int outcome = MessageBox(strMsg, NULL, MB_OKCANCEL);
            if (outcome == 2)	// "Cancel"
            {
                OnCancel();
                return;
            }
        }
    }

    CDialog::OnOK();
}


BOOL CRunDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Load the current starting address.
    m_strAddress.Format("0x%08x", starting_address());
    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CRunDlg::OnCheckUndefined()
{
    m_checkUndefinedSymbols = !m_checkUndefinedSymbols;
    g_checkUndefinedSymbols = m_checkUndefinedSymbols;
}
