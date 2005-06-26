///////////////////////////////////////////////////////////////////////
//
// $URL$
//
// Copyright (c) 1998-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
// $HeadURL$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#include "nub/objfactory.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclmain.h"
#include "tcl/tclpkg.h"
#include "tcl/tclsafeinterp.h"

#include "util/demangle.h"
#include "util/error.h"
#include "util/fstring.h"
#include "util/time.h"

#include <cstdlib> // for atoi()
#include <cstring> // for strcmp()
#include <exception>
#include <iostream>
#include <signal.h>
#include <tk.h>
#include <typeinfo>

// #include's for all the *_Init() procedures

#include "gfx/canvastcl.h"                   // for Canvas_Init(), Glcanvas_Init(),
#include "gfx/gxtcl.h"                       // for Gx_Init(), Gxnode_Init(), Gxseparator_Init(), Gxcolor_Init(), Gxdrawstyle_Init(), Gxline_Init(), Gxcylinder_Init(), Gxsphere_Init(), Gxlighting_Init(), Gxmaterial_Init(), Gxpointset_Init(), Gxscaler_Init(), Gxemptynode_Init(), Gxtransform_Init(), Gxshapekit_Init(), Gxpixmap_Init(), Gxtext_Init(), Gxfixedscalecamera_Init(), Gxpsyphycamera_Init(), Gxperspectivecamera_Init(), Gxdisk_Init(),
#include "gfx/toglettcl.h"                   // for Toglet_Init(),
#include "io/iotcl.h"                        // for Io_Init(), Outputfile_Init(),
#include "tcl/dlisttcl.h"                    // for Dlist_Init(),
#include "tcl/gtracetcl.h"                   // for Gtrace_Init(), Prof_Init(),
#include "tcl/logtcl.h"                      // for Log_Init(),
#include "tcl/misctcl.h"                     // for Misc_Init(),
#include "tcl/objtcl.h"                      // for Objdb_Init(), Obj_Init(), Objdb_Init(), Obj_Init(),
#include "tk/tkwidgettcl.h"                  // for Tkwidget_Init(),
#include "visx/blocktcl.h"                   // for Block_Init(),
#include "visx/elementcontainertcl.h"        // for Elementcontainer_Init(),
#include "visx/elementtcl.h"                 // for Element_Init(),
#include "visx/expttcl.h"                    // for Exptdriver_Init(),
#include "visx/facetcl.h"                    // for Face_Init(), Cloneface_Init(),
#include "visx/fishtcl.h"                    // for Fish_Init(),
#include "visx/fixpttcl.h"                   // for Fixpt_Init(),
#include "visx/gabortcl.h"                   // for Gabor_Init(), Gaborarray_Init(),
#include "visx/gltcl.h"                      // for Gl_Init(),
#include "visx/hooktcl.h"                    // for Hook_Init(),
#include "visx/housetcl.h"                   // for House_Init(),
#include "visx/jittertcl.h"                  // for Jitter_Init(),
#include "visx/masktcl.h"                    // for Maskhatch_Init(),
#include "visx/morphyfacetcl.h"              // for Morphyface_Init(),
#include "visx/rhtcl.h"                      // for Responsehandler_Init(), Eventresponsehdlr_Init(), Kbdresponsehdlr_Init(), Nullresponsehdlr_Init(), Serialrh_Init(),
#include "visx/soundtcl.h"                   // for Sound_Init(),
#include "visx/thtcl.h"                      // for Timinghdlr_Init(), Timinghandler_Init(),
#include "visx/tlisttcl.h"                   // for Tlist_Init(),
#include "visx/trialeventtcl.h"              // for Trialevent_Init(), Nulltrialevent_Init(), Filewriteevent_Init(), Genericevent_Init(), Multievent_Init(),
#include "visx/trialtcl.h"                   // for Trial_Init(),

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

//
// Info about the packages to be loaded
//

