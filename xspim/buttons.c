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


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Cardinals.h>

#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>

#include "spim.h"
#include "string-stream.h"
#include "spim-utils.h"
#include "xspim.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "scanner.h"
#include "sym-tbl.h"
#include "buttons.h"


/* Local functions: */

static void add_breakpoint_action (Widget w, XtPointer client_data,
				   XtPointer call_data);
static void add_clear_button (Widget parent);
static void add_mode_button (Widget parent);
static void add_reload_button (Widget parent);
static void add_print_button (Widget parent);
static void add_terminal_button (Widget parent);
static void breakpoint_prompt (Widget button, XtPointer client_data,
			       XtPointer call_data);
static void breakpoint_prompt_destroyed (Widget w, XtPointer client_data,
					 XtPointer call_data);
static void clear_program_state_action (Widget w, XtPointer client_data,
					XtPointer call_data);
static void continue_action (Widget w, XtPointer client_data,
			     XtPointer call_data);
static void continue_prompt_destroyed (Widget w, XtPointer client_data,
				       XtPointer call_data);
static void delete_breakpoint_action (Widget w, XtPointer client_data,
				      XtPointer call_data);
static void destroy_popup_prompt (Widget w, XtPointer client_data,
				  XtPointer call_data);
static void help_action (Widget w, XtPointer ignore, XtPointer ignored);
static void load_prompt (Widget button, XtPointer client_data,
			 XtPointer call_data);
static void load_prompt_destroyed (Widget w, XtPointer client_data,
				   XtPointer call_data);
static void list_breakpoint_action (Widget w, XtPointer client_data,
				    XtPointer call_data);
static void noop ();
static void parse_print_value (Widget w, XtPointer client_data,
			       XtPointer call_data);
static void parse_set_value (Widget w, XtPointer client_data,
			     XtPointer call_data);
static Widget popup_one_field_dialog (Widget button, String name,
				      String field1_label, String field1_value,
				      String action_name,
				      void (*action) (/* ??? */),
				      String action2_name,
				      void (*action2) (/* ??? */),
				      Widget *field1_text);
static Widget popup_two_field_dialog (Widget button, String name,
				      String field1_label, String field1_value,
				      String field2_label, String field2_value,
				      String action_name,
				      void (*action) (/* ??? */),
				      String action2_name,
				      void (*action2) (/* ??? */),
				      Widget *field1_text,
				      Widget *field2_text);
static void print_mem_prompt (Widget button, XtPointer client_data,
			      XtPointer call_data);
static void print_prompt_destroyed (Widget w, XtPointer client_data,
				    XtPointer call_data);
static void print_symbol_action (Widget w, XtPointer client_data,
				 XtPointer call_data);
static void quit_action (Widget w, XtPointer client_data,XtPointer call_data);
static void quit_prompt (Widget button, XtPointer client_data,
			 XtPointer call_data);
static void quit_prompt_destroyed (Widget w, XtPointer client_data,
				   XtPointer call_data);
static void read_assm_file_action (Widget w, XtPointer client_data,
				   XtPointer call_data);
static void reload_action (Widget w, XtPointer client_data,
			   XtPointer call_data);
static void run_program_action (Widget w, XtPointer client_data,
				XtPointer call_data);
static void run_prompt (Widget button, XtPointer client_data,
			XtPointer call_data);
static void run_prompt_destroyed (Widget w, XtPointer client_data,
				  XtPointer call_data);
static void select_mode_action (Widget w, XtPointer client_data,
				XtPointer call_data);
static void set_value_action (char *location_str,char *value_str);
static void set_value_prompt (Widget button, XtPointer client_data,
			      XtPointer call_data);
static void set_value_destroyed (Widget w, XtPointer client_data,
				 XtPointer call_data);
static void step_continue_action (Widget w, XtPointer client_data,
				  XtPointer call_data);
static void step_program_action (Widget w, XtPointer client_data,
				 XtPointer call_data);
static void step_prompt (Widget button, XtPointer client_data,
			 XtPointer call_data);
static void step_prompt_destroyed (Widget w, XtPointer client_data,
				   XtPointer call_data);
static void warp_to_second_dialog (Widget widget, XEvent *event,
				   String *params, Cardinal *num_params);


/* Local variables: */

static Widget breakpointButton;
static void (*confirmAction) (Widget w, XtPointer client_data, XtPointer call_data) = (void (*)(_WidgetRec*, void*, void*))noop;
static char *program_file_name = NULL;	/* Retain last file's name. */
static char *command_line = NULL;	/* argv */



