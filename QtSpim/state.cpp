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

#include <QDesktopWidget>

#include "spimview.h"
#include "ui_spimview.h"


//
// Restore program settings and window positions
//

void SpimView::readSettings()
{
    settings.beginGroup("OriginalGeometry");
    settings.remove("");    // Remove remnants of previous hack if earlier version
    settings.endGroup();    // of QtSpim ran on this system

    settings.beginGroup("MainWin");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray(), 1);
    settings.endGroup();

    // If the size of the restored window exceeds the current screen size, resize the window.
    //
    const QRect availGeo = App->desktop()->availableGeometry(this);
    const QRect& curFGeo = this->frameGeometry();
    if (!availGeo.contains(curFGeo))
    {
        this->adjustSize();
    }

    settings.beginGroup("RegWin");
    st_colorChangedRegisters = settings.value("ColorChangedRegs", true).toBool();
    st_changedRegisterColor = settings.value("ChangedRegColor", "red").toString();
    st_regDisplayBase = settings.value("RegisterDisplayBase", 16).toInt();
    st_regDisplayBase = setCheckedRegBase(st_regDisplayBase);

    st_regWinFont = settings.value("Font", QFont("Courier", 10)).value<QFont>();
    st_regWinFontColor = settings.value("FontColor", QColor("black")).value<QColor>();
    st_regWinBackgroundColor = settings.value("BackgroundColor", QColor("white")).value<QColor>();

    ui->action_Win_IntRegisters->setChecked(!ui->IntRegDockWidget->isHidden());
    ui->action_Win_FPRegisters->setChecked(!ui->FPRegDockWidget->isHidden());
    settings.endGroup();


    settings.beginGroup("TextWin");
    st_showUserTextSegment = settings.value("ShowUserTextSeg", true).toBool();
    ui->action_Text_DisplayUserText->setChecked(st_showUserTextSegment);
    st_showKernelTextSegment = settings.value("ShowKernelTextSeg", true).toBool();
    ui->action_Text_DisplayKernelText->setChecked(st_showKernelTextSegment);
    st_showTextComments =  settings.value("ShowTextComments", true).toBool();
    ui->action_Text_DisplayComments->setChecked(st_showTextComments);
    st_showTextDisassembly =  settings.value("ShowInstDisassembly", true).toBool();
    ui->action_Text_DisplayInstructionValue->setChecked(st_showTextDisassembly);

    st_textWinFont = settings.value("Font", QFont("Courier", 10)).value<QFont>();
    st_textWinFontColor = settings.value("FontColor", QColor("black")).value<QColor>();
    st_textWinBackgroundColor = settings.value("BackgroundColor", QColor("white")).value<QColor>();
    st_numberOfWordsPerLine = settings.value("NumberOfBytesPerLineStackSeg",1).toInt();

    ui->action_Win_TextSegment->setChecked(!ui->TextSegDockWidget->isHidden());
    settings.endGroup();


    settings.beginGroup("DataWin");
    st_showUserDataSegment = settings.value("ShowUserDataSeg", true).toBool();
    ui->action_Data_DisplayUserData->setChecked(st_showUserDataSegment);
    st_showUserStackSegment = settings.value("ShowUserStackSeg", true).toBool();
    ui->action_Data_DisplayUserStack->setChecked(st_showUserStackSegment);
    st_showKernelDataSegment = settings.value("ShowKernelDataSeg", true).toBool();
    ui->action_Data_DisplayKernelData->setChecked(st_showKernelDataSegment);
    st_dataSegmentDisplayBase = settings.value("DataSegmentDisplayBase", 16).toInt();
    st_dataSegmentDisplayBase = setCheckedDataSegmentDisplayBase(st_dataSegmentDisplayBase);

    ui->action_Win_DataSegment->setChecked(!ui->DataSegDockWidget->isHidden());
    settings.endGroup();


    settings.beginGroup("FileMenu");
    st_recentFilesLength = settings.value("RecentFilesLength", 4).toInt();
    st_recentFiles.clear();
    int i;
    for (i = 0; i < st_recentFilesLength; i++)
    {
        QString file = settings.value("RecentFile" + QString(i), "").toString();
        st_recentFiles.append(file);
    }
    rebuildRecentFilesMenu();
    settings.endGroup();


    settings.beginGroup("Spim");
    quiet = settings.value("Quiet", false).toBool();

    bare_machine = settings.value("BareMachine", 0).toBool();
    accept_pseudo_insts = settings.value("AcceptPseudoInsts", 1).toBool();
    delayed_branches = settings.value("DelayedBranches", 0).toBool();
    delayed_loads = settings.value("DelayedLoads", 0).toBool();
    mapped_io = settings.value("MappedIO", 0).toBool();

    st_loadExceptionHandler = settings.value("LoadExceptionHandler", true).toBool();
    st_exceptionHandlerFileName = settings.value("ExceptionHandlerFileName",
                                                 stdExceptionHandler).toString();
    st_startAddress = settings.value("StartingAddress", starting_address()).toInt();
    st_commandLine = settings.value("CommandLineArguments", "").toString();
    settings.endGroup();
}


