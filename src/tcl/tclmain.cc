///////////////////////////////////////////////////////////////////////
//
// tclmain.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jul 22 16:34:05 2002
// written: Wed Sep 11 14:24:46 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLMAIN_CC_DEFINED
#define TCLMAIN_CC_DEFINED

#include "tcl/tclmain.h"

#include "tcl/tclsafeinterp.h"

#include <util/error.h>
#include "util/strings.h"

#include <iostream>
#include <tk.h>
#include <unistd.h>

#ifdef WITH_READLINE
#  include <cstdlib> // for malloc/free
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
  const char* itsArgv0;
  Tcl_Channel itsInChannel;
  fstring itsCommand;   // Build lines of terminal input into Tcl commands.
  bool itsGotPartial;
  bool isItInteractive; // True if input is a terminal-like device.

  MainImpl(int argc, char** argv);

  int historyNext();

  void doPrompt(const char* text, unsigned int length);

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
    if (theMainImpl == 0)
      {
        throw Util::Error("no Tcl::Main object has yet been created");
      }

    return theMainImpl;
  }

  Tcl_Interp* interp() const { return itsSafeInterp.intp(); }

  Tcl::Interp& safeInterp() { return itsSafeInterp; }

  void run();

#ifdef WITH_READLINE
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
  itsArgv0(0),
  itsInChannel(0),
  itsCommand(),
  itsGotPartial(false),
  isItInteractive(isatty(0))
{
DOTRACE("Tcl::MainImpl::MainImpl");

  Tcl_FindExecutable(argv[0]);

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

  itsArgv0 =
    (itsStartupFileName == NULL)
    ? argv[0]
    : itsStartupFileName;

  itsSafeInterp.setGlobalVar("argv0", itsArgv0);

  itsSafeInterp.setGlobalVar("tcl_interactive",
                             ((itsStartupFileName == NULL) && isItInteractive)
                             ? 1 : 0);

#ifdef WITH_READLINE
  using_history();
#endif
}

//---------------------------------------------------------------------
//
// Get the next number in the history count.
//
//---------------------------------------------------------------------

int Tcl::MainImpl::historyNext()
{
DOTRACE("Tcl::MainImpl::historyNext");

#ifdef WITH_READLINE
  return history_length+1;
#else
  Tcl::ObjPtr obj = itsSafeInterp.getResult<Tcl_Obj*>();

  Tcl_GlobalEval(interp(), "history nextid");

  int result = itsSafeInterp.getResult<int>();

  itsSafeInterp.setResult(obj);

  return result;
#endif
}

//---------------------------------------------------------------------
//
// Actually write the prompt characters to the terminal.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::doPrompt(const char* text,
#ifdef WITH_READLINE
                             unsigned int /*length*/
#else
                             unsigned int length
#endif
                             )
{
DOTRACE("Tcl::MainImpl::doPrompt");

#ifdef WITH_READLINE
  rl_callback_handler_install(text, readlineLineComplete);
#else
  if (length > 0)
    {
      // Check that outChannel is a real channel - it is possible that
      // someone has transferred stdout out of this interpreter with
      // "interp transfer".

      Tcl_Channel outChannel = Tcl_GetChannel(interp(), "stdout", NULL);
      Tcl_WriteChars(outChannel, text, length);
      Tcl_Flush(outChannel);
    }
#endif
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::prompt()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::prompt(bool partial)
{
DOTRACE("Tcl::MainImpl::prompt");

  if (partial)
    {
      doPrompt("", 0);
    }
  else
    {
#ifdef WITH_READLINE
      fstring text(itsArgv0, " ", historyNext(), ">>> ");
#else
      fstring text(itsArgv0, " ", historyNext(), "> ");
#endif

      doPrompt(text.c_str(), text.length());
    }
}

//---------------------------------------------------------------------
//
// Callback triggered from the readline library when it has a full line of
// input for us to handle.
//
//---------------------------------------------------------------------

#ifdef WITH_READLINE

void Tcl::MainImpl::readlineLineComplete(char* line)
{
DOTRACE("Tcl::MainImpl::readlineLineComplete");

  DebugEvalNL(line);

  rl_callback_handler_remove();

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

#ifndef WITH_READLINE
  Tcl_DString line;

  Tcl_DStringInit(&line);

  int count = Tcl_Gets(itsInChannel, &line);

  handleLine(Tcl_DStringValue(&line), count);

  Tcl_DStringFree(&line);

#else // WITH_READLINE
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

  // Disable the stdin channel handler while evaluating the command;
  // otherwise if the command re-enters the event loop we might process
  // commands from stdin before the current command is finished.  Among
  // other things, this will trash the text of the command being evaluated.

  Tcl_CreateChannelHandler(itsInChannel, 0, &stdinProc, (ClientData) 0);

  bool display_result = false;

#ifdef WITH_READLINE
  char* expansion = 0;
  const int status = history_expand(const_cast<char*>(itsCommand.c_str()),
                                    &expansion);
#else
  char* expansion = itsCommand.data();
  const int status = 0;
#endif

  DebugEvalNL(itsCommand.c_str());
  DebugEvalNL(expansion);
  DebugEvalNL(status);

  // status: -1 --> error
  //          0 --> no expansions occurred
  //          1 --> expansions occurred
  //          2 --> display but don't execute

  if (status == -1 || status == 2) // display expansion?
    {
      Tcl_AppendResult(interp(), expansion, (char*) 0);
      display_result = true;
    }

  if (status == 1)
    {
      Tcl_Channel outChan = Tcl_GetStdChannel(TCL_STDOUT);
      if (outChan)
        {
          Tcl_WriteChars(outChan, expansion, -1);
          Tcl_Flush(outChan);
        }
    }

  if (status == 0 || status == 1) // execute expansion?
    {
#ifdef WITH_READLINE
      int code = Tcl_GlobalEval(interp(), expansion);

      size_t len = strlen(expansion);
      char c = expansion[len-1];

      if (c == '\n')
        expansion[len-1] = '\0';

      add_history(expansion);

      expansion[len-1] = c;
#else
      int code = Tcl_RecordAndEval(interp(), expansion, TCL_EVAL_GLOBAL);
#endif

      DebugEvalNL(Tcl_GetStringResult(interp()));

      display_result =
        (Tcl_GetStringResult(interp())[0] != '\0') &&
        ((code != TCL_OK) || isItInteractive);
    }

  if (display_result)
    {
      Tcl_Channel outChan = Tcl_GetStdChannel(TCL_STDOUT);
      if (outChan)
        {
          Tcl_WriteObj(outChan, Tcl_GetObjResult(interp()));
          Tcl_WriteChars(outChan, "\n", 1);
        }
    }

  itsInChannel = Tcl_GetStdChannel(TCL_STDIN);

  if (itsInChannel)
    {
      Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                               &stdinProc, (ClientData) 0);
    }

  itsCommand = "";

#ifdef WITH_READLINE
  free(expansion);
#endif
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
      isItInteractive = false;
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

Tcl_Interp* Tcl::Main::interp()
{
  return Tcl::MainImpl::get()->interp();
}

Tcl::Interp& Tcl::Main::safeInterp()
{
  return Tcl::MainImpl::get()->safeInterp();
}

void Tcl::Main::run()
{
  Tcl::MainImpl::get()->run();
}

static const char vcid_tclmain_cc[] = "$Header$";
#endif // !TCLMAIN_CC_DEFINED
