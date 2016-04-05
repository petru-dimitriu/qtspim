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


#ifndef SPIM_SETTING_DIALOG_H
#define SPIM_SETTING_DIALOG_H

#include <QObject>
#include <QFont>
#include <QColor>

#include "ui_settings.h"

namespace Ui{

    class SpimSettingDialog : public QObject, public SettingDialog
{
Q_OBJECT

public:
    SpimSettingDialog() : SettingDialog()
    {
        regWinFont = NULL;
        regWinColor = NULL;
        regWinBackground = NULL;

        textWinFont = NULL;
        textWinColor = NULL;
        textWinBackground = NULL;
    }

    QFont* regWinFont;
    QColor* regWinColor;
    QColor* regWinBackground;

    QFont* textWinFont;
    QColor* textWinColor;
    QColor* textWinBackground;


public slots:
    void setRegWinFont(QFont f)
    {
        regWinFont = new QFont(f);
        regWinFontLineEdit->setText(f.family());
    }

    void setRegWinColor(QColor c)
    {
        regWinColor = new QColor(c);
        regWinFontColorLineEdit->setText(c.name());
    }

    void setRegWinBackground(QColor c)
    {
        regWinBackground = new QColor(c);
        regWinBackgroundLineEdit->setText(c.name());
    }


    void setTextWinFont(QFont f)
    {
        textWinFont = new QFont(f);
        textWinFontLineEdit->setText(f.family());
    }

    void setTextWinColor(QColor c)
    {
        textWinColor = new QColor(c);
        textWinFontColorLineEdit->setText(c.name());
    }

    void setTextWinBackground(QColor c)
    {
        textWinBackground = new QColor(c);
        textWinBackgroundLineEdit->setText(c.name());
    }
};

}
#endif // SPIM_SETTING_DIALOG_H
