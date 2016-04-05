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


#include <stdarg.h>

#include "spimview.h"
#include "ui_spimview.h"

#include <QChar>


// SPIM globals
//
bool bare_machine;             /* => simulate bare machine */
bool accept_pseudo_insts;      /* => parse pseudo instructions  */
bool delayed_branches;         /* => simulate delayed branches */
bool delayed_loads;            /* => simulate delayed loads */
bool quiet;                    /* => no warning messages */
char* exception_file_name = 0; /* The path from which to load the exception handler, if desired */
bool mapped_io;                /* => activate memory-mapped IO */
int spim_return_value;         /* Value returned when spim exits */

port message_out;
port console_out;


#define BIG_BUF_SIZE 10000

int console_input_available()
{
  return Window->SpimConsole->InputAvailable();

}


void error(char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    char buf[BIG_BUF_SIZE];
    qvsnprintf(buf, BIG_BUF_SIZE, fmt, args);
    va_end(args);

    Window->Error(buf, 0);
}


void run_error (char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    char buf[BIG_BUF_SIZE];
    qvsnprintf(buf, BIG_BUF_SIZE, fmt, args);
    va_end(args);

    Window->Error(buf, 0);
}


void fatal_error(char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    char buf[BIG_BUF_SIZE];
    qvsnprintf(buf, BIG_BUF_SIZE, fmt, args);
    va_end(args);

    Window->Error(buf, 1);
}


char get_console_char()
{
    QString c = Window->SpimConsole->ReadChar();
    if (c != "")
    {
        char ac = c[0].toLatin1();
        if (ac == '\r')
        {
            return '\n';
        }
        return ac;
    }
    else
    {
        return 0;               // FIXME
    }
}


void put_console_char(char c)
{
    Window->SpimConsole->WriteOutput(QString(c));
    Window->SpimConsole->raise();
}


void read_input(char *str, int str_size)
{
    while ((1 < str_size) && (!force_break))
    {
        char ch = get_console_char();

        *str = ch;
        str += 1;
        str_size -= 1;

        if (ch == '\n')
        {
            break;
        }
    }

    if (0 < str_size)
    {
        *str = '\0';
    }
}


void write_output (port fp, char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    char buf[BIG_BUF_SIZE];
    qvsnprintf(buf, BIG_BUF_SIZE, fmt, args);
    va_end(args);

    if (fp.i == message_out.i)
    {
        Window->WriteOutput(buf);
    }
    else if (fp.i == console_out.i)
    {
        Window->SpimConsole->WriteOutput(QString(buf));
        Window->SpimConsole->raise();
    }
}
