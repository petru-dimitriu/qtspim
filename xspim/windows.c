/* SPIM S20 MIPS simulator.
   X interface to SPIM

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


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>

#include <stdio.h>

#include "spim.h"
#include "string-stream.h"
#include "spim-utils.h"
#include "buttons.h"
#include "xspim.h"
#include "windows.h"


/* Exported variables: */

Widget register_window, text_window, data_window;


/* Internal functions: */

static void create_command_panel (Widget parent, Dimension app_width,
				  Dimension button_height,
				  Dimension command_hspace,
				  Dimension command_vspace,
				  Dimension command_height);
static void create_data_window (Widget parent, Dimension segment_height);
static void create_display_window (Widget parent, Dimension display_height);
static void create_reg_window (Widget parent, Dimension reg_min_height,
			       Dimension reg_max_height);
static void create_text_window (Widget parent, Dimension segment_height);



static void
create_reg_window (Widget parent, Dimension reg_min_height,
		   Dimension reg_max_height)
{
  Arg args[15];
  Cardinal n = 0;

  XtSetArg (args[n], XtNtype, XawAsciiString); n++;
  XtSetArg (args[n], XtNeditType, XawtextRead); n++;
  XtSetArg (args[n], XtNpreferredPaneSize, reg_min_height); n++;
  XtSetArg (args[n], XtNmin, reg_min_height); n++;
  XtSetArg (args[n], XtNmax, reg_max_height); n++;
  XtSetArg (args[n], XtNresize, "width"); n++;
  XtSetArg (args[n], XtNwrap, "line"); n++;
  XtSetArg (args[n], XtNstring, ""); n++;
  XtSetArg (args[n], XtNlength, 0); n++;
  XtSetArg (args[n], XtNuseStringInPlace, True); n++;
  XtSetArg (args[n], XtNdisplayCaret, False); n++;
  XtSetArg (args[n], XtNscrollVertical, XawtextScrollAlways); n++;
  register_window = XtCreateManagedWidget ("register", asciiTextWidgetClass,
					   parent, args, n);
}


static void
create_command_panel (Widget parent, Dimension app_width,
		      Dimension button_height, Dimension command_hspace,
		      Dimension command_vspace, Dimension command_height)
{
  Widget panel;
  Arg args[10];
  Cardinal n = 0;

  XtSetArg (args[n], XtNwidth, app_width); n++;
  XtSetArg (args[n], XtNmin, command_height); n++;
  XtSetArg (args[n], XtNmax, command_height); n++;
  XtSetArg (args[n], XtNpreferredPaneSize, 2*button_height + 3 * command_vspace);  n++;
  XtSetArg (args[n], XtNhSpace, command_hspace); n++;
  XtSetArg (args[n], XtNvSpace, command_vspace); n++;
  XtSetArg (args[n], XtNallowResize, False); n++;
  panel = XtCreateManagedWidget ("panel", boxWidgetClass, parent, args, n);

  create_buttons (panel);
}


static void
create_text_window (Widget parent, Dimension segment_height)
{
  Arg args[15];
  Cardinal n = 0;

  XtSetArg (args[n], XtNallowResize, False); n++;
  XtSetArg (args[n], XtNshowGrip, False); n++;
  XtSetArg (args[n], XtNresize, False); n++;
  XtSetArg (args[n], XtNmin, TEXTHEIGHT); n++;
  XtSetArg (args[n], XtNlabel, "Text Segments"); n++;
  XtSetArg (args[n], XtNborderWidth, 0); n++;
  XtSetArg (args[n], XtNuseStringInPlace, True); n++;
  XtCreateManagedWidget ("TextSegmentLabel", labelWidgetClass, parent,
			 args, n);

  n = 0;
  XtSetArg (args[n], XtNtype, XawAsciiString); n++;
/*  XtSetArg (args[n], XtNeditType, XawtextRead); n++;*/
  XtSetArg (args[n], XtNpreferredPaneSize, segment_height); n++;
  XtSetArg (args[n], XtNstring, ""); n++;
  XtSetArg (args[n], XtNborderWidth, 0); n++;
  XtSetArg (args[n], XtNdisplayCaret, True); n++;
  XtSetArg (args[n], XtNdisplayCaret, False); n++;
  XtSetArg (args[n], XtNscrollVertical, XawtextScrollAlways); n++;
  text_window = XtCreateManagedWidget ("file", asciiTextWidgetClass, parent,
				       args, n);
}


static void
create_data_window (Widget parent, Dimension segment_height)
{
  Arg args[15];
  Cardinal n = 0;

  XtSetArg (args[n], XtNallowResize, False); n++;
  XtSetArg (args[n], XtNshowGrip, False); n++;
  XtSetArg (args[n], XtNresize, False); n++;
  XtSetArg (args[n], XtNmin, TEXTHEIGHT); n++;
  XtSetArg (args[n], XtNlabel, "Data Segments"); n++;
  XtSetArg (args[n], XtNborderWidth, 0); n++;
  XtSetArg (args[n], XtNuseStringInPlace, True); n++;
  XtSetArg (args[n], XtNdisplayCaret, False); n++;
  XtCreateManagedWidget ("DataSegmentLabel", labelWidgetClass, parent,
			 args, n);

  n = 0;
  XtSetArg (args[n], XtNtype, XawAsciiString); n++;
  XtSetArg (args[n], XtNeditType, XawtextRead); n++;
  XtSetArg (args[n], XtNpreferredPaneSize, segment_height); n++;
  XtSetArg (args[n], XtNstring, ""); n++;
  XtSetArg (args[n], XtNborderWidth, 0); n++;
  XtSetArg (args[n], XtNdisplayCaret, False); n++;
  XtSetArg (args[n], XtNscrollVertical, XawtextScrollAlways); n++;
  data_window = XtCreateManagedWidget ("file", asciiTextWidgetClass, parent,
				       args, n);
}


static void
create_display_window (Widget parent, Dimension display_height)
{
  Arg args[15];
  Cardinal n = 0;

  XtSetArg (args[n], XtNeditType, XawtextEdit); n++;
  XtSetArg (args[n], XtNpreferredPaneSize, display_height); n++;
  XtSetArg (args[n], XtNborderWidth, 0); n++;
  XtSetArg (args[n], XtNdisplayCaret, False); n++;
  XtSetArg (args[n], XtNdisplayNonprinting, False); n++;
  XtSetArg (args[n], XtNscrollVertical, XawtextScrollAlways); n++;
  XtSetArg (args[n], XtNwrap, "line"); n++;
  message = XtCreateManagedWidget ("display", asciiTextWidgetClass, parent,
				   args, n);
  message_out.f = (FILE *) message;
}


void
create_sub_windows (Widget parent, Dimension app_width,
		    Dimension reg_min_height, Dimension reg_max_height,
		    Dimension command_height, Dimension command_hspace,
		    Dimension command_vspace, Dimension button_height,
		    Dimension segment_height, Dimension display_height)
{
  Widget pane;
  Arg args[10];
  Cardinal n = 0;

  XtSetArg (args[n], XtNwidth, app_width); n++;
  pane = XtCreateManagedWidget ("pane", panedWidgetClass, parent, args, n);

  create_reg_window (pane, reg_min_height, reg_max_height);
  create_command_panel (pane, app_width, button_height, command_hspace,
			command_vspace, command_height);
  create_text_window (pane, segment_height);
  create_data_window (pane, segment_height);
  create_display_window (pane, display_height);
}