void
record_file_name_for_prompt (char *name)
{
  program_file_name = str_copy (name);
  command_line = program_file_name;
}


void
create_buttons (Widget parent)
{
  Widget button;
  Widget command;
  Arg args[10];


  XtSetArg (args[0], XtNlabel, "quit");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("quitbutton", commandWidgetClass, parent,
				  args, TWO);
  XtAddCallback (button, XtNcallback, quit_prompt, NULL);


  XtSetArg (args[0], XtNlabel, "load");
  XtSetArg (args[1], XtNwidth, button_width);
  command = XtCreateManagedWidget ("loadbutton", commandWidgetClass, parent,
				   args, TWO);
  XtAddCallback (command, XtNcallback, load_prompt, NULL);


  add_reload_button (parent);


  XtSetArg (args[0], XtNlabel, "run");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("runbutton", commandWidgetClass, parent,
				  args, TWO);
  XtAddCallback (button, XtNcallback, run_prompt, NULL);


  XtSetArg (args[0], XtNlabel, "step");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("stepbutton", commandWidgetClass, parent,
				  args, TWO);
  XtAddCallback (button, XtNcallback, step_prompt, NULL);


  add_clear_button (parent);


  XtSetArg (args[0], XtNlabel, "set value");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("setvaluebutton", commandWidgetClass, parent,
				  args, TWO);
  XtAddCallback (button, XtNcallback, set_value_prompt, NULL);


  add_print_button (parent);


  XtSetArg (args[0], XtNlabel, "breakpoints");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("bkptbutton", commandWidgetClass, parent,
				  args, TWO);
  breakpointButton = button;
  XtAddCallback (button, XtNcallback, breakpoint_prompt, NULL);


  XtSetArg (args[0], XtNlabel, "help");
  XtSetArg (args[1], XtNwidth, button_width);
  button = XtCreateManagedWidget ("helpbutton", commandWidgetClass, parent,
				  args, TWO);
  XtAddCallback (button, XtNcallback, help_action, NULL);


  add_terminal_button (parent);


  add_mode_button (parent);
}



/* *** Quit *** */

static Widget quit_popup = NULL;

static void
quit_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  Widget parent, dialog;
  Arg args[10];
  Position x, y;

  if (quit_popup == NULL)
    {
      parent = XtParent (button);

      XtTranslateCoords (button, (Position) 0, (Position) 0, &x, &y);
      XtSetArg (args[0], XtNx, x);
      XtSetArg (args[1], XtNy, y);
      quit_popup = XtCreatePopupShell ("prompt", transientShellWidgetClass,
				       parent, args, TWO);
      XtAddCallback (quit_popup, XtNdestroyCallback, quit_prompt_destroyed,
		     (XtPointer) 0);

      XtSetArg (args[0], XtNlabel, "quit?");
      dialog = XtCreateManagedWidget ("quit", dialogWidgetClass, quit_popup,
				      args, ONE);

      XawDialogAddButton (dialog, "quit", quit_action, (XtPointer) dialog);
      XawDialogAddButton (dialog, "abort command", destroy_popup_prompt,
			  (XtPointer) dialog);

    }
  confirmAction = quit_action;
  XtPopup (quit_popup, XtGrabNone);
}


static void
quit_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;

  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
  XtDestroyApplicationContext (XtWidgetToApplicationContext (w));
  exit (0);
}


static void
quit_prompt_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  quit_popup = NULL;
}


/* *** Load *** */

static Widget load_popup = NULL;

static void
load_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  Widget parent, dialog;
  Arg args[10];
  Position x, y;

  if (load_popup == NULL)
    {
      parent = XtParent (button);

      XtTranslateCoords (button, (Position) 0, (Position) 0, &x, &y);
      XtSetArg (args[0], XtNx, x);
      XtSetArg (args[1], XtNy, y);
      load_popup = XtCreatePopupShell ("popup", transientShellWidgetClass,
				      parent, args, TWO);
      XtAddCallback (load_popup, XtNdestroyCallback, load_prompt_destroyed,
		     (XtPointer) 0);

      if (program_file_name == NULL)
        {
          program_file_name = str_copy ("");
          command_line = program_file_name;
        }

      XtSetArg (args[0], XtNlabel, "input filename:");
      XtSetArg (args[1], XtNvalue, program_file_name);
      dialog = XtCreateManagedWidget ("dialog", dialogWidgetClass, load_popup,
				      args, TWO);

      XawDialogAddButton (dialog, "assembly file", read_assm_file_action,
			  (XtPointer) dialog);
      XawDialogAddButton (dialog, "abort command", destroy_popup_prompt,
			  (XtPointer) dialog);
    }

  confirmAction = read_assm_file_action;
  XtPopup (load_popup, XtGrabNone);
}


