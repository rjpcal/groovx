///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  2 08:00:00 1998
// written: Mon May  5 16:39:38 2003
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
///////////////////////////////////////////////////////////////////////

#include "grsh/grsh.h"

#include "system/demangle.h"
#include "system/system.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/strings.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <tk.h>
#include <typeinfo>

#include "util/debug.h"
#include "util/trace.h"

//
// Forward declarations of package init procedures
//

extern "C"
{
  Tcl_PackageInitProc Block_Init;
  Tcl_PackageInitProc Dlist_Init;
  Tcl_PackageInitProc Expt_Init;
  Tcl_PackageInitProc Face_Init;
  Tcl_PackageInitProc Fish_Init;
  Tcl_PackageInitProc Fixpt_Init;
  Tcl_PackageInitProc Gabor_Init;
  Tcl_PackageInitProc Gltcl_Init;
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
  Tcl_PackageInitProc Rh_Init;
  Tcl_PackageInitProc Sound_Init;
  Tcl_PackageInitProc Th_Init;
  Tcl_PackageInitProc Tlist_Init;
  Tcl_PackageInitProc Toglet_Init;
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
  { "Block",    Block_Init     },
  { "Dlist",    Dlist_Init     },
  { "Expt",     Expt_Init      },
  { "Face",     Face_Init      },
  { "Fish",     Fish_Init      },
  { "Fixpt",    Fixpt_Init     },
  { "Gabor",    Gabor_Init     },
  { "Gltcl",    Gltcl_Init     },
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
  { "Rh",       Rh_Init        },
  { "Sound",    Sound_Init     },
  { "Th",       Th_Init        },
  { "Tlist",    Tlist_Init     },
  { "Toglet",   Toglet_Init    },
  { "Trial",    Trial_Init     },
};

#if 0
PackageInfo DELAYED_PKGS[] = {};
#endif

int          appArgc      = 0;
char**       appArgv      = 0;

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////
//
// namespace Grsh function definitions
//
///////////////////////////////////////////////////////////////////////

int Grsh::argc()
{
DOTRACE("Grsh::argc");

  if (appArgc == 0)
    throw Util::Error("appArgc not inited");
  return appArgc;
}

char** Grsh::argv()
{
DOTRACE("Grsh::argv");

  if (appArgv == 0)
    throw Util::Error("appArgv not inited");
  return appArgv;
}

const char* Grsh::libraryDirectory()
{
DOTRACE("Grsh::libraryDirectory");

  return System::theSystem().getenv("GRSH_LIB_DIR");
}

///////////////////////////////////////////////////////////////////////
//
// main procedure
//
///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
DOTRACE("main");

  try
    {
      appArgc = argc;
      appArgv = argv;

      Tcl::Main app(argc, argv);

      if (Tcl::Main::isInteractive())
        {
          std::cerr << PACKAGE_STRING << " (" << __DATE__ << ")\n"
                    << "Copyright (c) 1998-2003 Rob Peters <rjpeters at klab dot caltech dot edu>\n"
                    << PACKAGE_NAME << " is free software, covered by the GNU General Public License, and you are\n"
                    << "welcome to change it and/or distribute copies of it under certain conditions.\n";
        }

      Tcl::Interp& interp = app.interp();

      for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
        {
#ifdef LOCAL_TRACE
          std::cerr << "initializing " << IMMEDIATE_PKGS[i].pkgName << '\n';
#endif
          int result = IMMEDIATE_PKGS[i].pkgInitProc(interp.intp());
          if (result != TCL_OK)
            {
              std::cerr << "initialization failed: "
                        << IMMEDIATE_PKGS[i].pkgName << '\n';
              fstring msg = interp.getResult<const char*>();
              if ( !msg.is_empty() )
                std::cerr << '\t' << msg << '\n';
              interp.resetResult();
            }
        }

      Tcl::List path = interp.getGlobalVar<Tcl::List>("auto_path");

      path.append(VISX_LIB_DIR);

      interp.setGlobalVar("auto_path", path.asObj());

      // specifies a file to be 'source'd upon startup
      interp.setGlobalVar("tcl_rcFileName",
                          Tcl::toTcl("./grsh_startup.tcl"));

      app.run();
      return 0;
    }
  catch (Util::Error& err)
    {
      std::cerr << "caught in main: Error ("
                << demangle_cstr(typeid(err).name())
                << "): " << err.msg_cstr() << '\n';
      err.backTrace().print(std::cerr);
    }
  catch (std::exception& err)
    {
      std::cerr << "caught in main: std::exception ("
                << demangle_cstr(typeid(err).name())
                << "): " << err.what() << '\n';
    }
  catch (...)
    {
      std::cerr << "caught in main: (an exception of unknown type)\n";
    }

  // if we got here, then some error occurred
  return -1;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
