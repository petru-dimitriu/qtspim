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

// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCSpim.h"
#include "PCSpimView.h"
#include "SettingsDlg.h"
#include "SpimReg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSettingsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSettingsDlg)
    m_fBare = FALSE;
    m_fDelayedBranches = FALSE;
    m_fDelayedLoads = FALSE;
    m_fLoadException = FALSE;
    m_fMappedIO = FALSE;
    m_fAllowPseudo = FALSE;
    m_fQuiet = FALSE;
    m_strExceptionFile = _T("");
    m_fFPRegHex = FALSE;
    m_fGenRegHex = FALSE;
    m_fSaveWinPos = FALSE;
    //}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSettingsDlg)
    DDX_Check(pDX, IDC_BARE, m_fBare);
    DDX_Check(pDX, IDC_DELAYEDBRANCHES, m_fDelayedBranches);
    DDX_Check(pDX, IDC_DELAYEDLOADS, m_fDelayedLoads);
    DDX_Check(pDX, IDC_LOADEXCEPTION, m_fLoadException);
    DDX_Check(pDX, IDC_MAPPED, m_fMappedIO);
    DDX_Check(pDX, IDC_PSEUDO, m_fAllowPseudo);
    DDX_Check(pDX, IDC_QUIET, m_fQuiet);
    DDX_Text(pDX, IDC_EXCEPTIONFILE, m_strExceptionFile);
    DDX_Check(pDX, IDC_FPREG_HEX, m_fFPRegHex);
    DDX_Check(pDX, IDC_GENREG_HEX, m_fGenRegHex);
    DDX_Check(pDX, IDC_SAVEWINPOS, m_fSaveWinPos);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
    //{{AFX_MSG_MAP(CSettingsDlg)
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
    ON_BN_CLICKED(IDC_LOADEXCEPTION, OnLoadException)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

void CSettingsDlg::OnBrowse()
{
    UpdateData(TRUE);	// Make sure we don't lose any changes made.

    TCHAR szFilters[] = "Assembly files (*.s; *.asm; *.handler)|*.s;*.asm;*.handler|All Files (*.*)|*.*||";
    // Prompt for a filename.
    CFileDialog dlg(TRUE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
        szFilters,
        this);

    if (dlg.DoModal() != IDOK)
        return;

    m_strExceptionFile = dlg.GetPathName();
    UpdateData(FALSE);
}


void CSettingsDlg::OnOK()
{
    CPCSpimApp *pApp = (CPCSpimApp *)AfxGetApp();
    UpdateData();

    if (!CheckValid())
        return;

    // Place them in their proper global locations, and write them to the registry.
    bare_machine = m_fBare != 0;
    pApp->WriteSetting(SPIM_REG_BARE, bare_machine);

    if (m_fBare)
    {
        m_fDelayedBranches = 1;
        m_fDelayedLoads = 1;
    }

    accept_pseudo_insts = m_fAllowPseudo != 0;
    pApp->WriteSetting(SPIM_REG_PSEUDO, accept_pseudo_insts);

    delayed_branches = m_fDelayedBranches != 0;
    pApp->WriteSetting(SPIM_REG_DELAYEDBRANCHES, delayed_branches);

    delayed_loads = m_fDelayedLoads != 0;
    pApp->WriteSetting(SPIM_REG_DELAYEDLOADS, delayed_loads);

    g_fLoadExceptionHandler = m_fLoadException;
    pApp->WriteSetting(SPIM_REG_LOADEXCEPTION, g_fLoadExceptionHandler);

    if (m_fLoadException)
    {
        delete [] exception_file_name;
        exception_file_name = new TCHAR[(size_t)m_strExceptionFile.GetLength() + 1];
        lstrcpy(exception_file_name, m_strExceptionFile);
        pApp->WriteSetting(SPIM_REG_EXCEPTIONFILE, exception_file_name);
    }

    mapped_io = m_fMappedIO != 0;
    pApp->WriteSetting(SPIM_REG_MAPPEDIO, mapped_io);

    quiet = m_fQuiet != 0;
    pApp->WriteSetting(SPIM_REG_QUIET, quiet);

    g_fGenRegHex = m_fGenRegHex;
    pApp->WriteSetting(SPIM_REG_GENREG_HEX, g_fGenRegHex);

    g_fFPRegHex = m_fFPRegHex;
    pApp->WriteSetting(SPIM_REG_FPREG_HEX, g_fFPRegHex);

    g_fSaveWinPos = m_fSaveWinPos;
    pApp->WriteSetting(SPIM_REG_SAVEWINPOS, g_fSaveWinPos);

    g_pView->DisplayRegisters(FALSE);

    CDialog::OnOK();
}


BOOL CSettingsDlg::OnInitDialog()
{
    CPCSpimApp *pApp = (CPCSpimApp *)AfxGetApp();

    CDialog::OnInitDialog();

    // Load the current settings.
    m_fBare = pApp->GetSetting(SPIM_REG_BARE, FALSE);
    m_fDelayedBranches = pApp->GetSetting(SPIM_REG_DELAYEDBRANCHES, FALSE);
    m_fDelayedLoads = pApp->GetSetting(SPIM_REG_DELAYEDLOADS, FALSE);
    m_fAllowPseudo = pApp->GetSetting(SPIM_REG_PSEUDO, TRUE);
    m_fQuiet = pApp->GetSetting(SPIM_REG_QUIET, FALSE);
    m_fMappedIO = pApp->GetSetting(SPIM_REG_MAPPEDIO, TRUE);
    m_fLoadException = pApp->GetSetting(SPIM_REG_LOADEXCEPTION, TRUE);
    m_fGenRegHex = pApp->GetSetting(SPIM_REG_GENREG_HEX, TRUE);
    m_fFPRegHex = pApp->GetSetting(SPIM_REG_FPREG_HEX, FALSE);
    m_strExceptionFile = exception_file_name;
    m_fSaveWinPos = g_fSaveWinPos;

    UpdateData(FALSE);

    GetDlgItem(IDC_EXCEPTIONFILE)->EnableWindow(m_fLoadException);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CSettingsDlg::CheckValid()
{
    m_strExceptionFile.TrimLeft();
    m_strExceptionFile.TrimRight();

    OFSTRUCT ofs;
    if (m_fLoadException && (HFILE_ERROR == OpenFile(m_strExceptionFile, &ofs, OF_EXIST)))
    {
        MessageBox("The specified exception file does not exist.\n\n"
            "Please specify an existing file, or turn off exception file loading.",
            NULL, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}


void CSettingsDlg::OnLoadException()
{
    GetDlgItem(IDC_EXCEPTIONFILE)->EnableWindow((BOOL)IsDlgButtonChecked(IDC_LOADEXCEPTION));
}

