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


#include "stdafx.h"

#include "PCSpim.h"

#include "PCSpimView.h"
#include "ConsoleWnd.h"

// Necessary SPIM globals
bool bare_machine;                 /* => simulate bare machine */
bool accept_pseudo_insts;          /* => parse pseudo instructions  */
bool delayed_branches;             /* => simulate delayed branches */
bool delayed_loads;                /* => simulate delayed loads */
bool quiet;                        /* => no warning messages */
char * exception_file_name = NULL; /* The path from which to load the exception handler, if desired */
port message_out, console_out;
bool mapped_io;                  /* => activate memory-mapped IO */
int spim_return_value;		/* Value returned when spim exits */


int console_input_available()
{
    if (mapped_io)
    {
        ((CPCSpimApp *)AfxGetApp())->RunMessageLoop();

        return g_pView->GetConsole()->AreKeysAvailable();
    }
    else
        return 0;
}


#define IO_BUFFSIZE 10000
void error(char *fmt, ...)
{
    va_list args;
    char io_buffer [IO_BUFFSIZE];

    va_start (args, fmt);
    vsprintf (io_buffer, fmt, args);
    va_end (args);

    write_output(message_out, io_buffer);
}


void fatal_error(char *fmt, ...)
{
    CString strMsg, strErrText;

    va_list args;
    va_start(args, fmt);

    // Get the error message.
    _vsnprintf(strErrText.GetBuffer(2048), 2048, fmt, args);
    strErrText.ReleaseBuffer();
    strErrText.TrimLeft();
    strErrText.TrimRight();

    // Create the message box contents.
    strMsg.Format(
        "A fatal error has occurred within the SPIM simulator.\n"
        "The error message is:\n"
        "\n"
        "%s\n"
        "\n"
        "Would you like to attempt to continue?", strErrText);

    // Prompt the user.
    if (IDYES == MessageBox(NULL, strMsg, NULL, MB_YESNO | MB_ICONEXCLAMATION))
        return;
    else
        exit(-1);
}


char get_console_char()
{
    g_pView->BringConsoleToTop();
    CPCSpimApp *pApp = (CPCSpimApp *)AfxGetApp();

    // Loop until the user presses a key.
    while (!force_break)
    {
        pApp->RunMessageLoop();
        if (g_pView->GetConsole()->AreKeysAvailable())
            return (char)g_pView->GetConsole()->GetKeyPress();
    }

    return 0;
}


void put_console_char(char c)
{
    char buf[4];

    buf[0] = c;
    buf[1] = '\0';

    write_output(console_out, buf);
}


void read_input(char *str, int str_size)
{
    char ch;

    if (NULL == str)
    {
        run_error ("Buffer is null in read_string");
        return;
    }
    if (str_size < 0)
    {
        run_error ("Buffer size is null in read_string");
        return;
    }

    // We assume that get_console_char will "do the right thing" wrt the msg
    // pump, etc.
    while ((1 < str_size) && (!force_break))
    {
        ch = get_console_char();
        *str = ch;
        ++str;
        --str_size;
        put_console_char(ch);

        // Read just one line.
        if (ch == '\n')
            break;
    }

    if (0 < str_size)
        *str = '\0';
}


void run_error (char *fmt, ...)
{
    va_list args;
    char io_buffer [IO_BUFFSIZE];

    va_start (args, fmt);
    vsprintf (io_buffer, fmt, args);
    va_end (args);

    write_output(message_out, io_buffer);

    // Force a break;
    force_break = true;
}


void write_output (port fp, char *fmt, ...)
{
    CString strBuf;
    va_list args;

    va_start(args, fmt);
    _vsnprintf(strBuf.GetBuffer(IO_BUFFSIZE), IO_BUFFSIZE, fmt, args);
    strBuf.ReleaseBuffer();
    va_end(args);

    if (fp.i == message_out.i)
        g_pView->WriteToMessage(strBuf);
    else if (fp.i == console_out.i)
    {
        g_pView->MakeConsoleVisible();
        g_pView->WriteToConsole(strBuf);
    }
    else
        error("Internal error in calling write_output!");
}
