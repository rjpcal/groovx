///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  2 08:00:00 1998
// written: Mon Jul 22 15:14:37 2002
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
///////////////////////////////////////////////////////////////////////

#include "visx/grshapp.h"

#include "system/demangle.h"

#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/strings.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <tk.h>
#include <typeinfo>
#include <unistd.h>

#include "util/trace.h"

//
// Forward declarations of package init procedures
//

extern "C"
{
  Tcl_PackageInitProc Bitmap_Init;
  Tcl_PackageInitProc Block_Init;
  Tcl_PackageInitProc Dlist_Init;
  Tcl_PackageInitProc Expt_Init;
  Tcl_PackageInitProc Face_Init;
  Tcl_PackageInitProc Fish_Init;
  Tcl_PackageInitProc Fixpt_Init;
  Tcl_PackageInitProc Gabor_Init;
  Tcl_PackageInitProc Gltcl_Init;
  Tcl_PackageInitProc Grobj_Init;
  Tcl_PackageInitProc Gtext_Init;
  Tcl_PackageInitProc Gx_Init;
  Tcl_PackageInitProc Gtrace_Init;
  Tcl_PackageInitProc Hook_Init;
  Tcl_PackageInitProc House_Init;
  Tcl_PackageInitProc Io_Init;
  Tcl_PackageInitProc Jitter_Init;
  Tcl_PackageInitProc Mask_Init;
  Tcl_PackageInitProc Misc_Init;
  Tcl_PackageInitProc Morphyface_Init;
  Tcl_PackageInitProc Obj_Init;
  Tcl_PackageInitProc Objtogl_Init;
  Tcl_PackageInitProc Position_Init;
  Tcl_PackageInitProc Rh_Init;
  Tcl_PackageInitProc Sound_Init;
  Tcl_PackageInitProc Subject_Init;
  Tcl_PackageInitProc Th_Init;
  Tcl_PackageInitProc Tlist_Init;
  Tcl_PackageInitProc Togl_Init;
  Tcl_PackageInitProc Trial_Init;
}

//
// Info about the packages to be loaded
//

namespace
{

struct PackageInfo
{
  const char* pkgName;
  Tcl_PackageInitProc *pkgInitProc;
};

PackageInfo IMMEDIATE_PKGS[] =
{
  { "Tcl",      Tcl_Init       },
  { "Tk",       Tk_Init        },
  { "Togl",     Togl_Init      },
  { "Bitmap",   Bitmap_Init    },
  { "Block",    Block_Init     },
  { "Dlist",    Dlist_Init     },
  { "Expt",     Expt_Init      },
  { "Face",     Face_Init      },
  { "Fish",     Fish_Init      },
  { "Fixpt",    Fixpt_Init     },
  { "Gabor",    Gabor_Init     },
  { "Gltcl",    Gltcl_Init     },
  { "Grobj",    Grobj_Init     },
  { "Gtext",    Gtext_Init     },
  { "Gtrace",   Gtrace_Init    },
  { "Gx",       Gx_Init        },
  { "Hook",     Hook_Init      },
  { "House",    House_Init     },
  { "Io",       Io_Init        },
  { "Jitter",   Jitter_Init    },
  { "Mask",     Mask_Init      },
  { "Misc",     Misc_Init      },
  { "Morphyface",Morphyface_Init},
  { "Obj",      Obj_Init       },
  { "Objtogl",  Objtogl_Init   },
  { "Position", Position_Init  },
  { "Rh",       Rh_Init        },
  { "Sound",    Sound_Init     },
  { "Subject",  Subject_Init   },
  { "Th",       Th_Init        },
  { "Tlist",    Tlist_Init     },
  { "Trial",    Trial_Init     },
};

#if 0
PackageInfo DELAYED_PKGS[] = {};
#endif

} // end anonymous namespace


///////////////////////////////////////////////////////////////////////
//
// TclApp class definition
//
///////////////////////////////////////////////////////////////////////

class TclApp : public GrshApp
{
public:
  TclApp(int argc, char** argv, Tcl::Interp& interp);

  int status() const { return itsStatus; }
private:
  int itsStatus;
};

TclApp::TclApp(int argc, char** argv, Tcl::Interp& interp) :
  GrshApp(argc, argv, interp.intp()),
  itsStatus(TCL_OK)
{
DOTRACE("TclApp::TclApp(Tcl_Interp*)");

  {for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
    {
#ifdef LOCAL_TRACE
      std::cerr << "initializing " << IMMEDIATE_PKGS[i].pkgName << '\n';
#endif
      int result = IMMEDIATE_PKGS[i].pkgInitProc(interp.intp());
      if (result != TCL_OK)
        {
          itsStatus = result;
          std::cerr << "initialization failed: "
                    << IMMEDIATE_PKGS[i].pkgName << '\n';
          fstring msg = interp.getResult<const char*>();
          if ( !msg.is_empty() )
            std::cerr << '\t' << msg << '\n';
          interp.resetResult();
        }
    }
  }

  // set prompt to "cmd[n]% " where cmd is the name of the program,
  // and n is the history event number
  interp.setGlobalVar("tcl_prompt1",
                      "puts -nonewline \"([history nextid]) $argv0> \"");

  // specifies a file to be 'source'd upon startup
  interp.setGlobalVar("tcl_rcFileName", "./grsh_startup.tcl");
}