static void
read_assm_file_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;
  String value = XawDialogGetValueString (dialog);

  free (program_file_name);
  program_file_name = str_copy (value);
  command_line = program_file_name;
  read_file (program_file_name);

  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
}


static void
load_prompt_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  load_popup = NULL;
}


/* *** Run *** */

static Widget run_popup = NULL;
static Widget run_field1_text, run_field2_text;

static void
run_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  char sa[20];

  if (run_popup == NULL)
    {
      sprintf (sa, "0x%08x", starting_address ());
      run_popup = popup_two_field_dialog (button, "run program",
					 "starting address:", sa,
					 "args:", command_line,
					 "ok", (void (*)()) run_program_action,
					 NULL, NULL,
					 &run_field1_text, &run_field2_text);
      XtAddCallback (run_popup, XtNdestroyCallback, run_prompt_destroyed,
                     (XtPointer) 0);
    }
  confirmAction = run_program_action;
  XtPopup (run_popup, XtGrabNone);
}


static void
run_program_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  String sa, cl;
  mem_addr addr;
  Widget form = XtParent (w);

  XtSetArg (args[0], XtNstring, &sa);
  XtGetValues (run_field1_text, args, ONE);

  XtSetArg (args[0], XtNstring, &cl);
  XtGetValues (run_field2_text, args, ONE);

  destroy_popup_prompt (NULL, (XtPointer) form, NULL);

  command_line = str_copy(cl);
  initialize_stack (cl);

  addr = strtoul (sa, NULL, 0);
  if (addr > 0)
    start_program (addr);
}


static void
run_prompt_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  run_popup = NULL;
}


/* *** Step *** */

static char *step_size = NULL;	/* Retain step size */

static Widget step_popup = NULL;
static Widget step_field1_text;

static void
step_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  if (step_popup == NULL)
    {
      if (step_size == NULL)
	step_size = str_copy ("1");
      step_popup = popup_one_field_dialog (button, "step program",
					  "number of steps:", step_size,
					  "step", (void (*)())step_program_action,
					  "continue", (void (*)())step_continue_action,
					  &step_field1_text);
      XtAddCallback (step_popup, XtNdestroyCallback, step_prompt_destroyed,
                     (XtPointer) 0);
    }
  confirmAction = step_program_action;
  XtPopup (step_popup, XtGrabNone);
}


static void
step_program_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  String st;
  mem_addr addr;
  int steps;

  XtSetArg (args[0], XtNstring, &st);
  XtGetValues (step_field1_text, args, ONE);

  steps = atoi (st);
  free (step_size);
  step_size = str_copy (st);

  addr = PC == 0 ? starting_address () : PC;
  if (steps > 0 && addr > 0)
    execute_program (addr, steps, true, true);
  else
    error ("Cannot step %d steps from 0x%x\n", steps, addr);
}


static void
step_continue_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;

  XtPopdown (XtParent (dialog));
  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
  step_popup = NULL;
  execute_program (PC, DEFAULT_RUN_STEPS, false, false);
}


static void
step_prompt_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  step_popup = NULL;
}


/* *** Reload *** */

static void
add_reload_button (Widget parent)
{
  Widget command, menu, entry;
  Arg args[2];

  XtSetArg (args[0], XtNwidth, button_width);
  command = XtCreateManagedWidget ("reload", menuButtonWidgetClass,
				   parent, args, ONE);
  menu = XtCreatePopupShell ("menu", simpleMenuWidgetClass, command,
			     NULL, ZERO);

  entry = XtCreateManagedWidget ("assembly file", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, reload_action,
		 (XtPointer) 1);
}


static void
reload_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  if (program_file_name == NULL)
    return;

  write_output (message_out, "Memory and registers cleared\n\n");
  initialize_world (load_exception_handler ? exception_file_name : NULL, true);
  write_startup_message ();
  read_file (program_file_name);
}



/* *** Clear *** */

#define CLEAR_REGS 1
#define CLEAR_MEM_REGS 2
#define CLEAR_CONSOLE 3

