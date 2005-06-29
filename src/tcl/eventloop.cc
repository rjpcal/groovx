///////////////////////////////////////////////////////////////////////
//
// tclmain.cc
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jul 22 16:34:05 2002
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_TCL_EVENTLOOP_CC_UTC20050628162420_DEFINED
#define GROOVX_TCL_EVENTLOOP_CC_UTC20050628162420_DEFINED

#include "tcl/eventloop.h"

#include "tcl/interp.h"

#include "rutz/backtrace.h"
#include "rutz/error.h"
#include "rutz/fstring.h"

#include <iostream>
#include <tk.h>
#include <unistd.h>

#ifndef GVX_NO_READLINE
#define GVX_WITH_READLINE
#endif

#ifdef GVX_WITH_READLINE
#  include <cstdlib> // for malloc/free
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace Tcl
{
  class MainImpl;
}

namespace
{
  void exitHandler(ClientData /*clientData*/)
  {
#ifdef GVX_WITH_READLINE
    rl_callback_handler_remove();
#endif
  }
}

// Singleton implementation class for Tcl::Main
class Tcl::MainImpl
{
private:
  static MainImpl* theMainImpl;

  // Data members

  int itsArgc;
  const char** itsArgv;
  Tcl::Interp itsSafeInterp;
  const char* itsStartupFileName;
  const char* itsArgv0;
  Tcl_Channel itsInChannel;
  rutz::fstring itsCommand; // Build lines of tty input into Tcl commands.
  bool itsGotPartial;
  bool isItInteractive; // True if input is a terminal-like device.
  rutz::fstring itsCommandLine; // Entire command-line as a string
  bool itsNoWindow; // whether this is a windowless environment

  // Function members

  MainImpl(int argc, char** argv, bool nowindow);

  int historyNext();

  void doPrompt(const char* text, unsigned int length);

  enum PromptType { FULL, PARTIAL };

  void prompt(PromptType t);

  void grabInput();

  void handleLine(const char* line, int count);

  void execCommand();

  static void stdinProc(ClientData /*clientData*/, int /*mask*/);

public:
  static void create(int argc, char** argv, bool nowindow)
  {
    GVX_ASSERT(theMainImpl == 0);

    theMainImpl = new MainImpl(argc, argv, nowindow);
    Tcl_CreateExitHandler(exitHandler, static_cast<ClientData>(0));
  }

  static MainImpl* get()
  {
    if (theMainImpl == 0)
      {
        throw rutz::error("no Tcl::Main object has yet been created",
                          SRC_POS);
      }

    return theMainImpl;
  }

  bool isInteractive() const { return isItInteractive; }

  Tcl::Interp& interp() { return itsSafeInterp; }

  void run();

  int argc() const { return itsArgc; }

  const char* const* argv() const { return itsArgv; }

  rutz::fstring commandLine() const { return itsCommandLine; }

#ifdef GVX_WITH_READLINE
  static void readlineLineComplete(char* line);
#endif
};

Tcl::MainImpl* Tcl::MainImpl::theMainImpl = 0;

//---------------------------------------------------------------------
//
// Tcl::MainImpl::MainImpl()
//
//---------------------------------------------------------------------

