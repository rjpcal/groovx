///////////////////////////////////////////////////////////////////////
//
// tclmain.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:34:05 2002
// written: Thu Sep  5 16:26:38 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_CC_DEFINED
#define TCLMAIN_CC_DEFINED

#include "tcl/tclmain.h"

#include "tcl/tclsafeinterp.h"

#include <iostream>
#include <tk.h>
#include <unistd.h>

#include "util/trace.h"
#include "util/debug.h"

namespace Tcl
{
  class MainImpl;
}

// Singleton implementation class for Tcl::Main
class Tcl::MainImpl
{
private:
  static MainImpl* theMainImpl;

  MainImpl(int argc, char** argv);

public:
  static void create(int argc, char** argv)
  {
    theMainImpl = new MainImpl(argc, argv);
  }

  static MainImpl* get()
  {
    Assert(theMainImpl != 0);

    return theMainImpl;
  }

  void defaultPrompt(bool partial);
  void prompt(bool partial);

  static void stdinProc(ClientData /*clientData*/, int /*mask*/);

  Tcl_Interp* interp() const { return safeInterp.intp(); }

  Tcl::Interp safeInterp;
  const char* startupFileName;
  Tcl_Channel inChannel;
};

Tcl::MainImpl* Tcl::MainImpl::theMainImpl = 0;

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
// Tcl::MainImpl::MainImpl()
//
//---------------------------------------------------------------------

