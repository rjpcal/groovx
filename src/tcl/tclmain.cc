///////////////////////////////////////////////////////////////////////
//
// tclmain.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:34:05 2002
// written: Mon Jul 22 16:34:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_CC_DEFINED
#define TCLMAIN_CC_DEFINED

#include "tcl/tclmain.h"

#include <iostream>
#include <tk.h>
#include <unistd.h>

namespace
{

typedef struct ThreadSpecificData {
  Tcl_Interp *interp;         /* Interpreter for this thread. */
  Tcl_DString command;        /* Used to assemble lines of terminal input
                               * into Tcl commands. */
  Tcl_DString line;           /* Used to read the next line from the
                               * terminal input. */
  int tty;                    /* Non-zero means standard input is a
                               * terminal-like device.  Zero means it's
                               * a file. */
} ThreadSpecificData;
Tcl_ThreadDataKey dataKey;

}

//---------------------------------------------------------------------
//
// Tcl::Main::Main()
//
//---------------------------------------------------------------------

Tcl::Main::Main(int argc, char** argv) :
  itsInterp(Tcl_CreateInterp()),
  itsStartupFileName(0),
  itsInChannel(0 /* NULL */)
{
  Tcl_FindExecutable(argv[0]);

  // Set up thread-specific data

  ThreadSpecificData* tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  tsdPtr->interp = itsInterp;

  Tcl_DStringInit(&tsdPtr->command);
  Tcl_DStringInit(&tsdPtr->line);

  tsdPtr->tty = isatty(0);

  // Parse command-line arguments.  If the next argument doesn't start with
  // a "-" then strip it off and use it as the name of a script file to
  // process.

  if ((argc > 1) && (argv[1][0] != '-'))
    {
      itsStartupFileName = argv[1];
      --argc;
      ++argv;
    }

  // Make command-line arguments available in the Tcl variables "argc" and
  // "argv".

  Tcl_SetVar2Ex(itsInterp, const_cast<char*>("argc"), NULL,
                Tcl_NewIntObj(argc-1), TCL_GLOBAL_ONLY);

  char* args = Tcl_Merge(argc-1, (const char **)argv+1);
  Tcl_SetVar(itsInterp, const_cast<char*>("argv"),
             args, TCL_GLOBAL_ONLY);
  Tcl_Free(args);

  Tcl_SetVar(itsInterp, const_cast<char*>("argv0"),
             (itsStartupFileName == NULL) ? argv[0] : itsStartupFileName,
             TCL_GLOBAL_ONLY);

  // Set up the "tcl_interactive" variable.

  Tcl_SetVar(itsInterp, const_cast<char*>("tcl_interactive"),
             ((itsStartupFileName == NULL) && tsdPtr->tty) ? "1" : "0",
             TCL_GLOBAL_ONLY);
}

//---------------------------------------------------------------------
//
// Tcl::Main::run()
//
//---------------------------------------------------------------------

void Tcl::Main::run()
{
  ThreadSpecificData* tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  /*
   * Invoke the script specified on the command line, if any.
   */

  if (itsStartupFileName != NULL)
    {
      Tcl_ResetResult(itsInterp);
      int code = Tcl_EvalFile(itsInterp, itsStartupFileName);
      if (code != TCL_OK)
        {
          // ensure errorInfo is set properly:
          Tcl_AddErrorInfo(itsInterp, "");

          std::cerr << Tcl_GetVar(itsInterp, const_cast<char*>("errorInfo"),
                                  TCL_GLOBAL_ONLY)
                    << "\nError in startup script\n";
          Tcl_DeleteInterp(itsInterp);
          Tcl_Exit(1);
        }
      tsdPtr->tty = 0;
    }
  else
    {
      // Evaluate the .rc file, if one has been specified.
      Tcl_SourceRCFile(itsInterp);

      // Set up a stdin channel handler.
      itsInChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (itsInChannel)
        {
          Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                                   &stdinProc, (ClientData) this);
        }
      if (tsdPtr->tty)
        {
          prompt(false);
        }
    }

  Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel)
    {
      Tcl_Flush(outChannel);
    }
  Tcl_ResetResult(itsInterp);

  // Loop indefinitely, waiting for commands to execute, until there are no
  // main windows left, then exit.

  while (Tk_GetNumMainWindows() > 0)
    {
      Tcl_DoOneEvent(0);
    }
  Tcl_DeleteInterp(itsInterp);
  Tcl_Exit(0);
}

//---------------------------------------------------------------------
//
// Tcl::Main::defaultPrompt()
//
//---------------------------------------------------------------------

