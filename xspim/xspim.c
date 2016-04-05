/* SPIM S20 MIPS simulator.
   X interface to SPIM
   (Derived from an earlier work by Alan Siow.)

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


#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xlib.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Dialog.h>
#include <X11/keysym.h>

#include "spim.h"
#include "string-stream.h"
#include "spim-utils.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "parser_yacc.h"
#include "buttons.h"
#include "windows.h"
#include "xspim.h"
#include "sym-tbl.h"


typedef struct _AppResources
{
  String textFont;
  Boolean bare;
  Boolean delayed_branches;
  Boolean delayed_loads;
  Boolean pseudo;
  Boolean asmm;
  Boolean exception;
  char *exception_file_name;
  Boolean quiet;
  Boolean mapped_io;
  char *filename;
  char *display2;
  Boolean hex_gpr;
  Boolean hex_fpr;
  char *initial_data_limit;
  char *initial_data_size;
  char *initial_k_data_limit;
  char *initial_k_data_size;
  char *initial_k_text_size;
  char *initial_stack_limit;
  char *initial_stack_size;
  char *initial_text_size;
} AppResources;


/* Exported variables: */

/* Not local, but not export so all files don't need setjmp.h */
jmp_buf spim_top_level_env; /* For ^C */

bool bare_machine;		/* => simulate bare machine */
bool delayed_branches;		/* => simulate delayed branches */
bool delayed_loads;		/* => simulate delayed loads */
bool accept_pseudo_insts;	/* => parse pseudo instructions  */
bool quiet;			/* => no warning messages */
port message_out, console_out, console_in;
bool mapped_io;			/* => activate memory-mapped IO */
int pipe_out;
int spim_return_value;		/* Value returned when spim exits */

XtAppContext app_con;
Widget message, console = NULL;
XtAppContext app_context;
XFontStruct *text_font;
Dimension button_width;
int load_exception_handler;
char *exception_file_name = DEFAULT_EXCEPTION_HANDLER;
Pixmap mark;


/* Local functions: */

static void center_text_at_PC ();
static char *check_buf_limit (char *, int *, int *);
static void create_console_display ();
static void display_data_seg ();
static char *display_values (mem_addr from, mem_addr to, char *buf, int *limit,
			     int *n);
static char *display_insts (mem_addr from, mem_addr to, char *buf, int *limit,
			    int *n);
static void display_registers ();
static void initialize (AppResources app_res);
static mem_addr print_partial_line (mem_addr, char *, int *, int *);
static void show_running ();
static void syntax (char *program_name);
static void write_text_to_window (Widget w, char *s);


static String fallback_resources[] =
{
  "*font:		*-courier-medium-r-normal--12-*-75-*",
  "*Label*font:		*-adobe-helvetica-bold-r-*-*-12-*-75-*",
  "*panel*font:		*-adobe-helvetica-medium-r-*-*-12-*-75-*",
  "*ShapeStyle:		Oval",
  "*dialog*value.translations: #override \\n <Key>Return: confirm()",
  "*Form*left:		ChainLeft",
  "*Form*right:		ChainLeft",
  "*Form*top:		ChainTop",
  "*Form*bottom:	ChainTop",
  "*console.label:	SPIM Console",
  "*Shell1*iconName:	SPIM Console",
  NULL,
};


static XtActionsRec actionTable[2] =
{
  {"confirm", (XtActionProc) confirm},
};