Tcl::MainImpl::MainImpl(int argc, char** argv) :
  safeInterp(Tcl_CreateInterp()),
  startupFileName(0),
  inChannel(0)
{
DOTRACE("Tcl::MainImpl::MainImpl");

  Tcl_FindExecutable(argv[0]);

  // Set up thread-specific data

  ThreadSpecificData* tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  tsdPtr->interp = this->interp();

  Tcl_DStringInit(&tsdPtr->command);
  Tcl_DStringInit(&tsdPtr->line);

  tsdPtr->tty = isatty(0);

  // Parse command-line arguments.  If the next argument doesn't start with
  // a "-" then strip it off and use it as the name of a script file to
  // process.

  if ((argc > 1) && (argv[1][0] != '-'))
    {
      this->startupFileName = argv[1];
      --argc;
      ++argv;
    }

  // Make command-line arguments available in the Tcl variables "argc" and
  // "argv".

  this->safeInterp.setGlobalVar("argc", argc-1);

  char* args = Tcl_Merge(argc-1, (const char **)argv+1);
  this->safeInterp.setGlobalVar("argv", args);
  Tcl_Free(args);

  this->safeInterp.setGlobalVar("argv0",
                                (this->startupFileName == NULL)
                                ? argv[0]
                                : this->startupFileName);

  this->safeInterp.setGlobalVar("tcl_interactive",
                                ((this->startupFileName == NULL) && tsdPtr->tty)
                                ? 1 : 0);
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::defaultPrompt()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::defaultPrompt(bool partial)
{
  if (!partial)
    {
      /*
       * We must check that outChannel is a real channel - it
       * is possible that someone has transferred stdout out of
       * this interpreter with "interp transfer".
       */

      Tcl_Channel outChannel = Tcl_GetChannel(interp(), "stdout", NULL);
      if (outChannel != (Tcl_Channel) NULL)
        {
          Tcl_WriteChars(outChannel, "% ", 2);
          Tcl_Flush(outChannel);
        }
    }
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::prompt()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::prompt(bool partial)
{
  Tcl_Obj* promptCmd =
    Tcl_GetVar2Ex(interp(),
                  const_cast<char*>
                    (partial ? "tcl_prompt2" : "tcl_prompt1"),
                  NULL, TCL_GLOBAL_ONLY);

  if (promptCmd == NULL)
    {
      defaultPrompt(partial);
    }
  else
    {
      int code = Tcl_EvalObjEx(interp(), promptCmd, TCL_EVAL_GLOBAL);
      if (code != TCL_OK)
        {
          Tcl_AddErrorInfo(interp(),
                           "\n    (script that generates prompt)");
          /*
           * We must check that errChannel is a real channel - it
           * is possible that someone has transferred stderr out of
           * this interpreter with "interp transfer".
           */

          Tcl_Channel errChannel = Tcl_GetChannel(interp(), "stderr", NULL);
          if (errChannel != (Tcl_Channel) NULL)
            {
              Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp()));
              Tcl_WriteChars(errChannel, "\n", 1);
            }
          defaultPrompt(partial);
        }
      else
        {
          Tcl_Channel outChannel = Tcl_GetChannel(interp(), "stdout", NULL);
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

void Tcl::MainImpl::stdinProc(ClientData /*clientData*/, int /*mask*/)
{
  static int gotPartial = 0;

  Tcl::MainImpl* rep = Tcl::MainImpl::get();

  ThreadSpecificData* tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  Tcl_Interp* interp = tsdPtr->interp;

  int count = Tcl_Gets(rep->inChannel, &tsdPtr->line);

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
              Tcl_DeleteChannelHandler(rep->inChannel,
                                       &stdinProc, (ClientData) 0);
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

      Tcl_CreateChannelHandler(rep->inChannel, 0,
                               &stdinProc, (ClientData) 0);
      int code = Tcl_RecordAndEval(interp, cmd, TCL_EVAL_GLOBAL);

      rep->inChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (rep->inChannel)
        {
          Tcl_CreateChannelHandler(rep->inChannel, TCL_READABLE,
                                   &stdinProc, (ClientData) 0);
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
      rep->prompt(gotPartial);
    }
  Tcl_ResetResult(interp);
}

//---------------------------------------------------------------------
//
// Tcl::Main::Main()
//
//---------------------------------------------------------------------

Tcl::Main::Main(int argc, char** argv)
{
  Tcl::MainImpl::create(argc, argv);
}

//---------------------------------------------------------------------
//
// Tcl::Main::~Main()
//
//---------------------------------------------------------------------

Tcl::Main::~Main()
{}

//---------------------------------------------------------------------
//
// Tcl::Main::interp()
//
//---------------------------------------------------------------------

Tcl_Interp* Tcl::Main::interp() const
{
  return Tcl::MainImpl::get()->interp();
}

//---------------------------------------------------------------------
//
// Tcl::Main::safeInterp()
//
//---------------------------------------------------------------------

Tcl::Interp& Tcl::Main::safeInterp() const
{
  return Tcl::MainImpl::get()->safeInterp;
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

  Tcl::MainImpl* rep = Tcl::MainImpl::get();

  /*
   * Invoke the script specified on the command line, if any.
   */

  if (rep->startupFileName != NULL)
    {
      rep->safeInterp.resetResult();
      int code = Tcl_EvalFile(rep->interp(), rep->startupFileName);
      if (code != TCL_OK)
        {
          // ensure errorInfo is set properly:
          Tcl_AddErrorInfo(rep->interp(), "");

          std::cerr << rep->safeInterp.getGlobalVar<const char*>("errorInfo")
                    << "\nError in startup script\n";
          Tcl_DeleteInterp(rep->interp());
          Tcl_Exit(1);
        }
      tsdPtr->tty = 0;
    }
  else
    {
      // Evaluate the .rc file, if one has been specified.
      Tcl_SourceRCFile(rep->interp());

      // Set up a stdin channel handler.
      rep->inChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (rep->inChannel)
        {
          Tcl_CreateChannelHandler(rep->inChannel, TCL_READABLE,
                                   &MainImpl::stdinProc, (ClientData) 0);
        }
      if (tsdPtr->tty)
        {
          rep->prompt(false);
        }
    }

  Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel)
    {
      Tcl_Flush(outChannel);
    }
  rep->safeInterp.resetResult();

  // Loop indefinitely, waiting for commands to execute, until there are no
  // main windows left, then exit.

  while (Tk_GetNumMainWindows() > 0)
    {
      Tcl_DoOneEvent(0);
    }
  Tcl_DeleteInterp(rep->interp());
  Tcl_Exit(0);
}

static const char vcid_tclmain_cc[] = "$Header$";
#endif // !TCLMAIN_CC_DEFINED