Tcl::MainImpl::MainImpl(int argc, char** argv, bool nowindow) :
  itsArgc(argc),
  itsArgv(const_cast<const char**>(argv)),
  itsSafeInterp(Tcl_CreateInterp()),
  itsStartupFileName(0),
  itsArgv0(0),
  itsInChannel(0),
  itsCommand(),
  itsGotPartial(false),
  isItInteractive(isatty(0)),
  itsCommandLine(),
  itsNoWindow(nowindow)
{
GVX_TRACE("Tcl::MainImpl::MainImpl");

  Tcl_FindExecutable(argv[0]);

  itsCommandLine.append(argv[0]);
  for (int i = 1; i < argc; ++i)
    {
      itsCommandLine.append(" ", argv[i]);
    }

  // Parse command-line arguments.  If the next argument doesn't start
  // with a "-" then strip it off and use it as the name of a script
  // file to process.

  if ((argc > 1) && (argv[1][0] != '-'))
    {
      itsArgv0 = itsStartupFileName = argv[1];
      --argc;
      ++argv;
      isItInteractive = false;
    }
  else
    {
      itsArgv0 = argv[0];
    }

  // Make command-line arguments available in the Tcl variables "argc"
  // and "argv".

  itsSafeInterp.setGlobalVar("argc", Tcl::toTcl(argc-1));

  char* args = Tcl_Merge(argc-1,
                         const_cast<const char**>(argv+1));
  itsSafeInterp.setGlobalVar("argv", Tcl::toTcl(args));
  Tcl_Free(args);

  itsSafeInterp.setGlobalVar("argv0", Tcl::toTcl(itsArgv0));

  itsSafeInterp.setGlobalVar("tcl_interactive",
                             Tcl::toTcl(isItInteractive ? 1 : 0));

#ifdef GVX_WITH_READLINE
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
GVX_TRACE("Tcl::MainImpl::historyNext");

#ifdef GVX_WITH_READLINE
  return history_length+1;
#else
  Tcl::Obj obj = itsSafeInterp.getResult<Tcl_Obj*>();

  itsSafeInterp.eval("history nextid", Tcl::IGNORE_ERROR);

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
#ifdef GVX_WITH_READLINE
                             unsigned int /*length*/
#else
                             unsigned int length
#endif
                             )
{
GVX_TRACE("Tcl::MainImpl::doPrompt");

#ifdef GVX_WITH_READLINE
  rl_callback_handler_install(text, readlineLineComplete);
#else
  if (length > 0)
    {
      std::cout.write(text, length);
      std::cout.flush();
    }
#endif
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::prompt()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::prompt(Tcl::MainImpl::PromptType t)
{
GVX_TRACE("Tcl::MainImpl::prompt");

  if (t == PARTIAL)
    {
      doPrompt("", 0);
    }
  else
    {
#ifdef GVX_WITH_READLINE
      rutz::fstring text(itsArgv0, " ", historyNext(), ">>> ");
#else
      rutz::fstring text(itsArgv0, " ", historyNext(), "> ");
#endif

      doPrompt(text.c_str(), text.length());
    }
}

//---------------------------------------------------------------------
//
// Callback triggered from the readline library when it has a full
// line of input for us to handle.
//
//---------------------------------------------------------------------

#ifdef GVX_WITH_READLINE

void Tcl::MainImpl::readlineLineComplete(char* line)
{
GVX_TRACE("Tcl::MainImpl::readlineLineComplete");

  dbg_eval_nl(3, line);

  rl_callback_handler_remove();

  get()->handleLine(line, line == 0 ? -1 : int(strlen(line)));
}

#endif

//---------------------------------------------------------------------
//
// Pull any new characters in from the input stream. Returns the
// number of characters read from the input stream.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::grabInput()
{
GVX_TRACE("Tcl::MainImpl::grabInput");

#ifndef GVX_WITH_READLINE
  Tcl_DString line;

  Tcl_DStringInit(&line);

  int count = Tcl_Gets(itsInChannel, &line);

  handleLine(Tcl_DStringValue(&line), count);

  Tcl_DStringFree(&line);

#else // GVX_WITH_READLINE
  rl_callback_read_char();
#endif
}

//---------------------------------------------------------------------
//
// Handle a complete line of input (though not necessarily a complete
// Tcl command).
//
//---------------------------------------------------------------------

void Tcl::MainImpl::handleLine(const char* line, int count)
{
GVX_TRACE("Tcl::MainImpl::handleLine");

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
                                       &stdinProc,
                                       static_cast<ClientData>(0));
            }
        }
      return;
    }

  GVX_ASSERT(line != 0);

  itsCommand.append(line, "\n");

  dbg_eval_nl(3, itsCommand.c_str());

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
      prompt(itsGotPartial ? PARTIAL : FULL);
    }

  itsSafeInterp.resetResult();
}

//---------------------------------------------------------------------
//
// Executes a complete command string in the Tcl interpreter.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::execCommand()
{
GVX_TRACE("Tcl::MainImpl::execCommand");

  // Disable the stdin channel handler while evaluating the command;
  // otherwise if the command re-enters the event loop we might
  // process commands from stdin before the current command is
  // finished.  Among other things, this will trash the text of the
  // command being evaluated.

  Tcl_CreateChannelHandler(itsInChannel, 0, &stdinProc,
                           static_cast<ClientData>(0));

  bool should_display_result = false;

#ifdef GVX_WITH_READLINE
  char* expansion = 0;
  const int status =
    history_expand(const_cast<char*>(itsCommand.c_str()), &expansion);
#else
  char* expansion = itsCommand.data();
  const int status = 0;
#endif

  dbg_eval_nl(3, itsCommand.c_str());
  dbg_eval_nl(3, expansion);
  dbg_eval_nl(3, status);

  // status: -1 --> error
  //          0 --> no expansions occurred
  //          1 --> expansions occurred
  //          2 --> display but don't execute

  if (status == -1 || status == 2) // display expansion?
    {
      itsSafeInterp.appendResult(expansion);
      should_display_result = true;
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
      // The idea here is that we want to keep the readline history
      // and the Tcl history in sync. Tcl's "history add" command will
      // skip adding the string if it is empty or has whitespace
      // only. So, we need to make that same check here before adding
      // to the readline history. In fact, if we find that the command
      // is empty, we can just skip executing it altogether.

      // Skip over leading whitespace
      char* trimmed = expansion;

      while (isspace(trimmed[0]) && trimmed[0] != '\0')
        {
          ++trimmed;
        }

      size_t len = strlen(trimmed);

      if (len > 0)
        {
          int code = Tcl_RecordAndEval(itsSafeInterp.intp(),
                                       trimmed, TCL_EVAL_GLOBAL);

#ifdef GVX_WITH_READLINE
          char c = trimmed[len-1];

          if (c == '\n')
            trimmed[len-1] = '\0';

          add_history(trimmed);

          trimmed[len-1] = c;
#endif

          dbg_eval_nl(3, itsSafeInterp.getResult<const char*>());

          should_display_result =
            ((itsSafeInterp.getResult<const char*>())[0] != '\0') &&
            ((code != TCL_OK) || isItInteractive);
        }
    }

  if (should_display_result)
    {
      Tcl_Channel outChan = Tcl_GetStdChannel(TCL_STDOUT);
      if (outChan)
        {
          Tcl_WriteObj(outChan, itsSafeInterp.getResult<Tcl_Obj*>());
          Tcl_WriteChars(outChan, "\n", 1);
        }
    }

  itsInChannel = Tcl_GetStdChannel(TCL_STDIN);

  if (itsInChannel)
    {
      Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                               &stdinProc,
                               static_cast<ClientData>(0));
    }

  itsCommand.clear();