void SpimView::writeSettings()
{
    settings.beginGroup("MainWin");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState(1));
    settings.endGroup();


    settings.beginGroup("RegWin");
    settings.setValue("ColorChangedRegs", st_colorChangedRegisters);
    settings.setValue("ChangedRegColor", st_changedRegisterColor);
    settings.setValue("RegisterDisplayBase", st_regDisplayBase);

    settings.setValue("Font", st_regWinFont);
    settings.setValue("FontColor", st_regWinFontColor);
    settings.setValue("BackgroundColor", st_regWinBackgroundColor);
    settings.endGroup();


    settings.beginGroup("TextWin");
    settings.setValue("ShowUserTextSeg", st_showUserTextSegment);
    settings.setValue("ShowKernelTextSeg", st_showKernelTextSegment);
    settings.setValue("ShowTextComments", st_showTextComments);
    settings.setValue("ShowInstDisassembly", st_showTextDisassembly);

    settings.setValue("Font", st_textWinFont);
    settings.setValue("FontColor", st_textWinFontColor);
    settings.setValue("BackgroundColor", st_textWinBackgroundColor);
    settings.setValue("NumberOfBytesPerLineStackSeg", st_numberOfWordsPerLine);
    settings.endGroup();


    settings.beginGroup("DataWin");
    settings.setValue("ShowUserDataSeg", st_showUserDataSegment);
    settings.setValue("ShowUserStackSeg", st_showUserStackSegment);
    settings.setValue("ShowKernelDataSeg", st_showKernelDataSegment);
    settings.setValue("DataSegmentDisplayBase", st_dataSegmentDisplayBase);
    settings.endGroup();

    settings.beginGroup("FileMenu");
    settings.setValue("RecentFilesLength", st_recentFilesLength);
    int i;
    for (i = 0; i < st_recentFilesLength; i++)
    {
        if (i < st_recentFiles.length())
        {
            settings.setValue("RecentFile" + QString(i), st_recentFiles[i]);
        }
        else
        {
            settings.setValue("RecentFile" + QString(i), "");
        }
    }
    settings.endGroup();


    settings.beginGroup("Spim");
    settings.setValue("Quiet", quiet);

    settings.setValue("BareMachine", bare_machine);
    settings.setValue("AcceptPseudoInsts", accept_pseudo_insts);
    settings.setValue("DelayedBranches", delayed_branches);
    settings.setValue("DelayedLoads", delayed_loads);
    settings.setValue("MappedIO", mapped_io);

    settings.setValue("LoadExceptionHandler", st_loadExceptionHandler);
    settings.setValue("ExceptionHandlerFileName", st_exceptionHandlerFileName);
    settings.setValue("StartingAddress", st_startAddress);
    settings.setValue("CommandLine", st_commandLine);
    settings.endGroup();

    settings.sync();
}


// Restore windows to initial, default configuration.
//

void SpimView::win_Restore()
{
    win_Tile();
    settings.beginGroup("MainWin");
    settings.remove("Geometry");  // Remove current window configuration
    settings.endGroup();
}
