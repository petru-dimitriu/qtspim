/* SPIM S20 MIPS simulator.
   Terminal interface for SPIM simulator.

   Copyright (c) 1990-2015, James R. Larus.
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


#include "spimview.h"
#include "ui_spimview.h"

#include <QStringBuilder>
#define QT_USE_FAST_CONCATENATION
#include <QMessageBox>
#include <QResource>
#include <QTemporaryFile>
extern mem_addr text_top;
extern mem_addr PC;


SpimView::SpimView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SpimView),
    settings("Larus-Stone", "QtSpim")
{
    // Open windows
    //
    ui->setupUi(this);
    SpimConsole = new Console(this);

    stdExceptionHandler = QString("<<SPIM Exception Handler>>");

    // Set style parameters for docking widgets
    //
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Dock widgets
    //
    win_Tile();

    // Wire up the menu and context menu commands
    //
    wireCommands();

    // Restore program settings and window positions
    //
    readSettings();

    // Create a console
    //
    ui->action_Win_Console->setChecked(0);

    programStatus = IDLE;
}


void SpimView::closeEvent(QCloseEvent *)
{
    writeSettings();
    qApp->exit(0);
}


void SpimView::wireCommands()
{
    QObject::connect(ui->action_File_Load, SIGNAL(triggered(bool)), this, SLOT(file_LoadFile()));
    QObject::connect(ui->action_File_Reload, SIGNAL(triggered(bool)), this, SLOT(file_ReloadFile()));
    QObject::connect(ui->action_File_SaveLog, SIGNAL(triggered(bool)), this, SLOT(file_SaveLogFile()));
    QObject::connect(ui->action_File_Print, SIGNAL(triggered(bool)), this, SLOT(file_Print()));
    QObject::connect(ui->action_File_Exit, SIGNAL(triggered(bool)), this, SLOT(file_Exit()));

    QObject::connect(ui->action_Sim_ClearRegisters, SIGNAL(triggered(bool)), this, SLOT(sim_ClearRegisters()));
    QObject::connect(ui->action_Sim_Reinitialize, SIGNAL(triggered(bool)), this, SLOT(sim_ReinitializeSimulator()));
    QObject::connect(ui->action_Sim_SetRunParameters, SIGNAL(triggered(bool)), this, SLOT(sim_SetRunParameters()));
    QObject::connect(ui->action_Sim_Run, SIGNAL(triggered(bool)), this, SLOT(sim_Run()));
    QObject::connect(ui->action_Sim_Pause, SIGNAL(triggered(bool)), this, SLOT(sim_Pause()));
    QObject::connect(ui->action_Sim_Stop, SIGNAL(triggered(bool)), this, SLOT(sim_Stop()));
    QObject::connect(ui->action_Sim_SingleStep, SIGNAL(triggered(bool)), this, SLOT(sim_SingleStep()));
    QObject::connect(ui->action_Sim_DisplaySymbols, SIGNAL(triggered(bool)), this, SLOT(sim_DisplaySymbols()));
    QObject::connect(ui->action_Sim_Settings, SIGNAL(triggered(bool)), this, SLOT(sim_Settings()));

    QObject::connect(ui->action_Reg_DisplayBinary, SIGNAL(triggered(bool)), this, SLOT(reg_DisplayBinary()));
    QObject::connect(ui->action_Reg_DisplayHex, SIGNAL(triggered(bool)), this, SLOT(reg_DisplayHex()));
    QObject::connect(ui->action_Reg_DisplayDecimal, SIGNAL(triggered(bool)), this, SLOT(reg_DisplayDecimal()));

    QObject::connect(ui->action_Text_DisplayUserText, SIGNAL(triggered(bool)), this, SLOT(text_DisplayUserText()));
    QObject::connect(ui->action_Text_DisplayKernelText, SIGNAL(triggered(bool)), this, SLOT(text_DisplayKernelText()));
    QObject::connect(ui->action_Text_DisplayComments, SIGNAL(triggered(bool)), this, SLOT(text_DisplayComments()));
    QObject::connect(ui->action_Text_DisplayInstructionValue, SIGNAL(triggered(bool)), this, SLOT(text_DisplayInstructionValue()));

    QObject::connect(ui->action_Data_DisplayUserData, SIGNAL(triggered(bool)), this, SLOT(data_DisplayUserData()));
    QObject::connect(ui->action_Data_DisplayUserStack, SIGNAL(triggered(bool)), this, SLOT(data_DisplayUserStack()));
    QObject::connect(ui->action_Data_DisplayKernelData, SIGNAL(triggered(bool)), this, SLOT(data_DisplayKernelData()));
    QObject::connect(ui->action_Data_DisplayBinary, SIGNAL(triggered(bool)), this, SLOT(data_DisplayBinary()));
    QObject::connect(ui->action_Data_DisplayHex, SIGNAL(triggered(bool)), this, SLOT(data_DisplayHex()));
    QObject::connect(ui->action_Data_DisplayDecimal, SIGNAL(triggered(bool)), this, SLOT(data_DisplayDecimal()));

    QObject::connect(ui->action_Win_IntRegisters, SIGNAL(triggered(bool)), this, SLOT(win_IntRegisters()));
    QObject::connect(ui->action_Win_FPRegisters, SIGNAL(triggered(bool)), this, SLOT(win_FPRegisters()));
    QObject::connect(ui->action_Win_TextSegment, SIGNAL(triggered(bool)), this, SLOT(win_TextSegment()));
    QObject::connect(ui->action_Win_DataSegment, SIGNAL(triggered(bool)), this, SLOT(win_DataSegment()));
    QObject::connect(ui->action_Win_Console, SIGNAL(triggered(bool)), this, SLOT(win_Console()));
    QObject::connect(ui->action_Win_Tile, SIGNAL(triggered(bool)), this, SLOT(win_Tile()));
    QObject::connect(ui->action_Win_Restore, SIGNAL(triggered(bool)), this, SLOT(win_Restore()));

    QObject::connect(ui->action_Help_ViewHelp, SIGNAL(triggered(bool)), this, SLOT(help_ViewHelp()));
    QObject::connect(ui->action_Help_AboutSPIM, SIGNAL(triggered(bool)), this, SLOT(help_AboutSPIM()));

    QObject::connect(ui->TextSegmentTextEdit->action_Context_SetBreakpoint, SIGNAL(triggered(bool)),
                     ui->TextSegmentTextEdit, SLOT(setBreakpoint()));
    QObject::connect(ui->TextSegmentTextEdit->action_Context_ClearBreakpoint, SIGNAL(triggered(bool)),
                     ui->TextSegmentTextEdit, SLOT(clearBreakpoint()));


    QObject::connect(ui->IntRegTextEdit->action_Context_ChangeValue, SIGNAL(triggered(bool)),
                     ui->IntRegTextEdit, SLOT(changeValue()));
    QObject::connect(ui->FPRegTextEdit->action_Context_ChangeValue, SIGNAL(triggered(bool)),
                     ui->FPRegTextEdit, SLOT(changeValue()));
    QObject::connect(ui->DataSegmentTextEdit->action_Context_ChangeValue, SIGNAL(triggered(bool)),
                     ui->DataSegmentTextEdit, SLOT(changeValue()));
}


QString SpimView::windowFormattingStart(QFont font, QColor fontColor, QColor backgroundColor)
{
    return QString("<span style='font-family:" + font.family()
                   + "; font-size:" + QString::number(font.pointSize(), 10)
                   + "pt; color:" + fontColor.name()
                   + ";background-color:" + backgroundColor.name()
                   +"'>");
}


QString SpimView::windowFormattingEnd()
{
    return "</span>";
}

void SpimView::AdjustProgramCounter()
{
    PC = TEXT_BOT;
}

void SpimView::InitializeWorld()
{
    if (st_loadExceptionHandler)
    {
        if (st_exceptionHandlerFileName == stdExceptionHandler)
        {
            // Standard exception handler is a resource in this executable. Write it to a
            // temporary file and use that for initialization.
            //
            QResource exRes(":exceptions.s");
            QTemporaryFile tmpFile;
            tmpFile.open();
            tmpFile.write((char*)exRes.data());
            tmpFile.close();
            initialize_world(tmpFile.fileName().toLocal8Bit().data(), false);
        }
        else
        {
            // Use the file name supplied by the user.
            //
            initialize_world(st_exceptionHandlerFileName.toLocal8Bit().data(), true);
        }
    }
    else
    {
        // No exception handler.
        //
        initialize_world(NULL, true);
    }
}


void SpimView::SetExceptionHandler(QString fileName, bool loadHandler)
{
    if (fileName != "")
    {
        st_exceptionHandlerFileName = fileName;
    }
    st_loadExceptionHandler = loadHandler;
}


void SpimView::UpdateDataDisplay()
{
    // Text segment rarely changes -- update manually
    //
    DisplayIntRegisters();
    DisplayFPRegisters();
    DisplayDataSegments(false);
}


void SpimView::SaveStateAndExit(int val)
{
    writeSettings();
    exit(val);
}


QString SpimView::WriteOutput(QString message)
{
    if (message.endsWith("\n"))
    {
        message.chop(1);        // Appending adds a <br>, so avoid doubling last newline
    }
    message.replace("\n", "<br>");
    message.replace(" ", "&nbsp;");

    Window->ui->centralWidget->append(QString("<span style=\"font-family:Courier;color:" + outputColor + "\">") + message + QString("</span>"));
    Window->ui->centralWidget->ensureCursorVisible();

    return message;
}


void SpimView::SetOutputColor(QString color)
{
    outputColor = color;
}


void SpimView::Error(QString message, bool fatal)
{
    WriteOutput(message);

    if (fatal)
    {
        QMessageBox::critical(0, "Error", message, QMessageBox::Ok | QMessageBox::Abort, QMessageBox::Ok);
        SaveStateAndExit(1);
    }
    else
    {
        QMessageBox::StandardButton b = QMessageBox::information(0,
                                                                 "Error",
                                                                 message,
                                                                 QMessageBox::Ok | QMessageBox::Abort,
                                                                 QMessageBox::Ok);
        if (b == QMessageBox::Abort)
        {
            force_break = true;
        }
    }
}
