///////////////////////////////////////////////////////////////////////
//
// tclmain.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:34:05 2002
// written: Thu Sep  5 20:04:25 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_CC_DEFINED
#define TCLMAIN_CC_DEFINED

#include "tcl/tclmain.h"

#include "tcl/tclsafeinterp.h"

#include "util/strings.h"

#include <iostream>
#include <tk.h>
#include <unistd.h>

#define USE_READLINE

#ifdef USE_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

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

  Tcl::Interp itsSafeInterp;
  const char* itsStartupFileName;
  Tcl_Channel itsInChannel;

  fstring itsCommand;         /* Used to assemble lines of terminal input
                               * into Tcl commands. */

  bool itsGotPartial;

  int isItInteractive;        /* Non-zero means standard input is a
                               * terminal-like device.  Zero means it's
                               * a file. */

  MainImpl(int argc, char** argv);

  void defaultPrompt(bool partial);
  void prompt(bool partial);

  void grabInput();

  void handleLine(const char* line, int count);

  void execCommand();

  static void stdinProc(ClientData /*clientData*/, int /*mask*/);

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

  Tcl_Interp* interp() const { return itsSafeInterp.intp(); }

  Tcl::Interp& safeInterp() { return itsSafeInterp; }

  void run();

#ifdef USE_READLINE
  static void readlineLineComplete(char* line);
#endif
};

Tcl::MainImpl* Tcl::MainImpl::theMainImpl = 0;

//---------------------------------------------------------------------
//
// Tcl::MainImpl::MainImpl()
//
//---------------------------------------------------------------------