///////////////////////////////////////////////////////////////////////
//
// main
//
///////////////////////////////////////////////////////////////////////

namespace
{
  // Just an ugly hack to communicate argc+argv from main() into
  // Grsh_AppInit()
  int LOCAL_ARGC = 0;
  char** LOCAL_ARGV = 0;
}

// procedure to initialize a TclApp
extern "C"
int Grsh_AppInit(Tcl_Interp* interp)
{
DOTRACE("Grsh_AppInit");
  try
    {
      Tcl::Interp safeIntp(interp);
      static TclApp theApp(LOCAL_ARGC, LOCAL_ARGV, safeIntp);
      return theApp.status();
    }
  catch (Util::Error& err)
    {
      std::cerr << "uncaught Error: " << err.msg_cstr() << '\n';
    }
  catch (std::exception& err)
    {
      std::cerr << "uncaught std::exception of type "
                << demangle_cstr(typeid(err).name())
                << " occurred: "
                << err.what()
                << '\n';
    }
  catch (...)
    {
      std::cerr << "uncaught exception of unknown type" << '\n';
    }

  exit(-1);

  /* can't get here, but placate compiler: */ return TCL_ERROR;
}

namespace my
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

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *      Issue a prompt on standard output, or invoke a script
 *      to issue the prompt.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      A prompt gets output, and a Tcl script may be evaluated
 *      in interp.
 *
 *----------------------------------------------------------------------
 */

void Prompt(Tcl_Interp* interp, int partial)
{
  Tcl_Obj *promptCmd;
  int code;
  Tcl_Channel outChannel, errChannel;

  promptCmd = Tcl_GetVar2Ex(interp,
                            const_cast<char*>
                              (partial ? "tcl_prompt2" : "tcl_prompt1"),
                            NULL, TCL_GLOBAL_ONLY);
  if (promptCmd == NULL) {
  defaultPrompt:
    if (!partial) {

      /*
       * We must check that outChannel is a real channel - it
       * is possible that someone has transferred stdout out of
       * this interpreter with "interp transfer".
       */

      outChannel = Tcl_GetChannel(interp, "stdout", NULL);
      if (outChannel != (Tcl_Channel) NULL) {
        Tcl_WriteChars(outChannel, "% ", 2);
      }
    }
  } else {
    code = Tcl_EvalObjEx(interp, promptCmd, TCL_EVAL_GLOBAL);
    if (code != TCL_OK) {
      Tcl_AddErrorInfo(interp,
                       "\n    (script that generates prompt)");
      /*
       * We must check that errChannel is a real channel - it
       * is possible that someone has transferred stderr out of
       * this interpreter with "interp transfer".
       */

      errChannel = Tcl_GetChannel(interp, "stderr", NULL);
      if (errChannel != (Tcl_Channel) NULL) {
        Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
        Tcl_WriteChars(errChannel, "\n", 1);
      }
      goto defaultPrompt;
    }
  }
  outChannel = Tcl_GetChannel(interp, "stdout", NULL);
  if (outChannel != (Tcl_Channel) NULL) {
    Tcl_Flush(outChannel);
  }
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *      This procedure is invoked by the event dispatcher whenever
 *      standard input becomes readable.  It grabs the next line of
 *      input characters, adds them to a command being assembled, and
 *      executes the command if it's complete.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Could be almost arbitrary, depending on the command that's
 *      typed.
 *
 *----------------------------------------------------------------------
 */

void StdinProc(ClientData clientData, int /*mask*/)
{
  static int gotPartial = 0;
  char *cmd;
  int code, count;
  Tcl_Channel chan = (Tcl_Channel) clientData;
  ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
    Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));
  Tcl_Interp *interp = tsdPtr->interp;

  count = Tcl_Gets(chan, &tsdPtr->line);

  if (count < 0) {
    if (!gotPartial) {
      if (tsdPtr->tty) {
        Tcl_Exit(0);
      } else {
        Tcl_DeleteChannelHandler(chan, StdinProc, (ClientData) chan);
      }
      return;
    }
  }

  (void) Tcl_DStringAppend(&tsdPtr->command, Tcl_DStringValue(
                                                              &tsdPtr->line), -1);
  cmd = Tcl_DStringAppend(&tsdPtr->command, "\n", -1);
  Tcl_DStringFree(&tsdPtr->line);
  if (!Tcl_CommandComplete(cmd)) {
    gotPartial = 1;
    goto prompt;
  }
  gotPartial = 0;

  /*
   * Disable the stdin channel handler while evaluating the command;
   * otherwise if the command re-enters the event loop we might
   * process commands from stdin before the current command is
   * finished.  Among other things, this will trash the text of the
   * command being evaluated.
   */

  Tcl_CreateChannelHandler(chan, 0, StdinProc, (ClientData) chan);
  code = Tcl_RecordAndEval(interp, cmd, TCL_EVAL_GLOBAL);

  chan = Tcl_GetStdChannel(TCL_STDIN);
  if (chan) {
    Tcl_CreateChannelHandler(chan, TCL_READABLE, StdinProc,
                             (ClientData) chan);
  }
  Tcl_DStringFree(&tsdPtr->command);
  if (Tcl_GetStringResult(interp)[0] != '\0') {
    if ((code != TCL_OK) || (tsdPtr->tty)) {
      chan = Tcl_GetStdChannel(TCL_STDOUT);
      if (chan) {
        Tcl_WriteObj(chan, Tcl_GetObjResult(interp));
        Tcl_WriteChars(chan, "\n", 1);
      }
    }
  }

  /*
   * Output a prompt.
   */

 prompt:
  if (tsdPtr->tty) {
    Prompt(interp, gotPartial);
  }
  Tcl_ResetResult(interp);
}

}