static void
add_clear_button (Widget parent)
{
  Widget command, menu, entry;
  Arg args[2];

  XtSetArg (args[0], XtNwidth, button_width);
  command = XtCreateManagedWidget ("clear", menuButtonWidgetClass,
				   parent, args, ONE);
  menu = XtCreatePopupShell ("menu", simpleMenuWidgetClass, command,
			     NULL, ZERO);

  entry = XtCreateManagedWidget ("registers", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, clear_program_state_action,
		 (XtPointer) CLEAR_REGS);

  entry = XtCreateManagedWidget ("memory & registers", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, clear_program_state_action,
		 (XtPointer) CLEAR_MEM_REGS);

  entry = XtCreateManagedWidget ("console display", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, clear_program_state_action,
		 (XtPointer) CLEAR_CONSOLE);
}


static void
clear_program_state_action (Widget w, XtPointer client_data,
			    XtPointer call_data)
{
  long clear_op = (long) client_data;

  switch (clear_op)
    {
    case CLEAR_REGS:
      write_output (message_out, "Registers cleared\n\n");
      initialize_registers ();
      break;

    case CLEAR_MEM_REGS:
      write_output (message_out, "Memory and registers cleared\n\n");
      initialize_world (load_exception_handler ? exception_file_name : NULL, true);
      write_startup_message ();
      break;

    case CLEAR_CONSOLE:
      write_output (message_out, "Console display cleared\n\n");
      clear_console_display ();
      break;

    default:
      fatal_error("Unknown action: %d\n", clear_op);
    }

  redisplay_text ();
  redisplay_data ();
}


/* *** Set Value *** */

static Widget set_value_popup = NULL;
static Widget set_field1_text, set_field2_text;

static void
set_value_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  if (set_value_popup == NULL)
    {
      set_value_popup = popup_two_field_dialog (button, "set value",
					   "register/location:", "",
					   "value:", "",
					   "set", (void (*)())parse_set_value,
					   NULL, NULL,
					   &set_field1_text, &set_field2_text);
      XtAddCallback (set_value_popup, XtNdestroyCallback,
		     set_value_destroyed, (XtPointer) 0);
    }
  confirmAction = parse_set_value;
  XtPopup (set_value_popup, XtGrabNone);
}


static void
parse_set_value (Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  String value1, value2;
  Widget form = XtParent (w);

  XtSetArg (args[0], XtNstring, &value1);
  XtGetValues (set_field1_text, args, ONE);

  XtSetArg (args[0], XtNstring, &value2);
  XtGetValues (set_field2_text, args, ONE);

  destroy_popup_prompt (NULL, (XtPointer) form, NULL);

  set_value_action (value1, value2);
}


static void
set_value_action (char *location_str, char *value_str)
{
  unsigned long value;
  int reg_no;

  if (strlen (value_str) == 3
      && value_str[0] == '\''
      && value_str[2] == '\'')
    {
      /* Literal of the form: 'c' */
      value = value_str[1];
    }
  else
    {
      value = strtol (value_str, NULL, 0);
    }
  reg_no = register_name_to_number (location_str);

  if (reg_no < 0)
    if (*location_str == '$' || *location_str == 'r' || *location_str == 'R')
      reg_no = register_name_to_number (location_str + 1);

  if (reg_no == 0)
    error ("Cannot modify register 0\n");
  else if (reg_no > 0)
    R[reg_no] = (reg_word)value;
  else if (streq (location_str, "Status") || streq (location_str, "status"))
    CP0_Status = (reg_word)value;
  else if (streq (location_str, "PC") || streq (location_str, "pc"))
    PC = (reg_word)value;
  else if (streq (location_str, "EPC") | streq (location_str, "epc"))
    CP0_EPC = (reg_word)value;
  else
    {
      char *eptr;
      mem_addr addr;

      /* Try to parse string as a number */
      addr = strtol (location_str, &eptr, 0);
      if (eptr == location_str)
	error ("Unknown register selected: %s\n", location_str);
      else
	set_mem_word (addr, value);
    }
  redisplay_data ();
}


static void
set_value_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  set_value_popup = NULL;
}


/* *** Print *** */

static void
add_print_button (Widget parent)
{
  Widget command, menu, entry;
  Arg args[2];

  XtSetArg (args[0], XtNwidth, button_width);
  command = XtCreateManagedWidget ("print", menuButtonWidgetClass,
				   parent, args, ONE);
  menu = XtCreatePopupShell ("menu", simpleMenuWidgetClass, command,
			     NULL, ZERO);

  entry = XtCreateManagedWidget ("memory location(s)", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, print_mem_prompt, NULL);

  entry = XtCreateManagedWidget ("global symbols", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, print_symbol_action, NULL);
}