static XtResource resources[] =
{
  {XtNfont, XtCFont, XtRString, sizeof (char *),
   XtOffset (AppResources *, textFont), XtRString, NULL},
  {"bare", "Bare", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, bare), XtRImmediate, False},
  {"delayed_branches", "Delayed_Branches", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, delayed_branches), XtRImmediate, False},
  {"delayed_loads", "Delayed_Loads", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, delayed_loads), XtRImmediate, False},
  {"pseudo", "Pseudo", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, pseudo), XtRImmediate, (XtPointer)True},
  {"asm",  "Asm",  XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, asmm), XtRImmediate, False},
  {"exception", "Exception", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, exception), XtRImmediate, (XtPointer) True},
  {"exception_file_name", "Exception_File_Name", XtRString, sizeof (char *),
   XtOffset (AppResources *, exception_file_name), XtRString, NULL},
  {"quiet", "Quiet", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, quiet), XtRImmediate, False},
  {"mapped_io", "Mapped_IO", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, mapped_io), XtRImmediate, False},

  {"filename", "Filename", XtRString, sizeof (char *),
   XtOffset (AppResources *, filename), XtRString, NULL},
  {"display2", "Display2", XtRString, sizeof (char *),
   XtOffset (AppResources *, display2), XtRString, NULL},
  {"hexGpr", "DisplayHex", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, hex_gpr), XtRImmediate, (XtPointer) True},
  {"hexFpr", "DisplayHex", XtRBoolean, sizeof (Boolean),
   XtOffset (AppResources *, hex_fpr), XtRImmediate, False},

  {"stext", "Stext", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_text_size), XtRString, NULL},
  {"sdata", "Sdata", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_data_size), XtRString, NULL},
  {"ldata", "Ldata", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_data_limit), XtRString, NULL},
  {"sstack", "Sstack", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_stack_size), XtRString, NULL},
  {"lstack", "Lstack", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_stack_limit), XtRString, NULL},
  {"sktext", "Sktext", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_k_text_size), XtRString, NULL},
  {"skdata", "Skdata", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_k_data_size), XtRString, NULL},
  {"lkdata", "Lkdata", XtRString, sizeof (char *),
   XtOffset (AppResources *, initial_k_data_limit), XtRString, NULL}
};


static XrmOptionDescRec options[] =
{
  {"-asm",    "asmm",  XrmoptionNoArg, "True"},
  {"-a",    "asmm",  XrmoptionNoArg, "True"},
  {"-bare",   "bare", XrmoptionNoArg, "True"},
  {"-b",   "bare", XrmoptionNoArg, "True"},
  {"-delayed_branches", "delayed_branches", XrmoptionNoArg, "True"},
  {"-db", "delayed_branches", XrmoptionNoArg, "True"},
  {"-delayed_loads", "delayed_loads", XrmoptionNoArg, "True"},
  {"-dl", "delayed_loads", XrmoptionNoArg, "True"},
  {"-exception",   "exception", XrmoptionNoArg, "True"},
  {"-e",   "exception", XrmoptionNoArg, "True"},
  {"-noexception", "exception", XrmoptionNoArg, "False"},
  {"-ne", "exception", XrmoptionNoArg, "False"},
  {"-exception_file_name", "exception_file_name", XrmoptionSepArg, NULL},
  {"-ef", "exception_file_name", XrmoptionSepArg, NULL},
  {"-mapped_io",  "mapped_io", XrmoptionNoArg, "True"},
  {"-mio",  "mapped_io", XrmoptionNoArg, "True"},
  {"-nomapped_io","mapped_io", XrmoptionNoArg, "False"},
  {"-nmio","mapped_io", XrmoptionNoArg, "False"},
  {"-pseudo",   "pseudo", XrmoptionNoArg, "True"},
  {"-p",   "pseudo", XrmoptionNoArg, "True"},
  {"-nopseudo", "pseudo", XrmoptionNoArg, "False"},
  {"-np", "pseudo", XrmoptionNoArg, "False"},
  {"-quiet",  "quiet", XrmoptionNoArg, "True"},
  {"-q",  "quiet", XrmoptionNoArg, "True"},
  {"-noquiet","quiet", XrmoptionNoArg, "False"},
  {"-nq","quiet", XrmoptionNoArg, "False"},
  {"-trap",   "exception", XrmoptionNoArg, "True"},
  {"-t",   "exception", XrmoptionNoArg, "True"},
  {"-notrap", "exception", XrmoptionNoArg, "False"},
  {"-nt", "exception", XrmoptionNoArg, "False"},
  {"-trap_file_name", "exception_file_name", XrmoptionSepArg, NULL},
  {"-tf", "exception_file_name", XrmoptionSepArg, NULL},
  {"-stext", "stext", XrmoptionSepArg, NULL},
  {"-st", "stext", XrmoptionSepArg, NULL},
  {"-sdata", "sdata", XrmoptionSepArg, NULL},
  {"-sd", "sdata", XrmoptionSepArg, NULL},
  {"-ldata", "ldata", XrmoptionSepArg, NULL},
  {"-ld", "ldata", XrmoptionSepArg, NULL},
  {"-sstack", "sstack", XrmoptionSepArg, NULL},
  {"-ss", "sstack", XrmoptionSepArg, NULL},
  {"-lstack", "lstack", XrmoptionSepArg, NULL},
  {"-ls", "lstack", XrmoptionSepArg, NULL},
  {"-sktext", "sktext", XrmoptionSepArg, NULL},
  {"-skt", "sktext", XrmoptionSepArg, NULL},
  {"-skdata", "skdata", XrmoptionSepArg, NULL},
  {"-skd", "skdata", XrmoptionSepArg, NULL},
  {"-lkdata", "lkdata", XrmoptionSepArg, NULL},
  {"-lkd", "lkdata", XrmoptionSepArg, NULL},
  {"-file",   "filename", XrmoptionSepArg, NULL},
  {"-f",   "filename", XrmoptionSepArg, NULL},
  {"-d2",     "display2", XrmoptionSepArg, NULL},
  {"-hexgpr", "hexGpr", XrmoptionNoArg, "True"},
  {"-nohexgpr", "hexGpr", XrmoptionNoArg, "False"},
  {"-hexfpr", "hexFpr", XrmoptionNoArg, "True"},
  {"-nohexfpr", "hexFpr", XrmoptionNoArg, "False"},
};


