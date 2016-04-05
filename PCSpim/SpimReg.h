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


#ifndef SPIMREG_H
#define SPIMREG_H

#define SPIM_REG_SPIMROOT		"Software\\UW-Madison\\PCSpim"
#define SPIM_REG_DEFAULTSETTINGS	SPIM_REG_SPIMROOT "\\Default Settings"
#define SPIM_REG_SETTINGS		SPIM_REG_SPIMROOT "\\Settings"

#define SPIM_REG_BARE			"Bare Machine"
#define SPIM_REG_DELAYEDBRANCHES	"Simulate Delayed Branches"
#define SPIM_REG_DELAYEDLOADS		"Simulate Delayed Loads"
#define SPIM_REG_PSEUDO			"Accept Pseudo Instructions"
#define SPIM_REG_LOADEXCEPTION		"Load Exception Handler"
#define SPIM_REG_MAPPEDIO		"Mapped IO"
#define SPIM_REG_QUIET			"Quiet"
#define SPIM_REG_EXCEPTIONFILE		"Exception File"
#define SPIM_REG_FPREG_HEX		"FP Regs in Hex"
#define SPIM_REG_GENREG_HEX		"General Regs in Hex"
#define	SPIM_REG_CHECK_UNDEF		"Uncheck Undefined Symbols"

#define SPIM_REG_SAVEWINPOS		"Save Window Positions"

#define SPIM_REG_MAINWINPOS		"Main Window Position"
#define SPIM_REG_MESSAGESPOS		"Message Window Position"
#define SPIM_REG_MESSAGESMINMAX		"Message Window State"
#define SPIM_REG_REGISTERSPOS		"Register Window Position"
#define SPIM_REG_REGISTERSMINMAX	"Register Window State"
#define SPIM_REG_TEXTSEGPOS		"Text Segment Window Position"
#define SPIM_REG_TEXTSEGMINMAX		"Text Segment Window State"
#define SPIM_REG_DATASEGPOS		"Data Segment Window Position"
#define SPIM_REG_DATASEGMINMAX		"Data Segment Window State"
#define SPIM_REG_CONSOLEPOS		"Console Window Position"

#define SPIM_REG_FONTFACE		"Window Font Face"
#define SPIM_REG_FONTHEIGHT		"Window Font Height"
#define SPIM_REG_FONTWEIGHT		"Window Font Weight"
#define SPIM_REG_FONTITALIC		"Window Font Italic"

#endif // SPIMREG_H
