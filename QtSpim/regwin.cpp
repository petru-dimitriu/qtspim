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


#include "spimview.h"
#include "ui_spimview.h"

#include <QMessageBox>
#include <QRegExp>
#include <QStringBuilder>
#define QT_USE_FAST_CONCATENATION
#include <QInputDialog>
#include <QScrollBar>
#include "ui_changevalue.h"

//
// Integer registers window
//

void SpimView::DisplayIntRegisters()
{
    regTextEdit* te = ui->IntRegDockWidget->findChild<regTextEdit *>("IntRegTextEdit");
    te->isIntRegs = true;
    QString windowContents = windowFormattingStart(st_regWinFont, st_regWinFontColor, st_regWinBackgroundColor);
    int scrollPosition = te->verticalScrollBar()->value();

    windowContents += formatSpecialIntRegister(PC, "PC", PC != oldPC);
    windowContents += formatSpecialIntRegister(CP0_EPC, "EPC", CP0_EPC != oldEPC);
    windowContents += formatSpecialIntRegister(CP0_Cause, "Cause", CP0_Cause != oldCause);
    windowContents += formatSpecialIntRegister(CP0_BadVAddr, "BadVAddr", CP0_BadVAddr != oldBadVAddr);
    windowContents += formatSpecialIntRegister(CP0_Status, "Status", CP0_Status != oldStatus);
    windowContents += "<br />";
    windowContents += formatSpecialIntRegister(HI, "HI", HI != oldHI);
    windowContents += formatSpecialIntRegister(LO, "LO", LO != oldLO);

    windowContents += "<br />";

    int i;
    for (i = 0; i < 32; i++) {
        windowContents += formatIntRegister(i, R[i], int_reg_names[i], R[i] != oldR[i]);
    }
    windowContents += windowFormattingEnd();

    te->clear();
    te->appendHtml(windowContents);
    ui->IntRegDockWidget->setWindowTitle(QString(tr("Int Regs ["))
                                         + QString::number(st_regDisplayBase)
                                         + QString("] "));

    te->verticalScrollBar()->setValue(scrollPosition);
    CaptureIntRegisters();
}


QString SpimView::formatSpecialIntRegister(int value, char* name, bool changed)
{
    return formatReg(QString("<b>") % QString(name) % nnbsp(8 - strlen(name)) % QString("</b>"),
                     formatInt(value),
                     changed);
}


QString SpimView::formatIntRegister(int regNum, int value, char* name, bool changed)
{
    return formatReg(QString("<b>R") % QString::number(regNum, 10) % (regNum < 10 ? "&nbsp;" : "")
                     % QString("&nbsp;[") % QString(name) % QString("]</b>"),
                     formatInt(value),
                     changed);
}


void SpimView::CaptureIntRegisters()
{
    int i;
    for (i = 0; i < R_LENGTH; i++)
    {
        oldR[i] = R[i];
    }
    oldPC = PC;
    oldEPC = CP0_EPC;
    oldCause = CP0_Cause;
    oldBadVAddr = CP0_BadVAddr;
    oldStatus = CP0_Status;
    oldHI = HI;
    oldLO = LO;
}



//
// Floating point registers
//
void SpimView::DisplayFPRegisters()
{
    regTextEdit* te = ui->FPRegDockWidget->findChild<regTextEdit *>("FPRegTextEdit");
    te->isIntRegs = false;
    QString windowContents = windowFormattingStart(st_regWinFont, st_regWinFontColor, st_regWinBackgroundColor);
    int scrollPosition = te->verticalScrollBar()->value();

    windowContents += formatSFPRegisters() % formatDFPRegisters() % windowFormattingEnd();

    te->clear();
    te->appendHtml(windowContents);

    te->verticalScrollBar()->setValue(scrollPosition);
}


//
// Single precision floating point registers window
//

QString SpimView::formatSFPRegisters()
{
    QString windowContents = "";

    windowContents += formatSpecialSFPRegister(FIR, "FIR", FIR != oldFIR);
    windowContents += formatSpecialSFPRegister(FCSR, "FCSR", FCSR != oldFCSR);
    windowContents += formatSpecialSFPRegister(FCCR, "FCCR", FCCR != oldFCCR);
    windowContents += formatSpecialSFPRegister(FEXR, "FEXR", FEXR != oldFEXR);

    windowContents += "<br /><center><b>Single Precision</b></center>";

    int i;
    for (i = 0; i < FGR_LENGTH; i++) {
        windowContents += formatSFPRegister(i, FPR_S(i), FPR_S(i) != oldFPR_S[i]);
    }

    CaptureSFPRegisters();

    return windowContents;
}


void SpimView::CaptureSFPRegisters()
{
    int i;
    for (i = 0; i < FGR_LENGTH; i++)
    {
        oldFPR_S[i] = FPR_S(i);
    }
    oldFIR = FIR;
    oldFCSR = FCSR;
    oldFCCR = FCCR;
    oldFEXR = FEXR;
}


QString SpimView::formatSpecialSFPRegister(int value, char* name, bool changed)
{
    return formatReg(QString("<b>") % QString(name) % nnbsp(7 - strlen(name)) % QString("</b>"),
                     formatInt(value),
                     changed);
}


