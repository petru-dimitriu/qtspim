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


// SetValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCSpim.h"
#include "SetValueDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetValueDlg dialog


CSetValueDlg::CSetValueDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSetValueDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSetValueDlg)
    m_strAddress = _T("");
    m_strValue = _T("");
    //}}AFX_DATA_INIT
}


void CSetValueDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetValueDlg)
    DDX_Text(pDX, IDC_ADDRESS, m_strAddress);
    DDX_Text(pDX, IDC_VALUE, m_strValue);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetValueDlg, CDialog)
    //{{AFX_MSG_MAP(CSetValueDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetValueDlg message handlers

void CSetValueDlg::OnOK()
{
    int reg_no;
    char *eptr;
    char *szAddress;

    UpdateData();

    m_strAddress.MakeLower();

    int nValue = strtol(m_strValue, &eptr, 0);

    szAddress = m_strAddress.GetBuffer(0);
    reg_no = register_name_to_number(szAddress);
    m_strAddress.ReleaseBuffer();

    if (reg_no < 0)
    {
        if (m_strAddress[0] == '$' || m_strAddress[0] == 'r')
        {
            szAddress = m_strAddress.GetBuffer(0);
            reg_no = register_name_to_number(szAddress + 1);
            m_strAddress.ReleaseBuffer();
        }
    }

    if (reg_no == 0)
    {
        MessageBox("Cannot modify register 0.");
        return;
    }
    else if (reg_no > 0)
        R[reg_no] = nValue;
    else if (streq(m_strAddress, "status"))
        CP0_Status = nValue;
    else if (streq(m_strAddress, "pc"))
        PC = (mem_addr)nValue;
    else if (streq (m_strAddress, "epc"))
        CP0_EPC = nValue;
    else
    {
        mem_addr addr;

        /* Try to parse string as a number */
        addr = strtoul(m_strAddress, &eptr, 0);
        if (eptr == m_strAddress)
        {
            MessageBox("Please enter either a register name or a valid address.");
            return;
        }
        else
            set_mem_word (addr, nValue);
    }

    CDialog::OnOK();
}
