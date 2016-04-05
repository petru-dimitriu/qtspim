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
#include "ui_savelogfile.h"
#include "ui_printwindows.h"
#include "ui_runparams.h"
#include "ui_settings.h"
#include "ui_breakpoint.h"
#include "spim_settings.h"
#include "version.h"


#include <QStringBuilder>
#define QT_USE_FAST_CONCATENATION
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QProcess>
#include <QScrollBar>

//
// Menu functions
//



// File menu
//

void SpimView::file_LoadFile()
{
    QString file;
    if (((QAction*)sender())->objectName() != "action_File_Load"
        && ((QAction*)sender())->objectName() != "action_File_Reload")
    {
        file = ((QAction*)sender())->text(); // Recent file menu entry's names are file names
    }
    else
    {
        file = QFileDialog::getOpenFileName(this,
                                            "Open Assembly Code",
                                            st_recentFiles[0],
                                            "Assembly (*.a *.s *.asm);;Text files (*.txt)");
    }
    if (!file.isNull())
    {
        read_assembly_file(file.toLocal8Bit().data());
        st_recentFiles.removeAll(file);
        st_recentFiles.prepend(file);
        rebuildRecentFilesMenu();
        // set PC to 0x400000 in order to have the program ready to run rightaway!
        AdjustProgramCounter();
        DisplayTextSegments(true);
        DisplayDataSegments(false);
        // update registers window to make sure PC shows up!
        DisplayIntRegisters();
    }
}


void SpimView::file_ReloadFile()
{
    sim_ReinitializeSimulator();
    file_LoadFile();
}


void SpimView::rebuildRecentFilesMenu()
{
    ui->menuRecent_Files->clear();
    int i;
    for (i = 0; i < st_recentFilesLength; i++)
    {
        QAction* action = ui->menuRecent_Files->addAction(st_recentFiles[i]);
        QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(file_LoadFile()));
    }
}


void SpimView::file_SaveLogFile()
{
    static QDialog* saveLogFileDialog;
    static Ui::SaveLogFileDialog* slf;

    if (saveLogFileDialog == NULL)
    {
        saveLogFileDialog = new QDialog();
        slf = new Ui::SaveLogFileDialog();
        slf->setupUi(saveLogFileDialog);

        QFileDialog* fb = new QFileDialog(0,
                                          "Save To Log File",
                                          "",
                                          "Text files (*.txt);; All (*)");
        QObject::connect(slf->saveFileToolButton, SIGNAL(clicked()), fb, SLOT(exec()));
        QObject::connect(fb, SIGNAL(fileSelected(QString)), slf->SaveLineEdit, SLOT(setText(QString)));
    }

    if (saveLogFileDialog->exec() == QDialog::Accepted)
    {
        if (!slf->SaveLineEdit->text().isNull())
        {
            QFile file(slf->SaveLineEdit->text());
            file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream outFile(&file);

            if (slf->RegsCheckBox->isChecked())
            {
                outFile << ui->IntRegDockWidget->findChild<regTextEdit *>("IntRegTextEdit")->toPlainText();
                outFile << "\n\n";
            }
            if (slf->TextCheckBox->isChecked())
            {
                outFile << ui->TextSegDockWidget->findChild<textTextEdit *>("TextSegmentTextEdit")->toPlainText();
                outFile << "\n\n";
            }
            if (slf->DataCheckBox->isChecked())
            {
                outFile << ui->DataSegDockWidget->findChild<dataTextEdit *>("DataSegmentTextEdit")->toPlainText();
                outFile << "\n\n";
            }
            if (slf->ConsoleCheckBox->isChecked())
            {
                outFile << SpimConsole->toPlainText();
            }

            file.close();
        }
    }
}


void SpimView::file_Print()
{
    static QDialog* printFileDialog;
    static Ui::PrintWindowsDialog* pwd;

    if (printFileDialog == NULL)
    {
        printFileDialog = new QDialog();
        pwd = new Ui::PrintWindowsDialog();
        pwd->setupUi(printFileDialog);
    }

    if (printFileDialog->exec() == QDialog::Accepted)
    {
        QPrinter printer;
        QPrintDialog printDialog(&printer, this);
        printDialog.setWindowTitle("Print Windows");

        if (printDialog.exec() == QDialog::Accepted)
        {

            if (pwd->RegsCheckBox->isChecked())
            {
                ui->IntRegDockWidget->findChild<regTextEdit *>("IntRegTextEdit")->print(&printer);
            }
            if (pwd->TextCheckBox->isChecked())
            {
                ui->TextSegDockWidget->findChild<textTextEdit *>("TextSegmentTextEdit")->print(&printer);
            }
            if (pwd->DataCheckBox->isChecked())
            {
                ui->DataSegDockWidget->findChild<dataTextEdit *>("DataSegmentTextEdit")->print(&printer);
            }
            if (pwd->ConsoleCheckBox->isChecked())
            {
                SpimConsole->print(&printer);
            }
        }
    }
}