static Widget print_popup = NULL;
static Widget print_field1_text, print_field2_text;

static void
print_mem_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  if (print_popup == NULL)
    {
      print_popup = popup_two_field_dialog (XtParent (button), "print memory",
					"from", "",
					"to", "",
					"print", (void (*)())parse_print_value,
					NULL, NULL,
					&print_field1_text,
					&print_field2_text);
      XtAddCallback (print_popup, XtNdestroyCallback, print_prompt_destroyed,
		     (XtPointer) 0);
    }
  confirmAction = parse_print_value;
  XtPopup (print_popup, XtGrabNone);
}


static void
parse_print_value (Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  String value1, value2;
  Widget form = XtParent (w);

  XtSetArg (args[0], XtNstring, &value1);
  XtGetValues (print_field1_text, args, ONE);

  XtSetArg (args[0], XtNstring, &value2);
  XtGetValues (print_field2_text, args, ONE);

  XtPopdown (XtParent (form));
  destroy_popup_prompt (NULL, (XtPointer) form, NULL);

  if (!streq (value1, ""))
    {
      mem_addr from, to;

      from = strtoul (value1, NULL, 0);
      to = strtoul (value2, NULL, 0);
      if (streq (value2, ""))
	print_mem (from);
      else
	for ( ; from <= to; from+= BYTES_PER_WORD)
	  print_mem (from);
    }
}


static void
print_symbol_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  print_symbols ();
}


static void
print_prompt_destroyed (Widget w, XtPointer client_data, XtPointer call_data)
{
  print_popup = NULL;
}


/* *** Breakpoints **** */

static char *breakpoint_addr = NULL; /* Retain last breakpoint address */

static Widget bkpt_popup = NULL;

static void
breakpoint_prompt (Widget button, XtPointer client_data, XtPointer call_data)
{
  Widget parent, dialog;
  Arg args[10];
  Position x, y;

  if (bkpt_popup == NULL)
    {
      parent = XtParent (button);

      XtTranslateCoords (button, (Position) 0, (Position) 0, &x, &y);
      XtSetArg (args[0], XtNx, x);
      XtSetArg (args[1], XtNy, y);
      bkpt_popup = XtCreatePopupShell ("popup", transientShellWidgetClass,
				      parent, args, TWO);

      if (breakpoint_addr == NULL)
	breakpoint_addr = str_copy ("");
      XtSetArg (args[0], XtNlabel, "address:");
      XtSetArg (args[1], XtNvalue, breakpoint_addr);
      dialog = XtCreateManagedWidget ("dialog", dialogWidgetClass,
				      bkpt_popup,
				      args, TWO);
      XtAddCallback (bkpt_popup, XtNdestroyCallback,
		     breakpoint_prompt_destroyed, (XtPointer) 0);

      XawDialogAddButton (dialog, "add",
			  add_breakpoint_action, (XtPointer) dialog);
      XawDialogAddButton (dialog, "delete",
			  delete_breakpoint_action, (XtPointer) dialog);
      XawDialogAddButton (dialog, "list",
			  list_breakpoint_action, (XtPointer) dialog);
      XawDialogAddButton (dialog, "abort command", destroy_popup_prompt,
			  (XtPointer) dialog);
    }

  confirmAction = add_breakpoint_action;
  XtPopup (bkpt_popup, XtGrabNone);
}


static void
add_breakpoint_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;
  String value = XawDialogGetValueString (dialog);
  mem_addr addr;

  free (breakpoint_addr);
  breakpoint_addr = str_copy (value);

  while (*breakpoint_addr == ' ') breakpoint_addr++;
  if (isdigit (*breakpoint_addr))
    addr = strtoul (value, NULL, 0);
  else
    addr = find_symbol_address (breakpoint_addr);

  add_breakpoint (addr);

  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
}


static void
delete_breakpoint_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;
  String value = XawDialogGetValueString (dialog);
  mem_addr addr;

  free (breakpoint_addr);
  breakpoint_addr = str_copy (value);
  addr = strtoul (value, NULL, 0);

  delete_breakpoint (addr);

  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
}


static void
list_breakpoint_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  list_breakpoints ();
}


