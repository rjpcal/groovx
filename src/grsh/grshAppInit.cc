///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  2 08:00:00 1998
// written: Wed Sep 11 14:32:10 2002
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
///////////////////////////////////////////////////////////////////////

#include "visx/application.h"

#include "system/demangle.h"

#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/error.h"
#include "util/strings.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <tk.h>
#include <typeinfo>

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
// main procedure
//
///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  try
    {
      Tcl::Main app(argc, argv);

      Application grshApp(argc, argv);

      Tcl::Interp& safeIntp = app.safeInterp();

      for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
        {
#ifdef LOCAL_TRACE
          std::cerr << "initializing " << IMMEDIATE_PKGS[i].pkgName << '\n';
#endif
          int result = IMMEDIATE_PKGS[i].pkgInitProc(app.interp());
          if (result != TCL_OK)
            {
              std::cerr << "initialization failed: "
                        << IMMEDIATE_PKGS[i].pkgName << '\n';
              fstring msg = safeIntp.getResult<const char*>();
              if ( !msg.is_empty() )
                std::cerr << '\t' << msg << '\n';
              safeIntp.resetResult();
            }
        }

      // set prompt to "cmd[n]% " where cmd is the name of the program,
      // and n is the history event number
      safeIntp.setGlobalVar("tcl_prompt1",
                    "puts -nonewline \"([history nextid]) $argv0> \"");

      // specifies a file to be 'source'd upon startup
      safeIntp.setGlobalVar("tcl_rcFileName", "./grsh_startup.tcl");

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