#define TICK_WIDTH 10

#define TICK_HEIGHT 10

static unsigned char tick_bits[] = {
  0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x80, 0x01, 0xc1, 0x00, 0x63, 0x00,
  0x36, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x00, 0x00};


/* Flags to control the way that registers are displayed. */

static int print_gpr_hex;		/* Print GPRs in hex/decimal */
static int print_fpr_hex;		/* Print FPRs in hex/floating point */


/* Local variables: */

static Dimension app_width;
static Dimension button_height;
static Dimension command_height;
static Dimension command_hspace;
static Dimension command_vspace;
static int console_is_visible;
static Dimension display_height;
static char *file_name = NULL;
static Dimension reg_min_height;
static Dimension reg_max_height;
static Dimension segment_height;
static Widget shell1;
static int spim_is_running = 0;
static Widget toplevel;
static Widget pane1;



static void
initialize (AppResources app_res)
{
  bare_machine = false;
  delayed_branches = false;
  delayed_loads = false;
  accept_pseudo_insts = true;
  quiet = 0;

  if (app_res.bare)
    {
      bare_machine = true;
      delayed_branches = true;
      delayed_loads = true;
    }

  if (app_res.asmm)
    {
      bare_machine = false;
      delayed_branches = false;
      delayed_loads = false;
    }

  if (app_res.delayed_branches)
    delayed_branches = true;

  if (app_res.delayed_loads)
    delayed_loads = true;

  if (app_res.pseudo)
    accept_pseudo_insts = true;
  else
    accept_pseudo_insts = false;

  if (app_res.exception)
    load_exception_handler = 1;
  else
    load_exception_handler = 0;

  if (app_res.exception_file_name)
    {
      exception_file_name = app_res.exception_file_name;
      load_exception_handler = 1;
    }

  if (app_res.quiet)
    quiet = 1;
  else
    quiet = 0;

  if (app_res.mapped_io)
    mapped_io = true;
  else
    mapped_io = false;

  if (app_res.filename)
    file_name = app_res.filename;

  if (app_res.textFont == NULL)
    app_res.textFont = XtNewString ("8x13");
  if (!(text_font = XLoadQueryFont (XtDisplay (toplevel), app_res.textFont)))
    fatal_error ("Cannot open font %s\n", app_res.textFont);

  mark = XCreateBitmapFromData (XtDisplay (toplevel),
				RootWindowOfScreen (XtScreen (toplevel)),
				(char*)tick_bits, TICK_WIDTH, TICK_HEIGHT);

  button_height = TEXTHEIGHT * 1.6;
  button_width = TEXTWIDTH * 12;
  app_width = 6 * (button_width + 16);
  if ((unsigned)app_width < TEXTWIDTH * 4 * 22) /* Register display width */
    app_width = TEXTWIDTH * 4 * 22;
  command_hspace = 8;
  command_vspace = 8;
  command_height = (button_height * 2) + (command_vspace * 3);
  reg_min_height = 15 * TEXTHEIGHT + 4;
  reg_max_height = reg_min_height + 10 * TEXTHEIGHT + 4;
  segment_height = 8 * TEXTHEIGHT + 4;
  display_height = 8 * TEXTHEIGHT + 4;
  print_gpr_hex = app_res.hex_gpr;
  print_fpr_hex = app_res.hex_fpr;
}