static void
breakpoint_prompt_destroyed (Widget w, XtPointer client_data,
			     XtPointer call_data)
{
  bkpt_popup = NULL;
}


/* *** Help *** */

static void
help_action (Widget w, XtPointer ignore, XtPointer ignored)
{
  static char * msg = "\nSPIM is a MIPS R2000 simulator.\n\
Copyright (C) 1990-2010, James R. Larus\n\n\
quit  -- Exit from the simulator\n\
load -- Read a file into memory\n\
run -- Execute a program\n\
step -- Single-step through a program\n\
clear -- Reinitialize registers or memory\n\
set value -- Set the value in register or memory\n\
print -- Print the value in register or memory\n\
breakpoint -- Set or delete a breakpoint\n\
help -- This message\n\
terminal -- Raise or hide console window\n\
mode -- Set SPIM operating modes\n";

  write_output (message_out, msg);
}


/* *** Terminal *** */

static void
add_terminal_button (Widget parent)
{
  Widget command, menu, entry;
  Arg args[2];

  XtSetArg (args[0], XtNwidth, button_width);
  command = XtCreateManagedWidget ("terminal", menuButtonWidgetClass,
				   parent, args, ONE);
  menu = XtCreatePopupShell ("menu", simpleMenuWidgetClass, command,
			     NULL, ZERO);

  entry = XtCreateManagedWidget ("popup console", smeBSBObjectClass, menu,
				 args, ONE);
  XtAddCallback (entry, XtNcallback, popup_console, NULL);
}


/* *** Mode *** */

static Widget modeCommand;

static void
add_mode_button (Widget parent)
{
  Widget menu, entry;
  Arg args[2];

  XtSetArg (args[0], XtNwidth, button_width);
  modeCommand = XtCreateManagedWidget ("mode", menuButtonWidgetClass,
				       parent, args, ONE);
  menu = XtCreatePopupShell ("menu", simpleMenuWidgetClass, modeCommand,
			     NULL, ZERO);

  XtSetArg (args[0], XtNleftMargin, 20);
  entry = XtCreateManagedWidget ("bare", smeBSBObjectClass, menu, args, ONE);
  XtAddCallback (entry, XtNcallback, select_mode_action, NULL);
  if (bare_machine)
    {
      XtSetArg (args[0], XtNleftBitmap, mark);
      XtSetValues (entry, args, ONE);
    }

  XtSetArg (args[0], XtNleftMargin, 20);
  entry = XtCreateManagedWidget ("quiet", smeBSBObjectClass, menu, args, ONE);
  XtAddCallback (entry, XtNcallback, select_mode_action, NULL);
  if (quiet)
    {
      XtSetArg (args[0], XtNleftBitmap, mark);
      XtSetValues (entry, args, ONE);
    }
}


static void
select_mode_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[10];
  String name = XtName (w);

  if (streq (name, "bare"))
    {
      bare_machine = !bare_machine;
      if (bare_machine)
	XtSetArg (args[0], XtNleftBitmap, mark);
      else
	XtSetArg (args[0], XtNleftBitmap, None);
      XtSetValues (w, args, ONE);
    }
  else if (streq (name, "quiet"))
    {
      quiet = !quiet;
      if (quiet)
	XtSetArg (args[0], XtNleftBitmap, mark);
      else
	XtSetArg (args[0], XtNleftBitmap, None);
      XtSetValues (w, args, ONE);
    }
}


/* *** Continue *** */

static Widget continue_popup = NULL;

void
continue_prompt (int interrupt_seen)
{
  Widget dialog;
  Arg args[10];
  Position x, y;
  char msg[256];

  if (continue_popup != NULL)
    XtDestroyWidget (continue_popup);
  XtTranslateCoords (breakpointButton, (Position) 0, (Position) 0, &x, &y);
  XtSetArg (args[0], XtNx, x);
  XtSetArg (args[1], XtNy, y);
  continue_popup = XtCreatePopupShell ("prompt", transientShellWidgetClass,
				      breakpointButton, args, TWO);
  XtAddCallback (continue_popup, XtNdestroyCallback,
		 continue_prompt_destroyed, (XtPointer) 0);

  if (interrupt_seen)
    sprintf (msg, "execution interrupt at 0x%08x", PC);
  else
    sprintf (msg, "breakpoint encountered at 0x%08x", PC);
  XtSetArg (args[0], XtNlabel, msg);
  dialog = XtCreateManagedWidget ("continue", dialogWidgetClass,
				  continue_popup, args, ONE);

  XawDialogAddButton (dialog, "continue", continue_action,
		      (XtPointer) dialog);
  XawDialogAddButton (dialog, "abort command", destroy_popup_prompt,
		      (XtPointer) dialog);

  confirmAction = continue_action;
  XtPopup (continue_popup, XtGrabNone);
}


