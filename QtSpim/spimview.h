/* SPIM S20 MIPS simulator.
   Terminal interface for SPIM simulator.

   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SPIMVIEW_H
#define SPIMVIEW_H

#include <QMainWindow>
#include <QSettings>
#include <QList>
#include <QString>

#include "ui_spimview.h"
#include "console.h"

#include "../CPU/spim.h"
#include "../CPU/string-stream.h"
#include "../CPU/spim-utils.h"
#include "../CPU/inst.h"
#include "../CPU/reg.h"
#include "../CPU/mem.h"
#include "../CPU/sym-tbl.h"
#include "../CPU/version.h"

namespace Ui {
    class SpimView;
}

class SpimView : public QMainWindow
{
Q_OBJECT

 public:
    explicit SpimView(QWidget *parent = 0);

    virtual void closeEvent(QCloseEvent *);

    QString WriteOutput(QString message);
    void Error(QString message, bool fatal);

    void SaveStateAndExit(int val);

    void InitializeWorld();
    void SetExceptionHandler(QString fileName, bool loadHandler);

    void CaptureIntRegisters();
    void CaptureSFPRegisters();
    void CaptureDFPRegisters();

    void DisplayIntRegisters();
    void DisplayFPRegisters();
    void DisplayTextSegments(bool force);
    void DisplayDataSegments(bool force);
    void UpdateDataDisplay();

    void AdjustProgramCounter();

    int RegDisplayBase() { return st_regDisplayBase;}
    int DataDisplayBase() { return st_dataSegmentDisplayBase;}

    Ui::SpimView *ui;

    void SetOutputColor(QString color);

 private:

    QString outputColor;

    //
    // Program state
    //
    QSettings settings;
    void readSettings();
    void writeSettings();
    QString stdExceptionHandler;

    // File menu
    //
    int st_recentFilesLength;
    QList<QString> st_recentFiles;
    void rebuildRecentFilesMenu();

    // Simulator menu
    //
    bool st_loadExceptionHandler;
    QString st_exceptionHandlerFileName;
    int st_startAddress;
    QString st_commandLine;

    // Register window
    //
    bool st_colorChangedRegisters;
    QString st_changedRegisterBgColor;
    int st_regDisplayBase;
    QFont st_regWinFont;
    QColor st_regWinFontColor;
    QColor st_regWinBackgroundColor;

    // Text window
    //
    bool st_showUserTextSegment;
    bool st_showKernelTextSegment;
    bool st_showTextComments;
    bool st_showTextDisassembly;
    QFont st_textWinFont;
    QColor st_textWinFontColor;
    QColor st_textWinBackgroundColor;

    // Data window
    //
    bool st_showUserDataSegment;
    bool st_showUserStackSegment;
    bool st_showKernelDataSegment;
    int st_dataSegmentDisplayBase;
    unsigned int st_numberOfWordsPerLine;

    //
    // End of state

    //
    // Methods:
    //

    // Establish text formatting for a window
    //
    QString windowFormattingStart(QFont font, QColor fontColor, QColor backgroundColor);
    QString windowFormattingEnd();


    // Integer registers window
    //
    QString formatSpecialIntRegister(int value, char* name, bool changed);
    QString formatIntRegister(int regNum, int value, char* name, bool changed);

    // Value in register at previous call on displayIntRegister, so changed values
    // can be highlighted.
    //
    reg_word oldR[R_LENGTH];
    mem_addr oldPC;
    reg_word oldEPC;
    reg_word oldCause;
    reg_word oldBadVAddr;
    reg_word oldStatus;
    reg_word oldHI;
    reg_word oldLO;


    // Single precision FP registers window
    //
    QString formatSFPRegisters();
    QString formatSpecialSFPRegister(int value, char* name, bool changed);
    QString formatSFPRegister(int regNum, float value, bool changed);

    float oldFPR_S[FGR_LENGTH];
    reg_word oldFIR;
    reg_word oldFCSR;
    reg_word oldFCCR;
    reg_word oldFEXR;


    // Double precision FP registers window
    //
    QString formatDFPRegisters();
    QString formatDFPRegister(int regNum, double value, bool changed);

    double oldFPR_D[FPR_LENGTH];


    // Common register methods
    //
    QString formatInt(int value);
    QString formatFloat(float value);
    QString formatDouble(double value);
    QString formatReg(QString reg, QString value, bool changed);
    QString registerBefore(bool changed);
    QString registerAfter(bool changed);
    QString nnbsp(int n);


    // Text segment window
    //
    QString formatUserTextSeg();
    QString formatKernelTextSeg();
    QString formatInstructions(mem_addr from, mem_addr to);
    void highlightInstruction(mem_addr pc);


    // Data segment window
    //
    QString formatUserDataSeg();
    QString formatUserStack();
    QString formatKernelDataSeg();
    QString formatMemoryContents(mem_addr from, mem_addr to);
    QString formatPartialQuadWord(mem_addr from, mem_addr to);
    QString formatAsChars(mem_addr from, mem_addr to);


    //
    // Menu functions
    //
    void wireCommands();
    void initStack();
    void executeProgram(mem_addr pc, int steps, bool display, bool contBkpt);
    void initializePCAndStack();
    enum PROGSTATE {IDLE, STOPPED, PAUSED, RUNNING, SINGLESTEP} programStatus;
    void updateStatus(PROGSTATE status);


    //
    // Console
    //
 public:
    Console* SpimConsole;


    public slots:
    void file_LoadFile();
    void file_ReloadFile();
    void file_SaveLogFile();
    void file_Print();
    void file_Exit();

    void sim_ClearRegisters();
    void sim_ReinitializeSimulator();
    void sim_SetRunParameters();
    void sim_Run();
    void sim_Pause();
    void sim_Stop();
    void sim_SingleStep();
    void sim_DisplaySymbols();
    void sim_Settings();

    void reg_DisplayBinary();
    void reg_DisplayHex();
    void reg_DisplayDecimal();
    int setCheckedRegBase(int base);
    int setBaseInternal(int base, QAction* actionBinary, QAction* actionDecimal, QAction* actionHex);

    void text_DisplayUserText();
    void text_DisplayKernelText();
    void text_DisplayComments();
    void text_DisplayInstructionValue();

    void data_DisplayUserData();
    void data_DisplayUserStack();
    void data_DisplayKernelData();
    void data_DisplayBinary();
    void data_DisplayHex();
    void data_DisplayDecimal();
    int setCheckedDataSegmentDisplayBase(int base);

    void win_IntRegisters();
    void win_FPRegisters();
    void win_TextSegment();
    void win_DataSegment();
    void win_Console();
    void win_Tile();
    void win_Restore();

    void help_ViewHelp();
    void help_AboutSPIM();

    void continueBreakpoint();
    void singleStepBreakpoint();
    void abortBreakpoint();
};

extern SpimView* Window;
extern QApplication* App;


// Format SPIM abstractions for display
//
QString formatAddress(mem_addr addr);
QString formatWord(mem_word word, int base);
QString formatChar(int chr);
QString formatSegLabel(QString segName, mem_addr low, mem_addr high);

QString promptForNewValue(QString text, int* base);


#endif // SPIMVIEW_H