static void
create_console_display ()
{
  Arg args[10];
  Cardinal n;

  n = 0;
  XtSetArg (args[n], XtNeditType, XawtextAppend); n++;
  XtSetArg (args[n], XtNscrollVertical, XawtextScrollWhenNeeded); n++;
  XtSetArg (args[n], XtNpreferredPaneSize, TEXTHEIGHT * 24); n++;
  XtSetArg (args[n], XtNwidth, TEXTWIDTH * 80); n++;
  console = XtCreateManagedWidget ("console", asciiTextWidgetClass, pane1,
				   args, n);
  XawTextEnableRedisplay (console);
  console_out.f = (FILE*) console;
}

void
clear_console_display ()
{
  Arg args[10];
  Cardinal n;

  XtSetArg (args[0], XtNstring, "");
  XtSetValues(console, args, 1);
}


int
main (int argc, char **argv)
{
  Widget toplevel2;
  AppResources app_res;
  Display *display;

  spim_return_value = 0;

  if (getenv ("SPIM_EXCEPTION_HANDLER") != NULL)
    exception_file_name = getenv("SPIM_EXCEPTION_HANDLER");

  toplevel = XtAppInitialize (&app_context, "Xspim", options,
			      XtNumber (options), &argc, argv,
			      fallback_resources, NULL, ZERO);

  if (argc >= 0 && argv[argc] != NULL && argv[argc][0] == '-')
    syntax (argv[0]);		/* Bad command line argument */

  XtGetApplicationResources (toplevel, (XtPointer) &app_res, resources,
			     XtNumber (resources), NULL, ZERO);

  if (app_res.display2 == NULL)
    display = XtDisplay (toplevel);
  else
    display = XtOpenDisplay (app_context, app_res.display2, "xspim",
			     "Xspim", NULL, ZERO, &argc, argv);

  toplevel2 = XtAppCreateShell ("xspim","Xspim",applicationShellWidgetClass,
				display, NULL, ZERO);

  XtAppAddActions (app_context, actionTable, XtNumber (actionTable));

  initialize (app_res);

  /* Console window */

  shell1 = XtCreatePopupShell ("Shell1", topLevelShellWidgetClass,
			       toplevel, NULL, ZERO);
  pane1 = XtCreateManagedWidget ("pane1", panedWidgetClass, shell1,
				 NULL, ZERO);
  create_console_display ();

  create_sub_windows (toplevel, app_width, reg_min_height, reg_max_height,
		      command_height, command_hspace, command_vspace,
		      button_height, segment_height, display_height);

  XtRealizeWidget (toplevel);

  if (app_res.initial_text_size != NULL)
    initial_text_size = atoi (app_res.initial_text_size);
  if (app_res.initial_data_size != NULL)
    initial_data_size = atoi (app_res.initial_data_size);
  if (app_res.initial_data_limit != NULL)
    initial_data_limit = atoi (app_res.initial_data_limit);
  if (app_res.initial_stack_size != NULL)
    initial_stack_size = atoi (app_res.initial_stack_size);
  if (app_res.initial_stack_limit != NULL)
    initial_stack_limit = atoi (app_res.initial_stack_limit);
  if (app_res.initial_k_text_size != NULL)
    initial_k_text_size = atoi (app_res.initial_k_text_size);
  if (app_res.initial_k_data_size != NULL)
    initial_k_data_size = atoi (app_res.initial_k_data_size);
  if (app_res.initial_k_data_limit != NULL)
    initial_k_data_limit = atoi (app_res.initial_k_data_limit);

  write_startup_message ();
  initialize_world (load_exception_handler ? exception_file_name : NULL, true);

  if (file_name == NULL && argc > 1 && argv[argc - 1] != NULL)
    {
      /* The first unprocessed argument (aside from program name) is taken
	 as a filename containing input program. */
      file_name = argv[argc - 1];
    }

  if (file_name)
    {
      read_file (file_name);
      record_file_name_for_prompt (file_name);
    }
  else
    {
      PC = starting_address ();
      redisplay_text ();
      center_text_at_PC ();
      redisplay_data ();
    }

  XtAppMainLoop (app_context);
  return (spim_return_value);
}


