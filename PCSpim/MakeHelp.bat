REM SPIM S20 MIPS simulator.
REM Definitions for the SPIM S20.
REM
REM Copyright (c) 1990-2010, James R. Larus.
REM Changes for DOS and Windows versions by David A. Carley (dac@cs.wisc.edu)
REM All rights reserved.
REM
REM Redistribution and use in source and binary forms, with or without modification,
REM are permitted provided that the following conditions are met:
REM
REM Redistributions of source code must retain the above copyright notice,
REM this list of conditions and the following disclaimer.
REM
REM Redistributions in binary form must reproduce the above copyright notice,
REM this list of conditions and the following disclaimer in the documentation and/or
REM other materials provided with the distribution.
REM
REM Neither the name of the James R. Larus nor the names of its contributors may be
REM used to endorse or promote products derived from this software without specific
REM prior written permission.
REM
REM THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
REM AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
REM IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
REM ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
REM LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
REM CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
REM GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
REM HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
REM LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
REM OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
REM


REM Batch file to produce PCSPIM help files.
REM
REM Usage: MakeHelp <target directory>
REM    e.g. MakeHelp ..\Release
REM

@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by PCSPIM.HPJ. >"hlp\PCSpim.hm"
echo. >>"hlp\PCSpim.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\PCSpim.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\PCSpim.hm"
echo. >>"hlp\PCSpim.hm"
echo // Prompts (IDP_*) >>"hlp\PCSpim.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\PCSpim.hm"
echo. >>"hlp\PCSpim.hm"
echo // Resources (IDR_*) >>"hlp\PCSpim.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\PCSpim.hm"
echo. >>"hlp\PCSpim.hm"
echo // Dialogs (IDD_*) >>"hlp\PCSpim.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\PCSpim.hm"
echo. >>"hlp\PCSpim.hm"
echo // Frame Controls (IDW_*) >>"hlp\PCSpim.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\PCSpim.hm"
REM -- Make help for Project PCSPIM

echo Building Win32 Help files
start /wait hcrtf -x "hlp\PCSpim.hpj"
echo.

copy "hlp\PCSpim.hlp" %1
copy "hlp\PCSpim.cnt" %1

echo.