static void
continue_action (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget dialog = (Widget) client_data;

  XtPopdown (XtParent (dialog));
  destroy_popup_prompt (NULL, (XtPointer) dialog, (XtPointer) NULL);
  continue_popup = NULL;
  execute_program (PC, 1, false, true); /* Step over breakpoint */
  execute_program (PC, DEFAULT_RUN_STEPS - 1, false, false);
}


static void
continue_prompt_destroyed (Widget w, XtPointer client_data,
			   XtPointer call_data)
{
  continue_popup = NULL;
}




void
confirm (Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
  Widget dialog = XtParent (widget);
  (*confirmAction) (widget, (XtPointer) dialog, (XtPointer) NULL);
}


static void
destroy_popup_prompt (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget popup = XtParent ((Widget) client_data);

  confirmAction = (void (*)(_WidgetRec*, void*, void*))noop;
  XtDestroyWidget (popup);
}


static void
noop ()
{
}


static Widget
popup_one_field_dialog (Widget button, String name,
                        String field1_label, String field1_value,
			String action1_name, void (*action1) (/* ??? */),
                        String action2_name, void (*action2) (/* ??? */),
                        Widget *field1_text)
{
  Widget parent = XtParent (button);
  Widget popup, form;
  Widget labelx, field1;
  Widget button1, button2, cancelbutton;
  Arg args[10];
  Position x, y;
  static XtActionsRec action_table []
    = {{"warp_to_second_dialog", warp_to_second_dialog},};

  XtTranslateCoords (button, (Position) 0, (Position) 0, &x, &y);
  XtSetArg (args[0], XtNx, x);
  XtSetArg (args[1], XtNy, y);

  popup = XtCreatePopupShell ("prompt", transientShellWidgetClass, parent, args, TWO);

  form = XtCreateManagedWidget ("form", formWidgetClass, popup, NULL, ZERO);

  XtSetArg (args[0], XtNlabel, name);
  XtSetArg (args[1], XtNborderWidth, 0);
  labelx = XtCreateManagedWidget ("label", labelWidgetClass, form, args, TWO);

  XtSetArg (args[0], XtNfromVert, labelx);
  XtSetArg (args[1], XtNborderWidth, 0);
  XtSetArg (args[2], XtNlabel, field1_label);
  field1 = XtCreateManagedWidget ("field1", labelWidgetClass, form, args, THREE);

  XtSetArg (args[0], XtNfromHoriz, field1);
  XtSetArg (args[1], XtNfromVert, labelx);
  XtSetArg (args[2], XtNeditType, "edit");
  XtSetArg (args[3], XtNstring, field1_value);
  XtSetArg (args[4], XtNtype, XawAsciiString);
  *field1_text = XtCreateManagedWidget ("field1_text", asciiTextWidgetClass, form, args, FIVE);

  XtOverrideTranslations (*field1_text,
			  XtParseTranslationTable
			  ("#override \n <Key>Return:warp_to_second_dialog()"));
  XtAppAddActions (app_context, action_table, XtNumber (action_table));

  button1 = XtCreateManagedWidget (action1_name, commandWidgetClass, form, args, ONE);
  XtAddCallback (button1, XtNcallback, (void (*)(_WidgetRec*, void*, void*))action1, (XtPointer) form);

  if (action2 != NULL)
    {
      XtSetArg (args[0], XtNfromHoriz, button1);
      button2 = XtCreateManagedWidget (action2_name, commandWidgetClass, form, args, ONE);
      XtAddCallback (button2, XtNcallback, (void (*)(_WidgetRec*, void*, void*))action2, (XtPointer) form);
    }

  XtSetArg (args[0], XtNfromHoriz, action2 == NULL ? button1 : button2);
  cancelbutton = XtCreateManagedWidget ("abort command", commandWidgetClass, form, args, ONE);
  XtAddCallback (cancelbutton, XtNcallback, destroy_popup_prompt, (XtPointer) form);

  return (popup);
}


