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
#include <QContextMenuEvent>
#include <QStringBuilder>
#define QT_USE_FAST_CONCATENATION
#include <QInputDialog>
#include <QScrollBar>


#define WORD_WIDTH_10 10
#define WORD_WIDTH_16 8
#define WORD_WIDTH_DEFAULT 32


//
// Data segment window
//

void SpimView::DisplayDataSegments(bool force)
{
    if (force || data_modified)
    {
        dataTextEdit* te = ui->DataSegDockWidget->findChild<dataTextEdit *>("DataSegmentTextEdit");
        QString windowContents = windowFormattingStart(st_textWinFont, st_textWinFontColor, st_textWinBackgroundColor);
        int scrollPosition = te->verticalScrollBar()->value();

        windowContents += formatUserDataSeg() % formatUserStack() % formatKernelDataSeg() % windowFormattingEnd();

        te->clear();
        te->appendHtml(windowContents);

        te->verticalScrollBar()->setSliderPosition(scrollPosition);
    }
    data_modified = false;
}


QString SpimView::formatUserDataSeg()
{
    if (st_showUserDataSegment)
    {
        return formatSegLabel(tr("User data segment"), DATA_BOT, data_top)
            % formatMemoryContents(DATA_BOT, data_top);
    }
    else
    {
        return QString("");
    }
}


QString SpimView::formatUserStack()
{
    if (st_showUserStackSegment)
    {
        return formatSegLabel(tr("<br>User Stack"), ROUND_DOWN(R[29], BYTES_PER_WORD), STACK_TOP)
            % formatMemoryContents(ROUND_DOWN(R[29], BYTES_PER_WORD), STACK_TOP);
    }
    else
    {
        return QString("");
    }
}


QString SpimView::formatKernelDataSeg()
{
    if (st_showKernelDataSegment)
    {
        return formatSegLabel(tr("<br>Kernel data segment"), K_DATA_BOT, k_data_top)
            % formatMemoryContents(K_DATA_BOT, k_data_top);
    }
    else
    {
        return QString("");
    }
}

#define WORDS_PER_LINE 1
#define BYTES_PER_LINE (st_numberOfWordsPerLine * BYTES_PER_WORD)


QString SpimView::formatMemoryContents(mem_addr from, mem_addr to)
{
    mem_addr i = ROUND_UP(from, BYTES_PER_WORD);
    QString windowContents = formatPartialQuadWord(i, to);
    i = ROUND_UP(i, BYTES_PER_LINE);

    for ( ; i < to; )
    {
        mem_word val;

        /* Count consecutive zero words */
        int j;
        for (j = 0; (i + (uint32) j * BYTES_PER_WORD) < to; j += 1)
	{
            val = read_mem_word(i + (uint32) j * BYTES_PER_WORD);
            if (val != 0)
	    {
                break;
	    }
	}

        if (j >= WORDS_PER_LINE * 5)
	{
            /* Blocks of 2*WORDS_PER_LINE zero words  */
            windowContents += QString("[") % formatAddress(i)
                % QString("]..[") % formatAddress(i + (uint32) j * BYTES_PER_WORD - 1)
                % QString("]") % nnbsp(2) % QString("00000000<br>");

            i = i + (uint32) j * BYTES_PER_WORD;
            windowContents += formatPartialQuadWord(i, to);
            i = ROUND_UP(i, BYTES_PER_LINE);
	}
        else
	{
            windowContents += QString("[") % formatAddress(i) % "]" % nnbsp(2);
            mem_addr j = i;
            do
	    {
                val = read_mem_word(i);
                windowContents += nnbsp(2) % formatWord(val, st_dataSegmentDisplayBase);
                i += BYTES_PER_WORD;
	    }
            while ((i % BYTES_PER_LINE) != 0 && i < to);

            windowContents += nnbsp(2) % formatAsChars(j, i) % QString("<br>");
	}
    }
    return windowContents;
}


QString SpimView::formatPartialQuadWord(mem_addr from, mem_addr to)
{
    QString windowContents = QString("");

    if ((from % BYTES_PER_LINE) != 0 && from < to)
    {
        windowContents += QString("[") % formatAddress(from) % QString("]") % nnbsp(2);

        mem_addr a;
        for (a = from; (a % BYTES_PER_LINE) != 0 && from < to; a += BYTES_PER_WORD)
	{
            mem_word val = read_mem_word(a);
            windowContents += nnbsp(2) % formatWord(val, st_dataSegmentDisplayBase);
	}

        windowContents += formatAsChars(from, a) % QString("<br>");
    }

    return windowContents;
}


QString SpimView::formatAsChars(mem_addr from, mem_addr to)
{
    QString windowContents = nnbsp(2);

    if (to - from != BYTES_PER_LINE)
    {
        int missing = (BYTES_PER_LINE - (to - from)) / BYTES_PER_WORD;
        windowContents += nnbsp(2);
        switch (st_dataSegmentDisplayBase)
        {
        case 10: windowContents += nnbsp(missing * (WORD_WIDTH_10 + 2)); break;
        case 16: windowContents += nnbsp(missing * (WORD_WIDTH_16 + 2)); break;
        default: windowContents += nnbsp(missing * (WORD_WIDTH_DEFAULT + 2)); break;
        }
    }

    for (mem_addr a = from; a < to; a += 1)
    {
        mem_word val = read_mem_byte(a);
        windowContents += formatChar(val) % " ";
    }

    return windowContents;
}