static void
syntax (char *program_name)
{
  XtDestroyApplicationContext (app_context);
  fprintf (stderr, "Usage:\n %s", program_name);

  fprintf (stderr, "-bare			Bare machine (no pseudo-ops, delayed branches and loads)\n");
  fprintf (stderr, "-asm			Extended machine (pseudo-ops, no delayed branches and loads) (default)\n");
  fprintf (stderr, "-delayed_branches	Execute delayed branches\n");
  fprintf (stderr, "-delayed_loads		Execute delayed loads\n");
  fprintf (stderr, "-exception		Load exception handler (default)\n");
  fprintf (stderr, "-noexception		Do not load exception handler\n");
  fprintf (stderr, "-exception_file <file>	Specify exception handler in place of default\n");
  fprintf (stderr, "-quiet			Do not print warnings\n");
  fprintf (stderr, "-noquiet		Print warnings (default)\n");
  fprintf (stderr, "-mapped_io		Enable memory-mapped IO\n");
  fprintf (stderr, "-nomapped_io		Do not enable memory-mapped IO (default)\n");
  fprintf (stderr, "-file <file> <args>	Assembly code file and arguments to program\n");
  exit (1);
}


void
popup_console (Widget w, XtPointer client_data, XtPointer call_data)
{
  if (console_is_visible)
    {
      console_is_visible = 0;
      XtPopdown (shell1);
    }
  else
    {
      console_is_visible = 1;
      XtPopup (shell1, XtGrabNone);
    }
}


void
read_file (char *name)
{
  bool error_flag = false;

  if (*name != '\0')
    error_flag = read_assembly_file (name);

  if (error_flag)
    {
      PC = find_symbol_address (DEFAULT_RUN_LOCATION);
      redisplay_text ();
      center_text_at_PC ();
      redisplay_data ();
    }
}


void
start_program (mem_addr addr)
{
  if (addr == 0)
    addr = starting_address ();

  if (addr != 0)
    execute_program (addr, DEFAULT_RUN_STEPS, 0, 0);
}


void
execute_program (mem_addr pc, int steps, bool display, bool cont_bkpt)
{
  bool continuable;
  if (!setjmp (spim_top_level_env))
    {
      char *undefs = undefined_symbol_string ();
      if (undefs != NULL)
	{
	  write_output (message_out, "The following symbols are undefined:\n");
	  write_output (message_out, undefs);
	  write_output (message_out, "\n");
	  free (undefs);
	}

      spim_is_running = 1;
      show_running ();

      if (run_program (pc, steps, display, cont_bkpt, &continuable))
	continue_prompt (0);
    }
  redisplay_text ();
  spim_is_running = 0;
  center_text_at_PC ();
  redisplay_data ();
}


static void
show_running ()
{
  Arg args[1];

  XtSetArg (args[0], XtNstring, "Running.....");
  XtSetValues (register_window, args, ONE);
}


/* Redisplay the contents of the registers and, if modified, the data
   and stack segments. */

void
redisplay_data ()
{
  display_registers ();
  display_data_seg ();
}


/* Redisplay the contents of the registers in a wide variety of
   formats. */