#ifdef GVX_WITH_READLINE
  free(expansion);
#endif
}

//---------------------------------------------------------------------
//
// This procedure is invoked by the event dispatcher whenever standard
// input becomes readable.  It grabs the next line of input
// characters, adds them to a command being assembled, and executes
// the command if it's complete.
//
//---------------------------------------------------------------------

void Tcl::MainImpl::stdinProc(ClientData /*clientData*/, int /*mask*/)
{
GVX_TRACE("Tcl::MainImpl::stdinProc");

  Tcl::MainImpl::get()->grabInput();
}

//---------------------------------------------------------------------
//
// Tcl::MainImpl::run()
//
//---------------------------------------------------------------------

void Tcl::MainImpl::run()
{
GVX_TRACE("Tcl::MainImpl::run");

  /*
   * Invoke the script specified on the command line, if any.
   */

  if (itsStartupFileName != NULL)
    {
      itsSafeInterp.resetResult();
      bool success = itsSafeInterp.evalFile(itsStartupFileName);
      if (!success)
        {
          // ensure errorInfo is set properly:
          itsSafeInterp.addErrorInfo("");

          const rutz::fstring bt = rutz::error::last_backtrace().format();

          std::cerr << itsSafeInterp.getGlobalVar<const char*>("errorInfo")
                    << "\n" << bt << "\nError in startup script\n";
          itsSafeInterp.destroy();
          Tcl_Exit(1);
        }
    }
  else
    {
      // Evaluate the .rc file, if one has been specified.
      itsSafeInterp.sourceRCFile();

      // Set up a stdin channel handler.
      itsInChannel = Tcl_GetStdChannel(TCL_STDIN);
      if (itsInChannel)
        {
          Tcl_CreateChannelHandler(itsInChannel, TCL_READABLE,
                                   &MainImpl::stdinProc,
                                   static_cast<ClientData>(0));
        }
      if (isItInteractive)
        {
          this->prompt(FULL);
        }
    }

  Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel)
    {
      Tcl_Flush(outChannel);
    }
  itsSafeInterp.resetResult();

  // Loop indefinitely, waiting for commands to execute, until there
  // are no main windows left, then exit.

  while ((isItInteractive && itsNoWindow)
         || Tk_GetNumMainWindows() > 0)
    {
      Tcl_DoOneEvent(0);
    }
  itsSafeInterp.destroy();
  Tcl_Exit(0);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Main functions delegate to Tcl::MainImpl
//
///////////////////////////////////////////////////////////////////////

Tcl::Main::Main(int argc, char** argv, bool nowindow)
{
  Tcl::MainImpl::create(argc, argv, nowindow);
}

Tcl::Main::~Main()
{}

bool Tcl::Main::isInteractive()
{
GVX_TRACE("Tcl::Main::isInteractive");
  return Tcl::MainImpl::get()->isInteractive();
}

Tcl::Interp& Tcl::Main::interp()
{
GVX_TRACE("Tcl::Main::interp");
  return Tcl::MainImpl::get()->interp();
}

void Tcl::Main::run()
{
GVX_TRACE("Tcl::Main::run");
  Tcl::MainImpl::get()->run();
}

int Tcl::Main::argc()
{
GVX_TRACE("Tcl::Main::argc");
  return Tcl::MainImpl::get()->argc();
}

const char* const* Tcl::Main::argv()
{
GVX_TRACE("Tcl::Main::argv");
  return Tcl::MainImpl::get()->argv();
}

rutz::fstring Tcl::Main::commandLine()
{
GVX_TRACE("Tcl::Main::commandLine");
  return Tcl::MainImpl::get()->commandLine();
}

static const char vcid_groovx_tcl_eventloop_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_EVENTLOOP_CC_UTC20050628162420_DEFINED