static Widget
popup_two_field_dialog (Widget button, String name,
                        String field1_label, String field1_value,
                        String field2_label, String field2_value,
                        String action1_name, void (*action1) (/* ??? */),
                        String action2_name, void (*action2) (/* ??? */),
                        Widget *field1_text, Widget *field2_text)
{
  Widget popup, form;
  Widget labelx, field1, field2;
  Widget button1, button2, cancelbutton;
  Widget parent = XtParent (button);
  Arg args[10];
  Position x, y;
  static XtActionsRec action_table []
    = {{"warp_to_second_dialog", warp_to_second_dialog},};

  XtTranslateCoords (button, (Position) 0, (Position) 0, &x, &y);

  XtSetArg (args[0], XtNx, x);
  XtSetArg (args[1], XtNy, y);
  popup = XtCreatePopupShell ("prompt", transientShellWidgetClass, parent,
			      args, TWO);

  form = XtCreateManagedWidget ("form", formWidgetClass, popup, NULL, ZERO);

  XtSetArg (args[0], XtNlabel, name);
  XtSetArg (args[1], XtNborderWidth, 0);
  labelx = XtCreateManagedWidget ("label", labelWidgetClass, form, args, TWO);

  XtSetArg (args[0], XtNfromVert, labelx);
  XtSetArg (args[1], XtNborderWidth, 0);
  XtSetArg (args[2], XtNlabel, field1_label);
  field1 = XtCreateManagedWidget ("field1", labelWidgetClass, form, args,
				  THREE);

  XtSetArg (args[0], XtNfromHoriz, field1);
  XtSetArg (args[1], XtNfromVert, labelx);
  XtSetArg (args[2], XtNeditType, "edit");
  XtSetArg (args[3], XtNstring, field1_value);
  XtSetArg (args[4], XtNtype, XawAsciiString);
  *field1_text = XtCreateManagedWidget ("field1_text", asciiTextWidgetClass,
					form, args, FIVE);
  XtOverrideTranslations (*field1_text,
			  XtParseTranslationTable
			  ("#override \n <Key>Return:warp_to_second_dialog()"));
  XtAppAddActions (app_context, action_table, XtNumber (action_table));

  XtSetArg (args[0], XtNfromVert, *field1_text);
  XtSetArg (args[1], XtNborderWidth, 0);
  XtSetArg (args[2], XtNlabel, field2_label);
  field2 = XtCreateManagedWidget ("field2", labelWidgetClass, form, args,
				  THREE);

  XtSetArg (args[0], XtNfromHoriz, field1);
  XtSetArg (args[1], XtNfromVert, *field1_text);
  XtSetArg (args[2], XtNeditType, "edit");
  XtSetArg (args[3], XtNstring, field2_value);
  XtSetArg (args[4], XtNtype, XawAsciiString);
  *field2_text = XtCreateManagedWidget ("field2_text", asciiTextWidgetClass,
					form, args, FIVE);
  XtOverrideTranslations (*field2_text,
			  XtParseTranslationTable
			  ("#override \n <Key>Return: confirm()"));

  XtSetArg (args[0], XtNfromVert, *field2_text);
  button1 = XtCreateManagedWidget (action1_name, commandWidgetClass, form,
				   args, ONE);
  XtAddCallback (button1, XtNcallback, (void (*)(_WidgetRec*, void*, void*))action1, (XtPointer) form);

  if (action2 != NULL)
    {
      XtSetArg (args[0], XtNfromHoriz, button1);
      XtSetArg (args[1], XtNfromVert, *field2_text);
      button2 = XtCreateManagedWidget (action2_name, commandWidgetClass, form,
				       args, TWO);
      XtAddCallback (button2, XtNcallback, (void (*)(_WidgetRec*, void*, void*))action2, (XtPointer) form);
    }

  XtSetArg (args[0], XtNfromHoriz, action2 == NULL ? button1 : button2);
  XtSetArg (args[1], XtNfromVert, *field2_text);
  cancelbutton = XtCreateManagedWidget ("abort command", commandWidgetClass,
					form, args, TWO);
  XtAddCallback (cancelbutton, XtNcallback, destroy_popup_prompt,
		 (XtPointer) form);

  return (popup);
}


static void
warp_to_second_dialog (Widget widget, XEvent *event, String *params,
		       Cardinal *num_params)
{
  Widget form = XtParent (widget);
  Widget second_dialog;

  second_dialog = XtNameToWidget (form, "field2_text");
  if (second_dialog)
    XWarpPointer (XtDisplay (second_dialog), None, XtWindow (second_dialog),
		  0, 0, 0, 0, 0, 10);
}