QString SpimView::formatSFPRegister(int regNum, float value, bool changed)
{
    return formatReg(QString("<b>FG") % QString::number(regNum, 10) % (regNum < 10 ? "&nbsp;</b>" : "</b>"),
                     formatFloat(value),
                     changed);
}


//
// Double precision floating point registers window
//

QString SpimView::formatDFPRegisters()
{
    QString windowContents = QString("<br /><center><b>Double Precision</b></center>");

    int i;
    for (i = 0; i < FPR_LENGTH; i += 1)
    {
        windowContents += formatDFPRegister(2 * i, FPR_D(2 * i), FPR_D(2 * i) != oldFPR_D[i]);
    }

    CaptureDFPRegisters();

    return windowContents;
}


void SpimView::CaptureDFPRegisters()
{
    int i;
    for (i = 0; i < FPR_LENGTH; i += 1)
    {
        oldFPR_D[i] = FPR_D(2 * i);
    }
}


QString SpimView::formatDFPRegister(int regNum, double value, bool changed)
{
    return formatReg(QString("<b>FP") % QString::number(regNum, 10) % (regNum < 10 ? "&nbsp;</b>" : "</b>"),
                     formatDouble(value),
                     changed);
}


//
// Common register functionality
//

QString SpimView::formatInt(int value)
{
    QString str = QString::number(value, st_regDisplayBase);

    // Negative numbers are sign-extended to 64 bits in base 2 or 16; just return the non-sign
    // extended 32 bit value.
    if (st_regDisplayBase == 16)
    {
        str = str.right(8);
    }
    else if (st_regDisplayBase == 2)
    {
        str = str.right(32);
    }

    return str;
}


QString SpimView::formatFloat(float value)
{
    if (st_regDisplayBase == 16 || st_regDisplayBase == 2)
    {
        int* ival = (int*)&value;
        return QString::number(*ival, st_regDisplayBase);
    }
    else
    {
        return QString::number(value, 'f', 6);
    }
}


QString SpimView::formatDouble(double value)
{
    if (st_regDisplayBase == 16 || st_regDisplayBase == 2)
    {
        qlonglong* ival = (qlonglong*)&value;
        return QString::number(*ival, st_regDisplayBase);
    }
    else
    {
        return QString::number(value, 'f', 6);
    }
}


QString SpimView::formatReg(QString name, QString value, bool changed)
{
    return registerBefore(changed)
        % name
        % QString("&nbsp;=&nbsp;")
        % value
        % QString("<br>")
        % registerAfter(changed);
}


QString SpimView::registerBefore(bool changed)
{
    return QString(changed && st_colorChangedRegisters
                   ? QString("<font color='") % QString(st_changedRegisterColor) % QString("'>")
                   : QString(""));
}


QString SpimView::registerAfter(bool changed)
{
    return QString(changed && st_colorChangedRegisters ? "</font>" : "");
}


QString SpimView::nnbsp(int n)
{
    QString str = "";
    int i;
    for (i = 0; i < n; i++)
    {
        str += "&nbsp;";
    }
    return str;
}


//
// Change register value
//

regTextEdit::regTextEdit()
{
    action_Context_ChangeValue = new QAction(this);
    action_Context_ChangeValue->setObjectName("action_ChangeValue");
    action_Context_ChangeValue->setText(tr("Change Register Contents"));
}


void regTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu *menu = createStandardContextMenu();
    menu->addSeparator();

    menu->addAction(Window->ui->action_Reg_DisplayBinary);
    menu->addAction(Window->ui->action_Reg_DisplayDecimal);
    menu->addAction(Window->ui->action_Reg_DisplayHex);

    menu->addSeparator();
    menu->addAction(action_Context_ChangeValue);
    contextGlobalPos = event->globalPos();

    menu->exec(event->globalPos());
}


