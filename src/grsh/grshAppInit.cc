///////////////////////////////////////////////////////////////////////
// grshAppInit.cc
// Rob Peters
// created: Nov-98
// written: Tue Apr 13 14:31:03 1999
// $Id$
//
// This is the main application file for a Tcl/Tk application that
// manages collections of GrObj's (graphic objects) within an
// ObjList (a list of GrObj's). Each GrObj must be defined to be
// able to display itself using OpenGL (OpenGL commands are rendered
// through a Togl widget). A Glist is a list of lists of objID's. Each
// of the sublists can be easily rendered in the Togl window using the
// "show" Tcl command. This makes it easy to set up experiments in
// which different combinations of images are shown in series.
///////////////////////////////////////////////////////////////////////

#include "tk.h"
#include "togl.h"

// forward declarations of package init procedures (entire ".h" files
// are not needed here
namespace ExptTcl       { Tcl_PackageInitProc Expt_Init;        }
namespace FaceTcl       { Tcl_PackageInitProc Face_Init;        }
namespace FixptTcl      { Tcl_PackageInitProc Fixpt_Init;       }
namespace JitterTcl     { Tcl_PackageInitProc Jitter_Init;      }
namespace MiscTcl       { Tcl_PackageInitProc Misc_Init;        }
namespace ObjlistTcl    { Tcl_PackageInitProc Objlist_Init;     }
namespace ObjTogl       { Tcl_PackageInitProc Objtogl_Init;     }
namespace PositionTcl   { Tcl_PackageInitProc Position_Init;    }
namespace PoslistTcl    { Tcl_PackageInitProc Poslist_Init;     }
namespace SoundTcl      { Tcl_PackageInitProc Sound_Init;       }
namespace SubjectTcl    { Tcl_PackageInitProc Subject_Init;     }
namespace Tcldlist      { Tcl_PackageInitProc Tcldlist_Init;    }
namespace TclGL         { Tcl_PackageInitProc Tclgl_Init;       }
namespace TlistTcl      { Tcl_PackageInitProc Tlist_Init;       }

#define NO_TRACE
#include "trace.h"

int main(int argc, char **argv) {
  Tk_Main(argc, argv, Tcl_AppInit);
  return 0;
}

// initialize all the necessary packages
int Tcl_AppInit(Tcl_Interp *interp) {
DOTRACE("Tcl_AppInit");

  if (Tcl_Init(interp)                  != TCL_OK)   { return TCL_ERROR; }
  if (Tk_Init(interp)                   != TCL_OK)   { return TCL_ERROR; }
  if (Togl_Init(interp)                 != TCL_OK)   { return TCL_ERROR; }

  if (ExptTcl::Expt_Init(interp)        != TCL_OK)   { return TCL_ERROR; }
  if (FaceTcl::Face_Init(interp)        != TCL_OK)   { return TCL_ERROR; }
  if (FixptTcl::Fixpt_Init(interp)      != TCL_OK)   { return TCL_ERROR; }
  if (JitterTcl::Jitter_Init(interp)    != TCL_OK)   { return TCL_ERROR; }
  if (MiscTcl::Misc_Init(interp)        != TCL_OK)   { return TCL_ERROR; }
  if (ObjlistTcl::Objlist_Init(interp)  != TCL_OK)   { return TCL_ERROR; }
  if (ObjTogl::Objtogl_Init(interp)     != TCL_OK)   { return TCL_ERROR; }
  if (PositionTcl::Position_Init(interp) != TCL_OK)  { return TCL_ERROR; }
  if (PoslistTcl::Poslist_Init(interp)  != TCL_OK)   { return TCL_ERROR; }
  if (SoundTcl::Sound_Init(interp)      != TCL_OK)   { return TCL_ERROR; }
  if (SubjectTcl::Subject_Init(interp)  != TCL_OK)   { return TCL_ERROR; }
  if (Tcldlist::Tcldlist_Init(interp)   != TCL_OK)   { return TCL_ERROR; }
  if (TclGL::Tclgl_Init(interp)         != TCL_OK)   { return TCL_ERROR; }
  if (TlistTcl::Tlist_Init(interp)      != TCL_OK)   { return TCL_ERROR; }

  // set prompt to "grsh[n]% " where n is the history event number
  Tcl_SetVar(interp, "tcl_prompt1", 
             "puts -nonewline \"$argv0\\[[history nextid]\\]% \"",
             TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

  // specifies a file to be 'source'd upon startup
  Tcl_SetVar(interp, "tcl_rcFileName", "./grsh_startup.tcl", TCL_GLOBAL_ONLY);
  return TCL_OK;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