void SpimView::file_Exit()
{
#if 0
    QMessageBox msgBox(QMessageBox::Question, "Exit", "Exit?", QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if (msgBox.exec())
    {
        this->SaveStateAndExit(0);
    }
#endif
    this->SaveStateAndExit(0);
}



// Simulator menu
//

void SpimView::sim_ClearRegisters()
{
    initialize_registers();

    DisplayIntRegisters();
    DisplayFPRegisters();
}


void SpimView::sim_ReinitializeSimulator()
{
    write_output(message_out, "<hr>Memory and registers cleared\n\n");
    InitializeWorld();
    SpimConsole->Clear();
    initStack();

    SetOutputColor("green");
    write_startup_message();
    write_output(message_out, 
                 "QtSPIM is linked to the Qt library, which is distributed under the GNU Lesser General Public License version 3 and version 2.1.\n");
    SetOutputColor("black");

    CaptureIntRegisters();
    CaptureSFPRegisters();
    CaptureDFPRegisters();

    DisplayTextSegments(true);
    UpdateDataDisplay();

    // Scroll to top of windows
    //
    regTextEdit* rte = ui->IntRegDockWidget->findChild<regTextEdit *>("IntRegTextEdit");
    rte->verticalScrollBar()->setValue(rte->verticalScrollBar()->minimum());
    rte = ui->FPRegDockWidget->findChild<regTextEdit *>("FPRegTextEdit");
    rte->verticalScrollBar()->setValue(rte->verticalScrollBar()->minimum());
    textTextEdit* tte = ui->TextSegDockWidget->findChild<textTextEdit *>("TextSegmentTextEdit");
    tte->verticalScrollBar()->setValue(tte->verticalScrollBar()->minimum());
    dataTextEdit* dte = ui->DataSegDockWidget->findChild<dataTextEdit *>("DataSegmentTextEdit");
    dte->verticalScrollBar()->setValue(dte->verticalScrollBar()->minimum());
}


void SpimView::sim_SetRunParameters()
{
    QDialog d;
    Ui::SetRunParametersDialog srp;
    srp.setupUi(&d);

    // Default values:
    //
    srp.addressLineEdit->setText(QString("0x") + formatAddress(st_startAddress));
    srp.argsLineEdit->setText(st_commandLine);

    if (d.exec() == QDialog::Accepted)
    {
        if (!srp.addressLineEdit->text().isNull())
        {
            bool ok;
            st_startAddress = srp.addressLineEdit->text().toInt(&ok, 0);
        }
        if (!srp.argsLineEdit->text().isNull())
        {
            st_commandLine = srp.argsLineEdit->text();
        }
    }
}


void SpimView::sim_Run()
{
    initializePCAndStack();

    force_break = false;
    updateStatus(RUNNING);
    while (!force_break && programStatus == RUNNING)
    {
        executeProgram(PC, 100000, false, false);
        App->processEvents();   // Check for events in midst of long computation
    }
    updateStatus(IDLE);
    UpdateDataDisplay();
}


void SpimView::sim_Pause()
{
    force_break = true;
    updateStatus(PAUSED);
    UpdateDataDisplay();
}


void SpimView::sim_Stop()
{
    force_break = true;
    updateStatus(STOPPED);
    UpdateDataDisplay();
}


void SpimView::sim_SingleStep()
{
    initializePCAndStack();

    force_break = false;
    updateStatus(SINGLESTEP);
    executeProgram(PC, 1, false, false);
    UpdateDataDisplay();
}


void SpimView::initializePCAndStack()
{
    if ((programStatus == STOPPED && !force_break) || PC == 0)
    {
        if (st_startAddress == 0)
        {
            st_startAddress = starting_address();
        }
        PC = st_startAddress;
        initStack();
    }
}


void SpimView::initStack()
{
    // Prepend file name to arg list
    //
    initialize_stack((st_recentFiles[0] + " " + st_commandLine).toLocal8Bit().data());
}


void SpimView::updateStatus(PROGSTATE status)
{
    programStatus = status;
    switch (programStatus)
    {
    case IDLE:
        Window->statusBar()->showMessage("");
        break;

    case STOPPED:
        Window->statusBar()->showMessage("Stopped");
        break;

    case PAUSED:
        Window->statusBar()->showMessage("Paused");
        break;

    case RUNNING:
        Window->statusBar()->showMessage("Running");
        break;

    case SINGLESTEP:
        Window->statusBar()->showMessage("Single Step");
        break;

    default:
        Window->statusBar()->showMessage("?");
        break;
    }
}


void SpimView::executeProgram(mem_addr pc, int steps, bool display, bool contBkpt)
{
    bool continuable;
    bool breakpointEncountered = run_program(pc, steps, display, contBkpt, &continuable);

    highlightInstruction(PC);

    if (breakpointEncountered)
    {
        static QDialog* breakpointDialog;
        static Ui::BreakpointDialog* bpd;

        if (bpd == NULL)
        {
            breakpointDialog = new QDialog();
            bpd = new Ui::BreakpointDialog();
            bpd->setupUi(breakpointDialog);

            connect(bpd->continuePushButton, SIGNAL(clicked()), this, SLOT(continueBreakpoint()));
            connect(bpd->singleStepPushButton, SIGNAL(clicked()), this, SLOT(singleStepBreakpoint()));
            connect(bpd->abortPushButton, SIGNAL(clicked()), this, SLOT(abortBreakpoint()));
        }
        bpd->label->setText("Execution stopped at breakpoint at " + QString("0x") + formatAddress(PC));
        breakpointDialog->show();
    }
    else if (!continuable)
    {
        updateStatus(STOPPED);
    }
}


void SpimView::continueBreakpoint()
{
    bool continuable;
    run_program(PC, 1, false, true, &continuable); // Execute instruction replaced by breakpoint
    highlightInstruction(PC);
    UpdateDataDisplay();
    updateStatus((continuable == 1) ? RUNNING : STOPPED);
}


void SpimView::singleStepBreakpoint()
{
    bool continuable;
    run_program(PC, 1, false, true, &continuable); // Execute instruction replaced by breakpoint
    highlightInstruction(PC);
    UpdateDataDisplay();
    updateStatus((continuable == 1) ? SINGLESTEP : STOPPED);
}


void SpimView::abortBreakpoint()
{
    updateStatus(STOPPED);
}


void SpimView::sim_DisplaySymbols()
{
    WriteOutput("<br>");
    print_symbols();
}


void SpimView::sim_Settings()
{
    QDialog d;
    Ui::SpimSettingDialog sd;
    sd.setupUi(&d);

    sd.bareMachineCheckBox->setChecked(bare_machine);
    sd.pseudoInstCheckBox->setChecked(accept_pseudo_insts);
    sd.delayedBranchCheckBox->setChecked(delayed_branches);
    sd.delayedLoadCheckBox->setChecked(delayed_loads);
    sd.mappedIOCheckBox->setChecked(mapped_io);

    QObject::connect(sd.simplePushButton, SIGNAL(clicked()), sd.bareMachineCheckBox, SLOT(unset()));
    QObject::connect(sd.simplePushButton, SIGNAL(clicked()), sd.pseudoInstCheckBox, SLOT(set()));
    QObject::connect(sd.simplePushButton, SIGNAL(clicked()), sd.delayedBranchCheckBox, SLOT(unset()));
    QObject::connect(sd.simplePushButton, SIGNAL(clicked()), sd.delayedLoadCheckBox, SLOT(unset()));
    QObject::connect(sd.barePushButton, SIGNAL(clicked()), sd.bareMachineCheckBox, SLOT(set()));
    QObject::connect(sd.barePushButton, SIGNAL(clicked()), sd.pseudoInstCheckBox, SLOT(unset()));
    QObject::connect(sd.barePushButton, SIGNAL(clicked()), sd.delayedBranchCheckBox, SLOT(set()));
    QObject::connect(sd.barePushButton, SIGNAL(clicked()), sd.delayedLoadCheckBox, SLOT(set()));

    sd.loadExceptionHandlerCheckBox->setChecked(st_loadExceptionHandler);
    sd.exceptionHandlerLineEdit->setText(st_exceptionHandlerFileName);
    QFileDialog exceptionFileDialog(0,
                                    "Open Exception File",
                                    st_exceptionHandlerFileName,
                                    "Assembly (*.a *.s *.asm);;Text files (*.txt)");
    QObject::connect(sd.exceptionHandlerToolButton, SIGNAL(clicked()), &exceptionFileDialog, SLOT(exec()));
    QObject::connect(&exceptionFileDialog, SIGNAL(fileSelected(QString)),
                     sd.exceptionHandlerLineEdit, SLOT(setText(QString)));

    sd.recentFilesLineEdit->setText(QString::number(st_recentFilesLength, 10));
    sd.quietCheckBox->setChecked(quiet);
    sd.wordsPerLineEdit->setText(QString::number(st_numberOfWordsPerLine));

    sd.regWinFontLineEdit->setText(st_regWinFont.family());
    QFontDialog* regWinFontDialog = new QFontDialog(st_regWinFont);
    QObject::connect(sd.regWinFontToolButton, SIGNAL(clicked()), regWinFontDialog, SLOT(exec()));
    QObject::connect(regWinFontDialog, SIGNAL(fontSelected(QFont)), &sd, SLOT(setRegWinFont(QFont)));

    sd.regWinFontColorLineEdit->setText(st_regWinFontColor.name());
    QColorDialog* regWinColorDialog = new QColorDialog(st_regWinFontColor);
    QObject::connect(sd.regWinFontColorToolButton, SIGNAL(clicked()), regWinColorDialog, SLOT(exec()));
    QObject::connect(regWinColorDialog, SIGNAL(colorSelected(QColor)), &sd, SLOT(setRegWinColor(QColor)));

    sd.regWinBackgroundLineEdit->setText(st_regWinBackgroundColor.name());
    QColorDialog* regWinBackgroundDialog = new QColorDialog(st_regWinBackgroundColor);
    QObject::connect(sd.regWinBackgroundToolButton, SIGNAL(clicked()), regWinBackgroundDialog, SLOT(exec()));
    QObject::connect(regWinBackgroundDialog, SIGNAL(colorSelected(QColor)), &sd, SLOT(setRegWinBackground(QColor)));

    sd.textWinFontLineEdit->setText(st_textWinFont.family());
    QFontDialog* textWinFontDialog = new QFontDialog(st_textWinFont);
    QObject::connect(sd.textWinFontToolButton, SIGNAL(clicked()), textWinFontDialog, SLOT(exec()));
    QObject::connect(textWinFontDialog, SIGNAL(fontSelected(QFont)), &sd, SLOT(setTextWinFont(QFont)));

    sd.textWinFontColorLineEdit->setText(st_textWinFontColor.name());
    QColorDialog* textWinColorDialog = new QColorDialog(st_textWinFontColor);
    QObject::connect(sd.textWinFontColorToolButton, SIGNAL(clicked()), textWinColorDialog, SLOT(exec()));
    QObject::connect(textWinColorDialog, SIGNAL(colorSelected(QColor)), &sd, SLOT(setTextWinColor(QColor)));

    sd.textWinBackgroundLineEdit->setText(st_textWinBackgroundColor.name());
    QColorDialog* textWinBackgroundDialog = new QColorDialog(st_textWinBackgroundColor);
    QObject::connect(sd.textWinBackgroundToolButton, SIGNAL(clicked()), textWinBackgroundDialog, SLOT(exec()));
    QObject::connect(textWinBackgroundDialog, SIGNAL(colorSelected(QColor)), &sd, SLOT(setTextWinBackground(QColor)));

    if (d.exec() == QDialog::Accepted)
    {
        bare_machine = sd.bareMachineCheckBox->isChecked();
        accept_pseudo_insts = sd.pseudoInstCheckBox->isChecked();
        delayed_branches = sd.delayedBranchCheckBox->isChecked();
        delayed_loads = sd.delayedLoadCheckBox->isChecked();
        mapped_io = sd.mappedIOCheckBox->isChecked();
        st_numberOfWordsPerLine = sd.wordsPerLineEdit->text().toInt();
        DisplayDataSegments(1);

        st_loadExceptionHandler = sd.loadExceptionHandlerCheckBox->isChecked();
        st_exceptionHandlerFileName = sd.exceptionHandlerLineEdit->text();

        st_recentFilesLength = sd.recentFilesLineEdit->text().toInt();
        quiet = sd.quietCheckBox->isChecked();
        if (st_recentFilesLength <= 0 || st_recentFilesLength > 20)
        {
            st_recentFilesLength = 4;
        }

        if (sd.regWinFont != NULL)
        {
            st_regWinFont = *sd.regWinFont;
        }
        if (sd.regWinColor != NULL)
        {
            st_regWinFontColor = *sd.regWinColor;
        }
        if (sd.regWinBackground != NULL)
        {
            st_regWinBackgroundColor = *sd.regWinBackground;
        }

        if (sd.textWinFont != NULL)
        {
            st_textWinFont = *sd.textWinFont;
        }
        if (sd.textWinColor != NULL)
        {
            st_textWinFontColor = *sd.textWinColor;
        }
        if (sd.textWinBackground != NULL)
        {
            st_textWinBackgroundColor = *sd.textWinBackground;
        }

        DisplayTextSegments(true);
        UpdateDataDisplay();
    }
}


// Registers menu
//

void SpimView::reg_DisplayBinary()
{
    st_regDisplayBase = 2;
    setCheckedRegBase(st_regDisplayBase);
    DisplayIntRegisters();
    DisplayFPRegisters();
}


void SpimView::reg_DisplayDecimal()
{
    st_regDisplayBase = 10;
    setCheckedRegBase(st_regDisplayBase);
    DisplayIntRegisters();
    DisplayFPRegisters();
}


void SpimView::reg_DisplayHex()
{
    st_regDisplayBase = 16;
    setCheckedRegBase(st_regDisplayBase);
    DisplayIntRegisters();
    DisplayFPRegisters();
}


int SpimView::setCheckedRegBase(int base)
{
    return setBaseInternal(base,
                           ui->action_Reg_DisplayBinary,
                           ui->action_Reg_DisplayDecimal,
                           ui->action_Reg_DisplayHex);
}


int SpimView::setBaseInternal(int base, QAction* actionBinary, QAction* actionDecimal, QAction* actionHex)
{
    actionBinary->setChecked(false);
    actionDecimal->setChecked(false);
    actionHex->setChecked(false);

    switch (base)
    {
    case 2:
        actionBinary->setChecked(true);
        return base;

    case 10:
        actionDecimal->setChecked(true);
        return base;

    case 16:
        actionHex->setChecked(true);
        return base;

    default:
        actionHex->setChecked(true);
        return 16;
    }
}


// Text menu
//

void SpimView::text_DisplayUserText()
{
    bool changed = st_showUserTextSegment == ui->action_Text_DisplayUserText->isChecked();
    st_showUserTextSegment = ui->action_Text_DisplayUserText->isChecked();
    DisplayTextSegments(changed);
}


void SpimView::text_DisplayKernelText()
{
    bool changed = st_showKernelTextSegment == ui->action_Text_DisplayKernelText->isChecked();
    st_showKernelTextSegment = ui->action_Text_DisplayKernelText->isChecked();
    DisplayTextSegments(changed);
}


void SpimView::text_DisplayComments()
{
    bool changed = st_showTextComments == ui->action_Text_DisplayComments->isChecked();
    st_showTextComments = ui->action_Text_DisplayComments->isChecked();
    DisplayTextSegments(changed);
}


void SpimView::text_DisplayInstructionValue()
{
    bool changed = st_showTextDisassembly == ui->action_Text_DisplayInstructionValue->isChecked();

    st_showTextDisassembly = ui->action_Text_DisplayInstructionValue->isChecked();
    DisplayTextSegments(changed);
}



// Data menu
//

void SpimView::data_DisplayUserData()
{
    st_showUserDataSegment = ui->action_Data_DisplayUserData->isChecked();
    DisplayDataSegments(true);
}


void SpimView::data_DisplayUserStack()
{
    st_showUserStackSegment = ui->action_Data_DisplayUserStack->isChecked();
    DisplayDataSegments(true);
}


void SpimView::data_DisplayKernelData()
{
    st_showKernelDataSegment = ui->action_Data_DisplayKernelData->isChecked();
    DisplayDataSegments(true);
}


void SpimView::data_DisplayBinary()
{
    st_dataSegmentDisplayBase = 2;
    setCheckedDataSegmentDisplayBase(st_dataSegmentDisplayBase);
    DisplayDataSegments(true);
}


void SpimView::data_DisplayDecimal()
{
    st_dataSegmentDisplayBase = 10;
    setCheckedDataSegmentDisplayBase(st_dataSegmentDisplayBase);
    DisplayDataSegments(true);
}


void SpimView::data_DisplayHex()
{
    st_dataSegmentDisplayBase = 16;
    setCheckedDataSegmentDisplayBase(st_dataSegmentDisplayBase);
    DisplayDataSegments(true);
}


int SpimView::setCheckedDataSegmentDisplayBase(int base)
{
    return setBaseInternal(base,
                           ui->action_Data_DisplayBinary,
                           ui->action_Data_DisplayDecimal,
                           ui->action_Data_DisplayHex);
}


// Window menu
//

void winUpDown(QObject* sender, QWidget* widget)
{
    QAction* act = (QAction*)sender;
    if (act->isChecked())
    {
        widget->show();
    }
    else
    {
        widget->hide();
    }
}


void SpimView::win_IntRegisters()
{
    winUpDown(sender(), ui->IntRegDockWidget);
}


void SpimView::win_FPRegisters()
{
    winUpDown(sender(), ui->FPRegDockWidget);
}


void SpimView::win_TextSegment()
{
    winUpDown(sender(), ui->TextSegDockWidget);
}


void SpimView::win_DataSegment()
{
    winUpDown(sender(), ui->DataSegDockWidget);
}


void SpimView::win_Console()
{
    winUpDown(sender(), SpimConsole);
}


void SpimView::win_Tile()
{
    ui->IntRegDockWidget->show();
    ui->action_Win_IntRegisters->setChecked(true);
    ui->FPRegDockWidget->show();
    ui->action_Win_FPRegisters->setChecked(true);
    ui->TextSegDockWidget->show();
    ui->action_Win_TextSegment->setChecked(true);
    ui->DataSegDockWidget->show();
    ui->action_Win_DataSegment->setChecked(true);

    ui->IntRegDockWidget->setFloating(false);
    ui->FPRegDockWidget->setFloating(false);
    tabifyDockWidget(ui->FPRegDockWidget, ui->IntRegDockWidget);
    ui->TextSegDockWidget->setFloating(false);
    ui->DataSegDockWidget->setFloating(false);
    tabifyDockWidget(ui->DataSegDockWidget, ui->TextSegDockWidget);
}


// Help menu
//

void SpimView::help_ViewHelp()
{
    QProcess *process = new QProcess;
    QStringList args;

    char* helpFile[] = {"help/qtspim.qhc",      // Windows
                        "/Applications/QtSpim.app/Contents/Resources/doc/qtspim.qhc", // Mac
                        "/usr/lib/qtspim/help/qtspim.qhc", // Linux
                        0};

    int i;
    for (i = 0; helpFile[i] != 0; i += 1)
    {
        QFileInfo fi1(helpFile[i]);
        if (fi1.exists())
        {
            args << QLatin1String("-collectionFile") << QLatin1String(helpFile[i]);
            break;
        }
    }

    if (helpFile[i] == 0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Cannot find QtSpim help file. Check installation."));
        msgBox.exec();
        return;
    }

    char* assistant[] = {"assistant", // Windows
                         "/Applications/QtSpim.app/Contents/MacOS/Assistant", // Mac
                         "/usr/lib/qtspim/bin/assistant", // Linux
                         0};

    process->start(QLatin1String(assistant[i]), args);
    if (!process->waitForStarted())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Cannot start help browser (Qt assistant). Check installation."));
        msgBox.exec();
    }
}