void regTextEdit::changeValue()
{
    int base = Window->RegDisplayBase();
    int reg = regAtPos("R");
    if (reg != -1)
    {
        QString val = promptForNewValue("New value for R" + QString::number(reg, 10), &base);
        bool ok;
        int newRegVal = convertIntLiteral(val, base, &ok);

        if (ok)
        {
            R[reg] = newRegVal;
        }
    }
    else
    {
        int reg = regAtPos("FG");
        if (reg != -1)
        {
            QString val = promptForNewValue("New value for FG" + QString::number(reg, 10), &base);
            bool ok;
            float newRegVal;
            if (base == 10)
            {
                newRegVal = val.toFloat(&ok);
            }
            else
            {
                int newIntRegVal = val.toULong(&ok, base); // Read as hex; treat bits -- not value -- as float
                void* ptr = &newIntRegVal;
                newRegVal = *(float*)ptr;
            }

            if (ok)
            {
                FGR[reg] = newRegVal;
            } else
            {
                QMessageBox msgBox;
                msgBox.setText("Bad FP value: " + val);
                msgBox.exec();
            }
        }
        else
        {
            int reg = regAtPos("FP");
            if (reg != -1)
            {
                QString val = promptForNewValue("New value for FP" + QString::number(reg, 10), &base);
                bool ok;
                float newRegVal;
                if (base == 10)
                {
                    newRegVal = val.toDouble(&ok);
                }
                else
                {
                    qlonglong newIntRegVal = val.toULongLong(&ok, base); // Read as hex; treat bits -- not value -- as double
                    void* ptr = &newIntRegVal;
                    newRegVal = *(double*)ptr;
                }

                if (ok)
                {
                    FPR[reg] = newRegVal;
                } else
                {
                    QMessageBox msgBox;
                    msgBox.setText("Bad FP value: " + val);
                    msgBox.exec();
                }
            }
            else
            {
                QString reg = strAtPos("([A-Za-z]+)");
                if (reg != "")
                {
                    QString val = promptForNewValue("New value for " + reg, &base);
                    bool ok;
                    int newRegVal = convertIntLiteral(val, base, &ok);

                    if (ok)
                    {
                        if (reg == "PC")
                        {
                            PC = newRegVal;
                        }
                        else if (reg == "EPC")
                        {
                            CP0_EPC = newRegVal;
                        }
                        else if (reg == "Cause")
                        {
                            CP0_Cause = newRegVal;
                        }
                        else if (reg == "BadVAddr")
                        {
                            CP0_BadVAddr = newRegVal;
                        }
                        else if (reg == "Status")
                        {
                            CP0_Status = newRegVal;
                        }
                        else if (reg == "HI")
                        {
                            HI = newRegVal;
                        }
                        else if (reg == "LO")
                        {
                            LO = newRegVal;
                        }
                        else if (reg == "FIR")
                        {
                            FIR = newRegVal;
                        }
                        else if (reg == "FCSR")
                        {
                            FCSR = newRegVal;
                        }
                        else if (reg == "FCCR")
                        {
                            FCCR = newRegVal;
                        }
                        else if (reg == "FEXR")
                        {
                            FEXR = newRegVal;
                        }
                    }
                }
            }
        }
    }
    Window->DisplayIntRegisters();
    Window->DisplayFPRegisters();
}


int regTextEdit::convertIntLiteral(QString val, int base, bool *ok)
{
    int newRegVal = 0;

    if (base == 10) {
        newRegVal = val.toLong(ok, base);  // decimal is signed
    } else
    {
        newRegVal = val.toULong(ok, base); // hex is unsigned
    }

    if (!*ok)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Bad ") + (base == 16 ? "hex" : "decimal") + " register value: " + val);
        msgBox.exec();
    }

    return newRegVal;
}


int regTextEdit::regAtPos(QString prefix)
{
    QString regStr = strAtPos(prefix + "([0-9]+)");    // Register name
    if (regStr != "")
    {
        return regStr.toInt();
    }
    else
    {
        return -1;
    }
}


QString regTextEdit::strAtPos(QString pattern)
{
    // Position of context menu is location user right-click. Find the line at this point
    // and compute the address of the memory location the user clicked on.
    //
    QPoint mouseViewportPos = this->viewport()->mapFromGlobal(contextGlobalPos);
    QTextCursor mouseCursor = this->cursorForPosition(mouseViewportPos);

    mouseCursor.select(QTextCursor::LineUnderCursor);
    QString line = mouseCursor.selectedText();

    QRegExp rx(pattern);

    rx.indexIn(line);
    return rx.cap(1);
}


QString promptForNewValue(QString text, int* base)
{
    QDialog d;
    Ui::ChangeValueDialog cvd;
    cvd.setupUi(&d);

    cvd.label->setText(text);
    if (*base == -1)
    {
        cvd.hexRadioButton->setVisible(false);
        cvd.decimalRadioButton->setVisible(false);
    }
    else
    {
        cvd.hexRadioButton->setVisible(true);
        cvd.decimalRadioButton->setVisible(true);

        cvd.hexRadioButton->setChecked(*base == 16);
        cvd.decimalRadioButton->setChecked(*base == 10);
    }

    if (d.exec() == QDialog::Accepted)
    {
        if (cvd.hexRadioButton->isChecked())
        {
            *base = 16;
        }
        else if (cvd.decimalRadioButton->isChecked())
        {
            *base = 10;
        }
        return cvd.answerLineEdit->text();
    }
    else
    {
        return QString("");
    }
}


void regTextEdit::closeEvent(QCloseEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        if (isIntRegs)
        {
            Window->ui->action_Win_IntRegisters->setChecked(false);
        }
        else
        {
            Window->ui->action_Win_FPRegisters->setChecked(false);
        }
    }
    event->accept();
}


void regTextEdit::hideEvent(QHideEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        if (isIntRegs)
        {
            Window->ui->action_Win_IntRegisters->setChecked(false);
        }
        else
        {
            Window->ui->action_Win_FPRegisters->setChecked(false);
        }
    }
    event->accept();
}


void regTextEdit::showEvent(QShowEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        if (isIntRegs)
        {
            Window->ui->action_Win_IntRegisters->setChecked(true);
        }
        else
        {
            Window->ui->action_Win_FPRegisters->setChecked(true);
        }
    }
    event->accept();
}
