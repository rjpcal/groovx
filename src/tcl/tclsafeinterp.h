///////////////////////////////////////////////////////////////////////
//
// tclutil.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 11 10:25:36 2000
// written: Wed Oct 11 11:34:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLUTIL_H_DEFINED
#define TCLUTIL_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLERROR_H_DEFINED)
#include "tcl/tclerror.h"
#endif

struct Tcl_Interp;
struct Tcl_Obj;

namespace Tcl {
  namespace Safe {
	 int listLength(Tcl_Interp* interp, Tcl_Obj* tcllist) throw(TclError);

	 Tcl_Obj* listElement(Tcl_Interp* interp,
								 Tcl_Obj* tcllist, int index) throw(TclError);

	 void splitList(Tcl_Interp* interp, Tcl_Obj* tcllist,
			  Tcl_Obj**& elements_out, int& length_out) throw(TclError);

	 int getInt(Tcl_Interp* interp, Tcl_Obj* intObj) throw(TclError);
  }

  class SafeInterp;
}

class Tcl::SafeInterp {
private:
  Tcl_Interp* itsInterp;
  Util::ErrorHandler* itsErrHandler;
  int itsLastResult;

  void handleError(const char* msg, int result);

public:
  enum ErrMode { IGNORE, BKD_ERROR, THROW };

  SafeInterp(Tcl_Interp* interp, ErrMode mode);
  ~SafeInterp();

  Tcl_Interp* interp() const { return itsInterp; }

  int lastResult() const { return itsLastResult; }
};

static const char vcid_tclutil_h[] = "$Header$";
#endif // !TCLUTIL_H_DEFINED