static void
display_registers ()
{
  Arg args [2];
  static str_stream ss;

  ss_clear (&ss);
  format_registers (&ss, print_gpr_hex, print_fpr_hex);

  XtSetArg (args[0], XtNstring, (String)ss_to_string (&ss));
  XtSetArg (args[1], XtNlength, ss_length (&ss));
  XtSetValues (register_window, args, TWO);
}


/* Redisplay the text segment and ktext segments if they have changed. */

void
redisplay_text ()
{
  Arg args [2];
  static str_stream ss;

  if (!text_modified)
    return;

  ss_clear (&ss);

  format_insts (&ss, TEXT_BOT, text_top);
  ss_printf (&ss, "\n\tKERNEL\n");
  format_insts (&ss, K_TEXT_BOT, k_text_top);

  XtSetArg (args[0], XtNstring, ss_to_string(&ss));
  XtSetArg (args[1], XtNlength, ss_length (&ss));
  XtSetValues (text_window, args, TWO);

  text_modified = false;
}


/* Center the text window at the instruction at the current PC and
   highlight the instruction. */

static void
center_text_at_PC ()
{
  char buf[100];
  XawTextBlock text;
  XawTextPosition start, finish;
  static mem_addr prev_PC = 0;

  XawTextUnsetSelection(text_window);

  if (PC < TEXT_BOT || (PC > text_top && (PC < K_TEXT_BOT || PC > k_text_top)))
    return;

  sprintf (buf, "\n[0x%08x]", PC);
  text.firstPos = 0;
  text.length = strlen (buf);
  text.ptr = buf;
  text.format = FMT8BIT;

  /* Find start of line at PC: */
  start = XawTextSearch (text_window, prev_PC <= PC ? XawsdRight : XawsdLeft,
			 &text);

  if (start == XawTextSearchError)
    {
      start = XawTextSearch (text_window,
			     prev_PC > PC ? XawsdRight : XawsdLeft,
			     &text);
    }

  if (start == XawTextSearchError)
    {
      if (PC != 0x00400000) return;
      XawTextSetInsertionPoint (text_window, 0);
    }
  else
    XawTextSetInsertionPoint (text_window, start + 1);

  /* Find end of the line: */
  text.length = 1;
  finish = XawTextSearch (text_window, XawsdRight, &text);
  if (finish == XawTextSearchError)
    return;

  /* Highlight the line: */
  XawTextSetSelection(text_window, start + 1, finish);

  prev_PC = PC;
}


/* Display the contents of the data and stack segments, if they have
   been modified. */

static void
display_data_seg ()
{
  Arg args [2];
  static str_stream ss;

  if (!data_modified)
    return;

  ss_clear (&ss);
  format_data_segs(&ss);

  XtSetArg (args[0], XtNstring, ss_to_string (&ss));
  XtSetArg (args[1], XtNlength, ss_length (&ss));
  XtSetValues (data_window, args, TWO);

  data_modified = false;
}




/* IO facilities: */


void
write_output (port fp, char *fmt, ...)
{
  va_list args;
  Widget w;
  char io_buffer [IO_BUFFSIZE];

  va_start (args, fmt);
  w = (Widget) fp.f;

  if (w == console && !console_is_visible)
    {
      XtPopup (shell1, XtGrabNone);
      console_is_visible = 1;
    }

  vsprintf (io_buffer, fmt, args);
  va_end (args);

  write_text_to_window (w, io_buffer);

  /* Look for keyboard input (such as ^C) */
  while (XtAppPending (app_context))
    {
      XEvent event;

      XtAppNextEvent (app_context, &event);
      XtDispatchEvent (&event);
    }
}


/* Simulate the semantics of fgets, not gets, on an x-window. */

