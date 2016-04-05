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


// PCSpimView.h : interface of the CPCSpimView class
//
/////////////////////////////////////////////////////////////////////////////

#include "BreakpointDlg.h"
#include "ConsoleWnd.h"

class CPCSpimDoc;


class CPCSpimView : public CView
{
protected: // create from serialization only
	CPCSpimView();
	DECLARE_DYNCREATE(CPCSpimView)

// Attributes
public:
	BOOL m_fConsoleMinimized;
	void BringConsoleToTop();
	void MakeConsoleVisible();
	CConsoleWnd * GetConsole();
	void UpdateStatusDisplay(BOOL forceDisplay);
	BOOL AskContinue(BOOL fBreak);
	void HighlightCurrentInstruction();
	LPCTSTR GetMessageCaptureBuf();
	void SetMessageCapture(BOOL fOn);
	void WriteToMessage(LPCTSTR strText);
	void WriteToConsole(LPCTSTR strText);
	CPCSpimDoc* GetDocument();
	void CPCSpimView::TileWindows(long cx, long cy, long bottom);
	CFont* GetSpimFont();
	void SetSpimFont(LOGFONT* font);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCSpimView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPCSpimView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void DisplayRegisters(BOOL forceDisplay);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPCSpimView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSimulatorReinitialize();
	afx_msg void OnSimulatorClearRegisters();
	afx_msg void OnFileOpen();
	afx_msg void OnSimulatorRun();
	afx_msg void OnSimulatorBreakpoints();
	afx_msg void OnSimulatorSetvalue();
	afx_msg void OnSimulatorBreak();
	afx_msg void OnSimulatorSettings();
	afx_msg void OnUpdateSimulatorReload(CCmdUI* pCmdUI);
	afx_msg void OnSimulatorReload();
	afx_msg void OnUpdateSimulatorBreak(CCmdUI* pCmdUI);
	afx_msg void OnSimulatorStep();
	afx_msg void OnSimulatorMultistep();
	afx_msg void OnSimulatorDisplaysymbols();
	afx_msg void OnWindowCascade();
	afx_msg void OnWindowNext();
	afx_msg void OnWindowPrevious();
	afx_msg void OnWindowTile();
	afx_msg void OnWindowClearConsole();
	afx_msg void OnUpdateSimulatorSetvalue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulatorBreakpoints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulatorDisplaysymbols(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulatorClearRegisters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulatorReinitialize(CCmdUI* pCmdUI);
	afx_msg void OnWindowMessages();
	afx_msg void OnWindowRegisters();
	afx_msg void OnWindowTextseg();
	afx_msg void OnWindowDataseg();
	afx_msg void OnWindowConsole();
	afx_msg void OnDestroy();
	afx_msg void OnWindowArrangeicons();
	afx_msg void OnFileSaveLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	BOOL m_fSimulatorInitialized;
	CString m_cmdLineFilename;
	CString m_strCurFilename;
	CString m_strMsgCaptureBuf;
	BOOL m_fCapture;

	CBreakpointDlg m_dlgBP;

	CConsoleWnd	m_wndConsole;
	CEdit		m_wndMessages;
	CEdit		m_wndDataSeg;
	CEdit		m_wndTextSeg;
	CEdit		m_wndRegisters;

	void OutputLog(CString &strBuf);
	void ProcessCommandLine();
	void ShowRunning();
	void LoadFile(LPCTSTR strFilename);
	void DisplayTextSegment(BOOL forceDisplay);
	void DisplayDataSegment(BOOL forceDisplay);
	void ExecuteProgram(mem_addr pc, int steps, bool display, bool cont_bkpt);
	void InitializeSimulator();
	void Initialize();
	void ReinitializeSimulator();
};

#ifndef _DEBUG  // debug version in PCSpimView.cpp
inline CPCSpimDoc* CPCSpimView::GetDocument()
   { return (CPCSpimDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