void SpimView::help_AboutSPIM()
{
    QMessageBox box(QMessageBox::NoIcon,
                    tr("About QtSpim"),
                    QString("<span style='font-size: 16pt;'>"
                            "<center><strong>QtSpim</strong></center>"
                            "<center><img src=':/icons/qtspim.png'>"
                            "<span style='font-size: 10pt;'>")
                    + QString("<p>") + QString(SPIM_VERSION) + QString("</p>")
                    + QString ("<p>SPIM is a simulator of the MIPS R3000 processor.</p>"
                               "<p>Copyright (c) 1990-2015, James R. Larus (larus@larusstone.org).</p>"
                               "<p>SPIM is distributed under a BSD license.</p>"
                               "<p>For more information, source code, and binaries:</p>"
                               "<p><a href='https://sourceforge.net/projects/spimsimulator/'>https://sourceforge.net/projects/spimsimulator/</a></p>"
                               "<p>QtSPIM is linked to the Qt library, which is distributed under the GNU Lesser General Public License version 3 and GNU Lesser General Public License version 2.1.</p>"
"<p><a href='http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html'>GNU Lesser General Public License, version 2.1</a></p>"
"<p><a href='http://www.gnu.org/licenses/lgpl-3.0.html'>GNU Lesser General Public License, version 3</a></p>"
"</span>"),
                    QMessageBox::Ok);
    box.exec();
}