void Tcl::Main::defaultPrompt(bool partial)
{
  if (!partial)
    {
      /*
       * We must check that outChannel is a real channel - it
       * is possible that someone has transferred stdout out of
       * this interpreter with "interp transfer".
       */

      Tcl_Channel outChannel = Tcl_GetChannel(itsInterp, "stdout", NULL);
      if (outChannel != (Tcl_Channel) NULL)
        {
          Tcl_WriteChars(outChannel, "% ", 2);
          Tcl_Flush(outChannel);
        }
    }
}

//---------------------------------------------------------------------
//
// Tcl::Main::prompt()
//
//---------------------------------------------------------------------

void Tcl::Main::prompt(bool partial)
{
  Tcl_Obj* promptCmd =
    Tcl_GetVar2Ex(itsInterp,
                  const_cast<char*>
                    (partial ? "tcl_prompt2" : "tcl_prompt1"),
                  NULL, TCL_GLOBAL_ONLY);

  if (promptCmd == NULL)
    {
      defaultPrompt(partial);
    }
  else
    {
      int code = Tcl_EvalObjEx(itsInterp, promptCmd, TCL_EVAL_GLOBAL);
      if (code != TCL_OK)
        {
          Tcl_AddErrorInfo(itsInterp,
                           "\n    (script that generates prompt)");
          /*
           * We must check that errChannel is a real channel - it
           * is possible that someone has transferred stderr out of
           * this interpreter with "interp transfer".
           */

          Tcl_Channel errChannel = Tcl_GetChannel(itsInterp, "stderr", NULL);
          if (errChannel != (Tcl_Channel) NULL)
            {
              Tcl_WriteObj(errChannel, Tcl_GetObjResult(itsInterp));
              Tcl_WriteChars(errChannel, "\n", 1);
            }
          defaultPrompt(partial);
        }
      else
        {
          Tcl_Channel outChannel = Tcl_GetChannel(itsInterp, "stdout", NULL);
          if (outChannel != 0)
            {
              Tcl_Flush(outChannel);
            }
        }
    }
}


//---------------------------------------------------------------------
//
// This procedure is invoked by the event dispatcher whenever standard
// input becomes readable.  It grabs the next line of input characters,
// adds them to a command being assembled, and executes the command if it's
// complete.
//
//---------------------------------------------------------------------

void Tcl::Main::stdinProc(ClientData clientData, int /*mask*/)
{
  static int gotPartial = 0;

  Tcl::Main* app = static_cast<Tcl::Main*>(clientData);

  ThreadSpecificData* tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  Tcl_Interp* interp = tsdPtr->interp;

  int count = Tcl_Gets(app->itsInChannel, &tsdPtr->line);

  if (count < 0)
    {
      if (!gotPartial)
        {
          if (tsdPtr->tty)
            {
              Tcl_Exit(0);
            }
          else
            {
              Tcl_DeleteChannelHandler(app->itsInChannel,
                                       &stdinProc, (ClientData) app);
            }
          return;
        }
    }

  Tcl_DStringAppend(&tsdPtr->command,
                    Tcl_DStringValue(&tsdPtr->line), -1);
  char* cmd = Tcl_DStringAppend(&tsdPtr->command, "\n", -1);
  Tcl_DStringFree(&tsdPtr->line);
  if (!Tcl_CommandComplete(cmd))
    {
      gotPartial = 1;
    }
  else
    {
      gotPartial = 0;

      /*
       * Disable the stdin channel handler while evaluating the command;
       * otherwise if the command re-enters the event loop we might
       * process commands from stdin before the current command is
       * finished.  Among other things, this will trash the text of the
       * command being evaluated.
       */

      Tcl_CreateChannelHandler(app->itsInChannel, 0,
                               &stdinProc, (ClientData) app);
      int code = Tcl_RecordAndEval(interp, cmd, TCL_EVAL_GLOBAL);

      app->itsInChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (app->itsInChannel)
        {
          Tcl_CreateChannelHandler(app->itsInChannel, TCL_READABLE,
                                   &stdinProc, (ClientData) app);
        }
      Tcl_DStringFree(&tsdPtr->command);
      if (Tcl_GetStringResult(interp)[0] != '\0')
        {
          if ((code != TCL_OK) || (tsdPtr->tty))
            {
              Tcl_Channel outChan = Tcl_GetStdChannel(TCL_STDOUT);
              if (outChan)
                {
                  Tcl_WriteObj(outChan, Tcl_GetObjResult(interp));
                  Tcl_WriteChars(outChan, "\n", 1);
                }
            }
        }
    }

  /*
   * Output a prompt.
   */

  if (tsdPtr->tty)
    {
      app->prompt(gotPartial);
    }
  Tcl_ResetResult(interp);
}

static const char vcid_tclmain_cc[] = "$Header$";
#endif // !TCLMAIN_CC_DEFINED