void
read_input (char *str, int str_size)
{
  char buffer[11];
  KeySym key;
  XComposeStatus compose;
  XEvent event;
  char *ptr;

  if (!console_is_visible)
    {
      XtPopup (shell1, XtGrabNone);
      console_is_visible = 1;
    }

  ptr = str;

  while (1 < str_size)		/* Reserve space for null */
    {
      XtAppNextEvent (app_context, &event);
      if (event.type == KeyPress)
	{
	  int chars = XLookupString (&event.xkey, buffer, 10, &key, &compose);
	  if ((key == XK_Return) || (key == XK_KP_Enter))
	    {
	      *ptr++ = '\n';

	      write_text_to_window (console, "\n");
	      break;
	    }
	  else if (*buffer == 3) /* ^C */
	    XtDispatchEvent (&event);
	  else if (chars > 0)
	    {
	      /* Event is a character, not a modifier key */
	      int n = (chars < str_size - 1 ? chars : str_size - 1);

	      strncpy (ptr, buffer, n);
	      ptr += n;
	      str_size -= n;

	      buffer[chars] = '\0';
	      write_text_to_window (console, buffer);
	    }
	}
      else
	XtDispatchEvent (&event);
    }

  if (0 < str_size)
    *ptr = '\0';
}


/* Checking for console input is messy with X windows. A key stroke appears
   as an X event, which must be read to check its type. In addition, the
   functions that read events all block.  So, when we check if console input
   is available, we may see an event for a keystroke. If so, save the
   keystroke for the next time the program reads a character. */

static int previous_char = -1;

int
console_input_available ()
{
  XEvent event;

  if (!console_is_visible)
    {
      XtPopup (shell1, XtGrabNone);
      console_is_visible = 1;
    }

  if (XtAppPending (app_context) != 0)
    {
      /* There is an event to process: */
      XtAppNextEvent (app_context, &event);
      if (event.type == KeyPress)
	{
	  /* Event is a key stroke: */
	  char buffer[11];
	  KeySym key;
	  XComposeStatus compose;
	  if (XLookupString (&event.xkey, buffer, 10, &key, &compose) > 0)
	    {
	      /* Event is a character, not a modifier key */
	      if ((key == XK_Return) || (key == XK_KP_Enter))
		previous_char = '\n';
	      else if (buffer[0] == 3)	/* ^C */
		XtDispatchEvent (&event);
	      else if (buffer[0] != 0)
		previous_char = (int)buffer[0];
	      return (1);		/* There is a character */
	    }
	}
      else
	{
	  XtDispatchEvent (&event);
	}
    }
  return (0);    		/* There is no character */
}

char
get_console_char ()
{
  /* assert (previous_char != -1); */
  return (char)previous_char;
}


void
put_console_char (char c)
{
  char buf[4];

  buf[0] = c;
  buf[1] = '\0';
  if (!console_is_visible)
    {
      XtPopup (shell1, XtGrabNone);
      console_is_visible = 1;
    }
  write_text_to_window (console, buf);
}



/* Print an error message. */

void
error (char *fmt, ...)
{
  va_list args;
  char io_buffer [IO_BUFFSIZE];

  va_start (args, fmt);
  vsprintf (io_buffer, fmt, args);
  va_end (args);
  if (message != 0)
    write_text_to_window (message, io_buffer);
  else
    fprintf (stderr, "%s", io_buffer);
}


/* Print the error message then exit. */

void
fatal_error (char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  fmt = va_arg (args, char *);

#ifdef NEED_VFPRINTF
  _doprnt (fmt, args, stderr);
#else
  vfprintf (stderr, fmt, args);
#endif
  exit (-1);
}


void
run_error (char *fmt, ...)
{
  va_list args;
  char io_buffer [IO_BUFFSIZE];

  va_start (args, fmt);
  vsprintf (io_buffer, fmt, args);
  va_end (args);
  if (message != 0)
    write_text_to_window (message, io_buffer);
  else
    fprintf (stderr, "%s", io_buffer);
  if (spim_is_running)
    longjmp (spim_top_level_env, 1);
}


static void
write_text_to_window (Widget w, char *s)
{
  XawTextBlock textblock;
  XawTextPosition ip = XawTextGetInsertionPoint (w);

  if (!s || strlen (s) == 0) return;

  textblock.firstPos = 0;
  textblock.length = strlen (s);
  textblock.ptr = s;
  textblock.format = FMT8BIT;

  XawTextReplace (w, ip, ip, &textblock);
  XawTextSetInsertionPoint (w,
			    XawTextGetInsertionPoint (w) + textblock.length);
}