namespace
{

struct PackageInfo
{
  const char* pkgName;
  Tcl_PackageInitProc *pkgInitProc;
  const char* pkgVersion;
  bool skipIfNoWindow;
};

PackageInfo IMMEDIATE_PKGS[] =
  {
    { "Tcl",      Tcl_Init,  "", false },
    { "Tk",       Tk_Init,   "", true },
  };

PackageInfo DELAYED_PKGS[] =
  {
    { "Block",               Block_Init,               "4.0", false },
    { "Canvas",              Canvas_Init,              "4.0", false },
    { "Cloneface",           Cloneface_Init,           "4.0", false },
    { "Dlist",               Dlist_Init,               "4.0", false },
    { "Element",             Element_Init,             "4.0", false },
    { "Elementcontainer",    Elementcontainer_Init,    "4.0", false },
    { "Eventresponsehdlr",   Eventresponsehdlr_Init,   "4.0", false },
    { "Exptdriver",          Exptdriver_Init,          "4.0", false },
    { "Face",                Face_Init,                "4.0", false },
    { "Filewriteevent",      Filewriteevent_Init,      "4.0", false },
    { "Fish",                Fish_Init,                "4.0", false },
    { "Fixpt",               Fixpt_Init,               "4.0", false },
    { "Gabor",               Gabor_Init,               "4.0", false },
    { "Gaborarray",          Gaborarray_Init,          "4.0", false },
    { "Genericevent",        Genericevent_Init,        "4.0", false },
    { "Gl",                  Gl_Init,                  "4.0", false },
    { "Glcanvas",            Glcanvas_Init,            "4.0", false },
    { "Gtrace",              Gtrace_Init,              "4.0", false },
    { "Gx",                  Gx_Init,                  "4.0", false },
    { "Gxcolor",             Gxcolor_Init,             "4.0", false },
    { "Gxcylinder",          Gxcylinder_Init,          "4.0", false },
    { "Gxdisk",              Gxdisk_Init,              "4.0", false },
    { "Gxdrawstyle",         Gxdrawstyle_Init,         "4.0", false },
    { "Gxemptynode",         Gxemptynode_Init,         "4.0", false },
    { "Gxfixedscalecamera",  Gxfixedscalecamera_Init,  "4.0", false },
    { "Gxlighting",          Gxlighting_Init,          "4.0", false },
    { "Gxline",              Gxline_Init,              "4.0", false },
    { "Gxmaterial",          Gxmaterial_Init,          "4.0", false },
    { "Gxnode",              Gxnode_Init,              "4.0", false },
    { "Gxperspectivecamera", Gxperspectivecamera_Init, "4.0", false },
    { "Gxpixmap",            Gxpixmap_Init,            "4.0", false },
    { "Gxpointset",          Gxpointset_Init,          "4.0", false },
    { "Gxpsyphycamera",      Gxpsyphycamera_Init,      "4.0", false },
    { "Gxscaler",            Gxscaler_Init,            "4.0", false },
    { "Gxseparator",         Gxseparator_Init,         "4.0", false },
    { "Gxshapekit",          Gxshapekit_Init,          "4.0", false },
    { "Gxsphere",            Gxsphere_Init,            "4.0", false },
    { "Gxtext",              Gxtext_Init,              "4.0", false },
    { "Gxtransform",         Gxtransform_Init,         "4.0", false },
    { "Hook",                Hook_Init,                "4.0", false },
    { "House",               House_Init,               "4.0", false },
    { "Io",                  Io_Init,                  "4.0", false },
    { "Jitter",              Jitter_Init,              "4.0", false },
    { "Log",                 Log_Init,                 "4.0", false },
    { "Kbdresponsehdlr",     Kbdresponsehdlr_Init,     "4.0", false },
    { "Maskhatch",           Maskhatch_Init,           "4.0", false },
    { "Misc",                Misc_Init,                "4.0", false },
    { "Morphyface",          Morphyface_Init,          "4.0", false },
    { "Multievent",          Multievent_Init,          "4.0", false },
    { "Nullresponsehdlr",    Nullresponsehdlr_Init,    "4.0", false },
    { "Nulltrialevent",      Nulltrialevent_Init,      "4.0", false },
    { "Obj",                 Obj_Init,                 "4.0", false },
    { "Objdb",               Objdb_Init,               "4.0", false },
    { "Outputfile",          Outputfile_Init,          "4.0", false },
    { "Prof",                Prof_Init,                "4.0", false },
    { "Responsehandler",     Responsehandler_Init,     "4.0", false },
    { "Serialrh",            Serialrh_Init,            "4.0", false },
    { "Sound",               Sound_Init,               "4.0", false },
    { "Timinghandler",       Timinghandler_Init,       "4.0", false },
    { "Timinghdlr",          Timinghdlr_Init,          "4.0", false },
    { "Tkwidget",            Tkwidget_Init,            "4.0", false },
    { "Tlist",               Tlist_Init,               "4.0", false },
    { "Toglet",              Toglet_Init,              "4.0", true },
    { "Trial",               Trial_Init,               "4.0", false },
    { "Trialevent",          Trialevent_Init,          "4.0", false },
  };