int main(int argc, char** argv)
{
  LOCAL_ARGC = argc;
  LOCAL_ARGV = argv;

  Tcl_AppInitProc* appInitProc = Grsh_AppInit;
  Tcl_Interp* interp = Tcl_CreateInterp();

  my::ThreadSpecificData* tsdPtr = (my::ThreadSpecificData *)
    Tcl_GetThreadData(&my::dataKey, sizeof(my::ThreadSpecificData));

  Tcl_FindExecutable(argv[0]);
  tsdPtr->interp = interp;

  /*
   * Parse command-line arguments.  If the next argument doesn't start with
   * a "-" then strip it off and use it as the name of a script file to
   * process.
   */

  const char* fileName = 0;

  if ((argc > 1) && (argv[1][0] != '-')) {
    fileName = argv[1];
    argc--;
    argv++;
  }

  /*
   * Make command-line arguments available in the Tcl variables "argc"
   * and "argv".
   */

  char* args = Tcl_Merge(argc-1, (const char **)argv+1);
  Tcl_DString argString;
  Tcl_ExternalToUtfDString(NULL, args, -1, &argString);
  Tcl_SetVar(interp, const_cast<char*>("argv"),
             Tcl_DStringValue(&argString), TCL_GLOBAL_ONLY);
  Tcl_DStringFree(&argString);
  ckfree(args);

  char buf[TCL_INTEGER_SPACE];
  sprintf(buf, "%d", argc-1);

  if (fileName == NULL) {
    Tcl_ExternalToUtfDString(NULL, argv[0], -1, &argString);
  } else {
    fileName = Tcl_ExternalToUtfDString(NULL, fileName, -1, &argString);
  }
  Tcl_SetVar(interp, const_cast<char*>("argc"), buf, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, const_cast<char*>("argv0"), Tcl_DStringValue(&argString), TCL_GLOBAL_ONLY);

  /*
   * Set the "tcl_interactive" variable.
   */

  tsdPtr->tty = isatty(0);

  Tcl_SetVar(interp, const_cast<char*>("tcl_interactive"),
             ((fileName == NULL) && tsdPtr->tty) ? "1" : "0", TCL_GLOBAL_ONLY);

  /*
   * Invoke application-specific initialization.
   */

  if ((*appInitProc)(interp) != TCL_OK) {
    std::cerr << Tcl_GetStringResult(interp)
              << "\nApplication initialization failed\n";
  }

  /*
   * Invoke the script specified on the command line, if any.
   */

  if (fileName != NULL) {
    Tcl_ResetResult(interp);
    int code = Tcl_EvalFile(interp, fileName);
    if (code != TCL_OK) {
      /*
       * The following statement guarantees that the errorInfo
       * variable is set properly.
       */

      Tcl_AddErrorInfo(interp, "");
      std::cerr << Tcl_GetVar(interp, const_cast<char*>("errorInfo"),
                              TCL_GLOBAL_ONLY)
                << "\nError in startup script\n";
      Tcl_DeleteInterp(interp);
      Tcl_Exit(1);
    }
    tsdPtr->tty = 0;
  } else {

    /*
     * Evaluate the .rc file, if one has been specified.
     */

    Tcl_SourceRCFile(interp);

    /*
     * Establish a channel handler for stdin.
     */

    Tcl_Channel inChannel = Tcl_GetStdChannel(TCL_STDIN);
    if (inChannel) {
      Tcl_CreateChannelHandler(inChannel, TCL_READABLE, my::StdinProc,
                               (ClientData) inChannel);
    }
    if (tsdPtr->tty) {
      my::Prompt(interp, 0);
    }
  }
  Tcl_DStringFree(&argString);

  Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel) {
    Tcl_Flush(outChannel);
  }
  Tcl_DStringInit(&tsdPtr->command);
  Tcl_DStringInit(&tsdPtr->line);
  Tcl_ResetResult(interp);

  /*
   * Loop infinitely, waiting for commands to execute.  When there
   * are no windows left, Tk_MainLoop returns and we exit.
   */

  Tk_MainLoop();
  Tcl_DeleteInterp(interp);
  Tcl_Exit(0);

  return 0;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