Tcl::MainImpl::MainImpl(int argc, char** argv) :
  itsSafeInterp(Tcl_CreateInterp()),
  itsStartupFileName(0),
  itsInChannel(0),
  itsCommand(),
  itsGotPartial(false)
{
DOTRACE("Tcl::MainImpl::MainImpl");

  Tcl_FindExecutable(argv[0]);

  isItInteractive = isatty(0);

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

  itsSafeInterp.setGlobalVar("argc", argc-1);

  char* args = Tcl_Merge(argc-1, (const char **)argv+1);
  itsSafeInterp.setGlobalVar("argv", args);
  Tcl_Free(args);

  itsSafeInterp.setGlobalVar("argv0",
                             (itsStartupFileName == NULL)
                             ? argv[0]
                             : itsStartupFileName);

  itsSafeInterp.setGlobalVar("tcl_interactive",
                             ((itsStartupFileName == NULL) && isItInteractive)
                             ? 1 : 0);
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::defaultPrompt()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::defaultPrompt(bool partial)
{
DOTRACE("Tcl::MainImpl::defaultPrompt");
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
DOTRACE("Tcl::MainImpl::prompt");
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

#ifdef USE_READLINE
  rl_callback_handler_install("", readlineLineComplete);
#endif
}

//---------------------------------------------------------------------
//
// Callback triggered from the readline library when it has a full line of
// input for us to handle.
//
//---------------------------------------------------------------------

#ifdef USE_READLINE

void Tcl::MainImpl::readlineLineComplete(char* line)
{
DOTRACE("Tcl::MainImpl::readlineLineComplete");

  DebugEvalNL(line);

  rl_callback_handler_remove();

  if (line != 0 && *line != '\0')
    add_history(line);

  get()->handleLine(line, line == 0 ? -1 : int(strlen(line)));
}

#endif

//---------------------------------------------------------------------
//
// Pull any new characters in from the input stream. Returns the number of
// characters read from the input stream.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::grabInput()
{
DOTRACE("Tcl::MainImpl::grabInput");

#ifndef USE_READLINE
  Tcl_DString line;

  Tcl_DStringInit(&line);

  int count = Tcl_Gets(itsInChannel, &line);

  handleLine(Tcl_DStringValue(&line), count);

  Tcl_DStringFree(&line);

#else // USE_READLINE
  rl_callback_read_char();
#endif
}

//---------------------------------------------------------------------
//
// Handle a complete line of input (though not necessarily a complete Tcl
// command).
//
//---------------------------------------------------------------------

void Tcl::MainImpl::handleLine(const char* line, int count)
{
DOTRACE("Tcl::MainImpl::handleLine");

  if (count < 0)
    {
      if (!itsGotPartial)
        {
          if (isItInteractive)
            {
              Tcl_Exit(0);
            }
          else
            {
              Tcl_DeleteChannelHandler(itsInChannel,
                                       &stdinProc, (ClientData) 0);
            }
        }
      return;
    }

  Assert(line != 0);

  itsCommand.append(line, "\n");

  DebugEvalNL(itsCommand.c_str());

  if (itsCommand.length() > 0 &&
      Tcl_CommandComplete(itsCommand.c_str()))
    {
      itsGotPartial = false;
      execCommand();
    }
  else
    {
      itsGotPartial = true;
    }

  if (isItInteractive)
    {
      prompt(itsGotPartial);
    }

  Tcl_ResetResult(interp());
}

//---------------------------------------------------------------------
//
// Executes a complete command string in the Tcl interpreter.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::execCommand()
{
DOTRACE("Tcl::MainImpl::execCommand");

  /*
   * Disable the stdin channel handler while evaluating the command;
   * otherwise if the command re-enters the event loop we might
   * process commands from stdin before the current command is
   * finished.  Among other things, this will trash the text of the
   * command being evaluated.
   */

  Tcl_CreateChannelHandler(itsInChannel, 0, &stdinProc, (ClientData) 0);

  DebugEvalNL(itsCommand.c_str());

  int code = Tcl_RecordAndEval(interp(), itsCommand.c_str(), TCL_EVAL_GLOBAL);

  itsInChannel = Tcl_GetStdChannel(TCL_STDIN);

  if (itsInChannel)
    {
      Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                               &stdinProc, (ClientData) 0);
    }

  itsCommand = "";

  DebugEvalNL(Tcl_GetStringResult(interp()));

  if (Tcl_GetStringResult(interp())[0] != '\0')
    {
      if ((code != TCL_OK) || isItInteractive)
        {
          Tcl_Channel outChan = Tcl_GetStdChannel(TCL_STDOUT);
          if (outChan)
            {
              Tcl_WriteObj(outChan, Tcl_GetObjResult(interp()));
              Tcl_WriteChars(outChan, "\n", 1);
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
DOTRACE("Tcl::MainImpl::stdinProc");

  Tcl::MainImpl::get()->grabInput();
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::run()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::run()
{
DOTRACE("Tcl::MainImpl::run");

  /*
   * Invoke the script specified on the command line, if any.
   */

  if (itsStartupFileName != NULL)
    {
      itsSafeInterp.resetResult();
      int code = Tcl_EvalFile(this->interp(), itsStartupFileName);
      if (code != TCL_OK)
        {
          // ensure errorInfo is set properly:
          Tcl_AddErrorInfo(this->interp(), "");

          std::cerr << itsSafeInterp.getGlobalVar<const char*>("errorInfo")
                    << "\nError in startup script\n";
          Tcl_DeleteInterp(this->interp());
          Tcl_Exit(1);
        }
      isItInteractive = 0;
    }
  else
    {
      // Evaluate the .rc file, if one has been specified.
      Tcl_SourceRCFile(this->interp());

      // Set up a stdin channel handler.
      itsInChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (itsInChannel)
        {
          Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                                   &MainImpl::stdinProc, (ClientData) 0);
        }
      if (isItInteractive)
        {
          this->prompt(false);
        }
    }

  Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel)
    {
      Tcl_Flush(outChannel);
    }
  itsSafeInterp.resetResult();

  // Loop indefinitely, waiting for commands to execute, until there are no
  // main windows left, then exit.

  while (Tk_GetNumMainWindows() > 0)
    {
      Tcl_DoOneEvent(0);
    }
  Tcl_DeleteInterp(this->interp());
  Tcl_Exit(0);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Main functions delegate to Tcl::MainImpl
//
///////////////////////////////////////////////////////////////////////

Tcl::Main::Main(int argc, char** argv)
{
  Tcl::MainImpl::create(argc, argv);
}

Tcl::Main::~Main()
{}

Tcl_Interp* Tcl::Main::interp() const
{
  return Tcl::MainImpl::get()->interp();
}

Tcl::Interp& Tcl::Main::safeInterp() const
{
  return Tcl::MainImpl::get()->safeInterp();
}

void Tcl::Main::run()
{
  Tcl::MainImpl::get()->run();
}

static const char vcid_tclmain_cc[] = "$Header$";
#endif // !TCLMAIN_CC_DEFINED
