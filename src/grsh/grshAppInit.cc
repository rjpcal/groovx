///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
// Rob Peters
// created: Nov-98
// written: Sat Jul  3 10:13:19 1999
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
///////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <tk.h>
#include <togl.h>

#define NO_TRACE
#include "trace.h"

// Forward declarations of package init procedures
extern "C" {
  Tcl_PackageInitProc Bitmap_Init;
  Tcl_PackageInitProc Block_Init;
  Tcl_PackageInitProc Expt_Init;
  Tcl_PackageInitProc Expttest_Init;
  Tcl_PackageInitProc Face_Init;
  Tcl_PackageInitProc Fixpt_Init;
  Tcl_PackageInitProc Grobj_Init;
  Tcl_PackageInitProc Gtext_Init;
  Tcl_PackageInitProc Jitter_Init;
  Tcl_PackageInitProc Misc_Init;
  Tcl_PackageInitProc Objlist_Init;
  Tcl_PackageInitProc Objtogl_Init;
  Tcl_PackageInitProc Pos_Init;
  Tcl_PackageInitProc Poslist_Init;
  Tcl_PackageInitProc Rh_Init;
  Tcl_PackageInitProc Sound_Init;
  Tcl_PackageInitProc Subject_Init;
  Tcl_PackageInitProc Tcldlist_Init;
  Tcl_PackageInitProc Tclgl_Init;
  Tcl_PackageInitProc Th_Init;
  Tcl_PackageInitProc Tlist_Init;
  Tcl_PackageInitProc Trial_Init;
}

struct PkgName_PkgProc {
  const char* PkgName;
  Tcl_PackageInitProc *PkgProc;
};

PkgName_PkgProc Names_Procs[] = {
  { "Tcl",      Tcl_Init       }
  , { "Tk",       Tk_Init        }
  , { "Togl",     Togl_Init      }
  , { "Bitmap",   Bitmap_Init    }
  , { "Block",    Block_Init     }
  , { "Expt",     Expt_Init      }
  , { "ExptTest", Expttest_Init  }
  , { "Face",     Face_Init      }
  , { "FixPt",    Fixpt_Init     }
  , { "Grobj",    Grobj_Init     }
  , { "Gtext",    Gtext_Init     }
  , { "Jitter",   Jitter_Init    }
  , { "Misc",     Misc_Init      }
  , { "ObjList",  Objlist_Init   }
  , { "Objtogl",  Objtogl_Init   }
  , { "Pos",      Pos_Init       }
  , { "PosList",  Poslist_Init   }
  , { "Rh",       Rh_Init        }
  , { "Sound",    Sound_Init     }
  , { "Subject",  Subject_Init   }
  , { "Tcldlist", Tcldlist_Init  }
  , { "TclGL",    Tclgl_Init     }
  , { "Th",       Th_Init        }
  , { "Tlist",    Tlist_Init     }
  , { "Trial",    Trial_Init     }
};

class TclApp {
public:
  TclApp(Tcl_Interp* interp) : 
	 itsInterp(interp), 
	 itsStatus(TCL_OK)
  {
  DOTRACE("TclApp::TclApp(Tcl_Interp*)");

    int result;

    for (int i = 0; i < sizeof(Names_Procs)/sizeof(PkgName_PkgProc); ++i) {
		//		cerr << "initializing " << Names_Procs[i].PkgName << endl << flush;
		result = Names_Procs[i].PkgProc(itsInterp);
		if (result != TCL_OK) { itsStatus = result; }
	 }

	 // set prompt to "cmd[n]% " where cmd is the name of the program,
	 // and n is the history event number
	 Tcl_SetVar(interp, "tcl_prompt1", 
					"puts -nonewline \"$argv0\\[[history nextid]\\]% \"",
					TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
	 
	 // specifies a file to be 'source'd upon startup
	 Tcl_SetVar(interp, "tcl_rcFileName", "./grsh_startup.tcl", TCL_GLOBAL_ONLY);
  }

  int status() const { return itsStatus; }
private:
  Tcl_Interp* itsInterp;
  int itsStatus;
};

// initialize all the necessary packages
int Tcl_AppInit(Tcl_Interp* interp) {
DOTRACE("Tcl_AppInit");
  static TclApp theApp(interp);
  return theApp.status();
}

int main(int argc, char** argv) {
  Tk_Main(argc, argv, Tcl_AppInit);
  return 0;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