//
// Utility functions
//

QString formatAddress(mem_addr addr)
{
    return QString::number(addr, 16).rightJustified(8, '0');
}


QString formatWord(mem_word word, int base)
{
    int width = 0;
    switch (base)
    {
    case 10: width = WORD_WIDTH_10; break;
    case 16: width = WORD_WIDTH_16; break;
    default: width = WORD_WIDTH_DEFAULT; break;
    }
    QString str = QString::number(word, base);
    str.remove(0, str.length() - width); // Negative hex number proceeded by 0xffffffff

    if (str[0] == '-')                   // decimal starting with a negative sign
        return str.rightJustified(width, ' ').replace(QRegExp(" "), "&nbsp;"); // Don't zero pad
    else
        return str.rightJustified(width, '0');
}


QString formatChar(int chr)
{
    if (chr == ' ')
    {
        return QString("&nbsp;");
    }
    else if (chr == '<')
    {
        return QString("&lt;");
    }
    else if (chr == '>')
    {
        return QString("&gt;");
    }
    else if (chr == '&')
    {
        return QString("&amp;");
    }
    else if (chr > ' ' && chr <= '~') // Printable ascii chars
    {
        return QString(QChar(chr));
    }
    else
    {
        return QString(QChar('.'));
    }
}


QString formatSegLabel(QString segName, mem_addr low, mem_addr high)
{
    return QString("<center><b>") % segName
        % QString(" [") % formatAddress(low) % QString("]..[") % formatAddress(high)
        % QString("]</b></center>");
}


//
// Change memory value
//

dataTextEdit::dataTextEdit()
{
    action_Context_ChangeValue = new QAction(this);
    action_Context_ChangeValue->setObjectName("action_ChangeValue");
    action_Context_ChangeValue->setText("Change Memory Contents");
}


void dataTextEdit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu *menu = createStandardContextMenu();
    menu->addSeparator();

    menu->addAction(Window->ui->action_Data_DisplayBinary);
    menu->addAction(Window->ui->action_Data_DisplayDecimal);
    menu->addAction(Window->ui->action_Data_DisplayHex);

    menu->addSeparator();
    menu->addAction(action_Context_ChangeValue);
    contextGlobalPos = event->globalPos();

    menu->exec(event->globalPos());
}


void dataTextEdit::changeValue()
{
    QTextCursor cursor;
    mem_addr addr = addrFromPos(&cursor);
    if (addr != 0)
    {
        int base = Window->DataDisplayBase();
        QString val = promptForNewValue(tr("New value for ") + formatAddress(addr), &base);

        bool ok;
        int newMemVal = 0;
        if (base == 10) {
            newMemVal = val.toLong(&ok, base);  // decimal is signed
        } else
        {
            newMemVal = val.toULong(&ok, base); // hex is unsigned
        }

        if (ok)
        {
            set_mem_word(addr, newMemVal);
        } else
        {
            QMessageBox msgBox;
            msgBox.setText(QString(tr("Bad ")) + (base == 16 ? tr("hex") : tr("decimal")) + tr(" memory value: ") + val);
            msgBox.exec();
        }

        Window->DisplayDataSegments(true);
    }
}


int dataTextEdit::addrFromPos(QTextCursor* cursor)
{
    // Position of context menu is location user right-click. Find the line at this point
    // and compute the address of the memory location the user clicked on.
    //
    QPoint mouseViewportPos = this->viewport()->mapFromGlobal(contextGlobalPos);
    QTextCursor mouseCursor = this->cursorForPosition(mouseViewportPos);
    *cursor = mouseCursor;

    cursor->select(QTextCursor::LineUnderCursor);
    QString line = cursor->selectedText();

    QRegExp rx("\\[([0-9a-fA-F]{8})\\]"); // Address of instruction

    rx.indexIn(line);
    QString addrStr = rx.cap(1);
    if (addrStr != "")
    {
        bool ok;
        mem_addr addr = addrStr.toUInt(&ok, 16);
        if (ok)
        {
            // [...]<sp><sp><sp><sp>#<sp><sp>#<sp><sp>#<sp><sp>#<sp><sp>
            //
            int offset = mouseCursor.position() - cursor->anchor(); // Start of line to mouse location
            line.truncate(offset);                                  // Remove address
            line.remove(0, 14);                                     // Remove line after mouse position

            QRegExp rx2("^([0-9a-fA-F]+\\s*)?([0-9a-fA-F]+\\s*)?([0-9a-fA-F]+\\s*)?([0-9a-fA-F]+\\s*)?");
            rx2.indexIn(line, 0);
            return addr
                + (rx2.cap(1) == "" ? 0 : 0)
                + (rx2.cap(2) == "" ? 0 : BYTES_PER_WORD)
                + (rx2.cap(3) == "" ? 0 : BYTES_PER_WORD)
                + (rx2.cap(4) == "" ? 0 : BYTES_PER_WORD);
        }
    }
    return 0;
}


void dataTextEdit::closeEvent(QCloseEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        Window->ui->action_Win_DataSegment->setChecked(false);
    }
    event->accept();
}


void dataTextEdit::hideEvent(QHideEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        Window->ui->action_Win_DataSegment->setChecked(false);
    }
    event->accept();
}


void dataTextEdit::showEvent(QShowEvent* event)
{
    if (Window != NULL)  // Event can occur while window is being constructed; ignore in that case
    {
        Window->ui->action_Win_DataSegment->setChecked(true);
    }
    event->accept();
}
