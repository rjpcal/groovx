///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Mon Dec 11 15:48:06 2000
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
///////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <tk.h>

#include "grshapp.h"

#define NO_TRACE
#include "util/trace.h"

//
// Forward declarations of package init procedures
//

extern "C" {
  Tcl_PackageInitProc Bitmap_Init;
  Tcl_PackageInitProc Block_Init;
  Tcl_PackageInitProc Expt_Init;
  Tcl_PackageInitProc Expttest_Init;
  Tcl_PackageInitProc Face_Init;
  Tcl_PackageInitProc Fish_Init;
  Tcl_PackageInitProc Fixpt_Init;
  Tcl_PackageInitProc Gabor_Init;
  Tcl_PackageInitProc Grobj_Init;
  Tcl_PackageInitProc Gtext_Init;
  Tcl_PackageInitProc Gx_Init;
  Tcl_PackageInitProc House_Init;
  Tcl_PackageInitProc Io_Init;
  Tcl_PackageInitProc Jitter_Init;
  Tcl_PackageInitProc Mask_Init;
  Tcl_PackageInitProc Misc_Init;
  Tcl_PackageInitProc Morphyface_Init;
  Tcl_PackageInitProc Objlist_Init;
  Tcl_PackageInitProc Objtogl_Init;
  Tcl_PackageInitProc Pos_Init;
  Tcl_PackageInitProc Rh_Init;
  Tcl_PackageInitProc Sound_Init;
  Tcl_PackageInitProc Subject_Init;
  Tcl_PackageInitProc Tcldlist_Init;
  Tcl_PackageInitProc Tclgl_Init;
  Tcl_PackageInitProc Th_Init;
  Tcl_PackageInitProc Tlist_Init;
  Tcl_PackageInitProc Togl_Init;
  Tcl_PackageInitProc Trial_Init;
}

//
// Info about the packages to be loaded
//

namespace {

struct PackageInfo {
  const char* pkgName;
  Tcl_PackageInitProc *pkgInitProc;
};

PackageInfo IMMEDIATE_PKGS[] = {
  { "Tcl",      Tcl_Init       }
  , { "Tk",       Tk_Init        }
  , { "Togl",     Togl_Init      }
  , { "Bitmap",   Bitmap_Init    }
  , { "Block",    Block_Init     }
  , { "Expt",     Expt_Init      }
  , { "Expttest", Expttest_Init  }
  , { "Face",     Face_Init      }
  , { "Fish",     Fish_Init      }
  , { "Fixpt",    Fixpt_Init     }
  , { "Gabor",    Gabor_Init     }
  , { "Grobj",    Grobj_Init     }
  , { "Gtext",    Gtext_Init     }
  , { "Gx",       Gx_Init        }
  , { "House",    House_Init     }
  , { "Io",       Io_Init        }
  , { "Jitter",   Jitter_Init    }
  , { "Mask",     Mask_Init      }
  , { "Misc",     Misc_Init      }
  , { "Morphyface",Morphyface_Init}
  , { "Objlist",  Objlist_Init   }
  , { "Objtogl",  Objtogl_Init   }
  , { "Pos",      Pos_Init       }
  , { "Rh",       Rh_Init        }
  , { "Sound",    Sound_Init     }
  , { "Subject",  Subject_Init   }
  , { "Tcldlist", Tcldlist_Init  }
  , { "Tclgl",    Tclgl_Init     }
  , { "Th",       Th_Init        }
  , { "Tlist",    Tlist_Init     }
  , { "Trial",    Trial_Init     }
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

class TclApp : public GrshApp {
public:
  TclApp(int argc, char** argv, Tcl_Interp* interp);

  int status() const { return itsStatus; }
private:
  int itsStatus;
};

TclApp::TclApp(int argc, char** argv, Tcl_Interp* interp) : 
  GrshApp(argc, argv, interp),
  itsStatus(TCL_OK)
{
DOTRACE("TclApp::TclApp(Tcl_Interp*)");

  {for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i) {
#ifdef LOCAL_TRACE		
	 cerr << "initializing " << IMMEDIATE_PKGS[i].pkgName << endl << flush;
#endif
	 int result = IMMEDIATE_PKGS[i].pkgInitProc(interp);
	 if (result != TCL_OK) { itsStatus = result; }
  }}

#if 0
  {for (size_t i = 0; i < sizeof(DELAYED_PKGS)/sizeof(PackageInfo); ++i) {
	 Tcl_StaticPackage((Tcl_Interp*) 0,
							 const_cast<char*>(DELAYED_PKGS[i].pkgName),
							 DELAYED_PKGS[i].pkgInitProc,
							 0);
  }}
#endif

  // set prompt to "cmd[n]% " where cmd is the name of the program,
  // and n is the history event number
  Tcl_SetVar(interp, "tcl_prompt1", 
				 "puts -nonewline \"([history nextid]) $argv0> \"",
				 TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
	 
  // specifies a file to be 'source'd upon startup
  Tcl_SetVar(interp, "tcl_rcFileName", "./grsh_startup.tcl", TCL_GLOBAL_ONLY);
}


///////////////////////////////////////////////////////////////////////
//
// main
//
///////////////////////////////////////////////////////////////////////

namespace {
  int LOCAL_ARGC = 0;
  char** LOCAL_ARGV = 0;
}

// procedure to initialize a TclApp
int Tcl_AppInit(Tcl_Interp* interp) {
DOTRACE("Tcl_AppInit");
  static TclApp theApp(LOCAL_ARGC, LOCAL_ARGV, interp);
  return theApp.status();
}

int main(int argc, char** argv) {
  LOCAL_ARGC = argc;
  LOCAL_ARGV = argv;
  Tk_Main(argc, argv, Tcl_AppInit);
  return 0;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