  void sigHandler(int signum)
  {
    switch (signum)
      {
        case SIGSEGV: PANIC("Segmentation fault (SIGSEGV)");
        case SIGFPE:  PANIC("Floating point exception (SIGFPE)");
        case SIGBUS:  PANIC("Bus error (SIGBUS)");
      }
    ASSERT(0);
  }

  // This is a fallback function to be used by the object
  // factory... if the factory can't figure out how to create a given
  // type, it will call this fallback function first before giving up
  // for good. This callback function tries to load a Tcl package
  // named after the desired object type.
  void factoryPkgLoader(const rutz::fstring& type)
  {
    dbg_eval_nl(3, type);

    Tcl::Pkg::lookup(Tcl::Main::interp(), type.c_str());
  }

} // end anonymous namespace

///////////////////////////////////////////////////////////////////////
//
// main procedure
//
///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
DOTRACE("main");

  signal(SIGSEGV, &sigHandler);
  signal(SIGFPE, &sigHandler);
  signal(SIGBUS, &sigHandler);

  bool minimal = false;
  bool nowindow = false;

  try
    {
      // Quick check argv to optionally turn on global tracing and/or set
      // the global debug level. This method is particularly useful for
      // helping to diagnose problems that are occurring during application
      // startup, before we have a chance to get to the command-line prompt
      // and do a "::gtrace 1" or a "::dbgLevel 9".
      for (int i = 0; i < argc; ++i)
        {
          if (strcmp(argv[i], "-dbglevel") == 0)
            {
              if (argv[i+1] != 0)
                rutz::debug::set_global_level( atoi(argv[i+1]) );
            }
          else if (strcmp(argv[i], "-gtrace") == 0)
            {
              rutz::trace::set_global_trace(true);
            }
          else if (strcmp(argv[i], "-showinit") == 0)
            {
              Tcl::Pkg::verboseInit(true);
            }
          else if (strcmp(argv[i], "-minimal") == 0)
            {
              minimal = true;
            }
          else if (strcmp(argv[i], "-nw") == 0)
            {
              nowindow = true;
            }
        }

      Tcl::Main app(argc, argv, nowindow);

      if (Tcl::Main::isInteractive())
        {
          std::cerr << PACKAGE_STRING << " (" << __DATE__ << ")\n"
                    << "Copyright (c) 1998-2005 Rob Peters <rjpeters at klab dot caltech dot edu>\n"
                    << PACKAGE_NAME << " is free software, covered by the GNU General Public License, and you are\n"
                    << "welcome to change it and/or distribute copies of it under certain conditions.\n";
        }

      Tcl::Interp& interp = app.interp();

      Nub::ObjFactory::theOne().set_fallback(&factoryPkgLoader);

      const rutz::time ru1 = rutz::time::user_rusage();
      const rutz::time rs1 = rutz::time::sys_rusage();
      const rutz::time wc1 = rutz::time::wall_clock_now();

      for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
        {
          if (nowindow && IMMEDIATE_PKGS[i].skipIfNoWindow)
            continue;

          int result = IMMEDIATE_PKGS[i].pkgInitProc(interp.intp());
          if (result != TCL_OK)
            {
              std::cerr << "fatal initialization error (package '"
                        << IMMEDIATE_PKGS[i].pkgName << "'):\n";
              rutz::fstring msg = interp.getResult<const char*>();
              if ( !msg.is_empty() )
                std::cerr << '\t' << msg << '\n';
              interp.resetResult();

              return 2;
            }
        }

      if (Tcl::Main::isInteractive())
        {
          const rutz::time ru = rutz::time::user_rusage() - ru1;
          const rutz::time rs = rutz::time::sys_rusage() - rs1;
          const rutz::time wc = rutz::time::wall_clock_now() - wc1;

          fprintf(stderr, "\tstartup time (tcl+tk) "
                  "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
                  ru.sec(), rs.sec(), wc.sec());
        }

      const rutz::time ru2 = rutz::time::user_rusage();
      const rutz::time rs2 = rutz::time::sys_rusage();
      const rutz::time wc2 = rutz::time::wall_clock_now();

      for (size_t i = 0; i < sizeof(DELAYED_PKGS)/sizeof(PackageInfo); ++i)
        {
          Tcl_StaticPackage(static_cast<Tcl_Interp*>(0),
                            // (Tcl_Interp*) 0 means this package
                            // hasn't yet been loaded into any
                            // interpreter
                            DELAYED_PKGS[i].pkgName,
                            DELAYED_PKGS[i].pkgInitProc,
                            0);

          rutz::fstring ifneededcmd("package ifneeded ",
                                    DELAYED_PKGS[i].pkgName,
                                    " ",
                                    DELAYED_PKGS[i].pkgVersion,
                                    " {load {} ",
                                    DELAYED_PKGS[i].pkgName,
                                    " }");

          interp.eval(ifneededcmd);
        }

      if (!minimal)
        {
          for (size_t i = 0; i < sizeof(DELAYED_PKGS)/sizeof(PackageInfo); ++i)
            {
              if (nowindow && DELAYED_PKGS[i].skipIfNoWindow)
                continue;

              const char* ver =
                Tcl_PkgRequire(interp.intp(),
                               DELAYED_PKGS[i].pkgName,
                               DELAYED_PKGS[i].pkgVersion,
                               0);

              if (ver == 0)
                {
                  std::cerr << "initialization error (package '"
                            << DELAYED_PKGS[i].pkgName << "'):\n";
                  rutz::fstring msg = interp.getResult<const char*>();
                  if ( !msg.is_empty() )
                    std::cerr << '\t' << msg << '\n';
                  interp.resetResult();
                }
            }
        }

      if (Tcl::Main::isInteractive())
        {
          const rutz::time ru = rutz::time::user_rusage() - ru2;
          const rutz::time rs = rutz::time::sys_rusage() - rs2;
          const rutz::time wc = rutz::time::wall_clock_now() - wc2;

          fprintf(stderr, "\tstartup time (GroovX) "
                  "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
                  ru.sec(), rs.sec(), wc.sec());
        }

      Tcl::List path = interp.getGlobalVar<Tcl::List>("auto_path");

      path.append(VISX_LIB_DIR);

      interp.setGlobalVar("auto_path", path.asObj());

      // specifies a file to be 'source'd upon startup
      interp.setGlobalVar("tcl_rcFileName",
                          Tcl::toTcl("./groovx_startup.tcl"));

      app.run();
      return 0;
    }
  catch (std::exception& err)
    {
      std::cerr << "caught in main: ("
                << rutz::demangled_name(typeid(err))
                << "): " << err.what() << '\n';
    }
  catch (...)
    {
      std::cerr << "caught in main: (an exception of unknown type)\n";
    }

  // if we got here, then some error occurred
  return 1;
}

static const char vcid_grshAppInit_cc[] = "$URL$ $Id$";
