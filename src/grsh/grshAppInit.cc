///////////////////////////////////////////////////////////////////////
//
// grshAppInit.cc
//
// Copyright (c) 1998-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  2 08:00:00 1998
// commit: $Id$
//
// This is the main application file for a Tcl/Tk application that
// runs visual experiments.
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "grsh/grsh.h"

#include "system/system.h"

#include "tcl/tcllistobj.h"
#include "tcl/tclmain.h"
#include "tcl/tclsafeinterp.h"

#include "util/demangle.h"
#include "util/error.h"
#include "util/strings.h"
#include "util/time.h"

#include <cstdlib> // for atoi()
#include <cstring> // for strcmp()
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
  Tcl_PackageInitProc Cloneface_Init;
  Tcl_PackageInitProc Dlist_Init;
  Tcl_PackageInitProc Eventresponsehdlr_Init;
  Tcl_PackageInitProc Exptdriver_Init;
  Tcl_PackageInitProc Face_Init;
  Tcl_PackageInitProc Fish_Init;
  Tcl_PackageInitProc Fixpt_Init;
  Tcl_PackageInitProc Gabor_Init;
  Tcl_PackageInitProc Gaborarray_Init;
  Tcl_PackageInitProc Gl_Init;
  Tcl_PackageInitProc Gtrace_Init;
  Tcl_PackageInitProc Gxcolor_Init;
  Tcl_PackageInitProc Gxcylinder_Init;
  Tcl_PackageInitProc Gxdisk_Init;
  Tcl_PackageInitProc Gxdrawstyle_Init;
  Tcl_PackageInitProc Gxemptynode_Init;
  Tcl_PackageInitProc Gxfixedscalecamera_Init;
  Tcl_PackageInitProc Gxlighting_Init;
  Tcl_PackageInitProc Gxline_Init;
  Tcl_PackageInitProc Gxmaterial_Init;
  Tcl_PackageInitProc Gxnode_Init;
  Tcl_PackageInitProc Gxperspectivecamera_Init;
  Tcl_PackageInitProc Gxpixmap_Init;
  Tcl_PackageInitProc Gxpointset_Init;
  Tcl_PackageInitProc Gxpsyphycamera_Init;
  Tcl_PackageInitProc Gxscaler_Init;
  Tcl_PackageInitProc Gxseparator_Init;
  Tcl_PackageInitProc Gxshapekit_Init;
  Tcl_PackageInitProc Gxsphere_Init;
  Tcl_PackageInitProc Gxtext_Init;
  Tcl_PackageInitProc Gxtransform_Init;
  Tcl_PackageInitProc Hook_Init;
  Tcl_PackageInitProc House_Init;
  Tcl_PackageInitProc Io_Init;
  Tcl_PackageInitProc Jitter_Init;
  Tcl_PackageInitProc Kbdresponsehdlr_Init;
  Tcl_PackageInitProc Maskhatch_Init;
  Tcl_PackageInitProc Misc_Init;
  Tcl_PackageInitProc Morphyface_Init;
  Tcl_PackageInitProc Nullresponsehdlr_Init;
  Tcl_PackageInitProc Obj_Init;
  Tcl_PackageInitProc Objdb_Init;
  Tcl_PackageInitProc Prof_Init;
  Tcl_PackageInitProc Responsehandler_Init;
  Tcl_PackageInitProc Serialrh_Init;
  Tcl_PackageInitProc Sound_Init;
  Tcl_PackageInitProc Tlist_Init;
  Tcl_PackageInitProc Toglet_Init;
  Tcl_PackageInitProc Trial_Init;
  Tcl_PackageInitProc Timinghdlr_Init;
  Tcl_PackageInitProc Timinghandler_Init;
  Tcl_PackageInitProc Trialevent_Init;
  Tcl_PackageInitProc Genericevent_Init;
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
  const char* pkgVersion;
};

PackageInfo IMMEDIATE_PKGS[] =
  {
    { "Tcl",      Tcl_Init,  ""       },
    { "Tk",       Tk_Init,   ""       },
  };

PackageInfo DELAYED_PKGS[] =
  {
    { "Block",               Block_Init,               "3.0" },
    { "Cloneface",           Cloneface_Init,           "2.0" },
    { "Dlist",               Dlist_Init,               "1.0" },
    { "Eventresponsehdlr",   Eventresponsehdlr_Init,   "3.0" },
    { "Exptdriver",          Exptdriver_Init,          "2.0" },
    { "Face",                Face_Init,                "2.0" },
    { "Fish",                Fish_Init,                "1.0" },
    { "Fixpt",               Fixpt_Init,               "2.0" },
    { "Gabor",               Gabor_Init,               "1.0" },
    { "Gaborarray",          Gaborarray_Init,          "1.0" },
    { "Genericevent",        Genericevent_Init,        "3.0" },
    { "Gl",                  Gl_Init,                  "1.0" },
    { "Gtrace",              Gtrace_Init,              "1.0" },
    { "Gxcolor",             Gxcolor_Init,             "1.0" },
    { "Gxcylinder",          Gxcylinder_Init,          "1.0" },
    { "Gxdisk",              Gxdisk_Init,              "1.0" },
    { "Gxdrawstyle",         Gxdrawstyle_Init,         "1.0" },
    { "Gxemptynode",         Gxemptynode_Init,         "1.0" },
    { "Gxfixedscalecamera",  Gxfixedscalecamera_Init,  "1.0" },
    { "Gxlighting",          Gxlighting_Init,          "1.0" },
    { "Gxline",              Gxline_Init,              "1.0" },
    { "Gxmaterial",          Gxmaterial_Init,          "1.0" },
    { "Gxnode",              Gxnode_Init,              "1.0" },
    { "Gxperspectivecamera", Gxperspectivecamera_Init, "1.0" },
    { "Gxpixmap",            Gxpixmap_Init,            "1.0" },
    { "Gxpointset",          Gxpointset_Init,          "1.0" },
    { "Gxpsyphycamera",      Gxpsyphycamera_Init,      "1.0" },
    { "Gxscaler",            Gxscaler_Init,            "1.0" },
    { "Gxseparator",         Gxseparator_Init,         "1.0" },
    { "Gxshapekit",          Gxshapekit_Init,          "1.0" },
    { "Gxsphere",            Gxsphere_Init,            "1.0" },
    { "Gxtext",              Gxtext_Init,              "1.0" },
    { "Gxtransform",         Gxtransform_Init,         "1.0" },
    { "Hook",                Hook_Init,                "1.0" },
    { "House",               House_Init,               "1.0" },
    { "Io",                  Io_Init,                  "1.0" },
    { "Jitter",              Jitter_Init,              "2.0" },
    { "Kbdresponsehdlr",     Kbdresponsehdlr_Init,     "3.0" },
    { "Maskhatch",           Maskhatch_Init,           "1.0" },
    { "Misc",                Misc_Init,                "1.0" },
    { "Morphyface",          Morphyface_Init,          "1.0" },
    { "Nullresponsehdlr",    Nullresponsehdlr_Init,    "3.0" },
    { "Obj",                 Obj_Init,                 "1.0" },
    { "Objdb",               Objdb_Init,               "1.0" },
    { "Prof",                Prof_Init,                "1.0" },
    { "Responsehandler",     Responsehandler_Init,     "3.0" },
    { "Serialrh",            Serialrh_Init,            "3.0" },
    { "Sound",               Sound_Init,               "1.0" },
    { "Timinghandler",       Timinghandler_Init,       "3.0" },
    { "Timinghdlr",          Timinghdlr_Init,          "3.0" },
    { "Tlist",               Tlist_Init,               "3.0" },
    { "Toglet",              Toglet_Init,              "1.0" },
    { "Trial",               Trial_Init,               "1.0" },
    { "Trialevent",          Trialevent_Init,          "3.0" },
  };

} // end anonymous namespace

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
                Debug::level = atoi(argv[i+1]);
            }
          else if (strcmp(argv[i], "-gtrace") == 0)
            {
              Util::Trace::setGlobalTrace(true);
            }
        }

      Tcl::Main app(argc, argv);

      if (Tcl::Main::isInteractive())
        {
          std::cerr << PACKAGE_STRING << " (" << __DATE__ << ")\n"
                    << "Copyright (c) 1998-2003 Rob Peters <rjpeters at klab dot caltech dot edu>\n"
                    << PACKAGE_NAME << " is free software, covered by the GNU General Public License, and you are\n"
                    << "welcome to change it and/or distribute copies of it under certain conditions.\n";
        }

      Tcl::Interp& interp = app.interp();

      const Util::Time ru1 = Util::Time::rusageUserNow();
      const Util::Time rs1 = Util::Time::rusageSysNow();
      const Util::Time wc1 = Util::Time::wallClockNow();

      for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
        {
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

      if (Tcl::Main::isInteractive())
        {
          const Util::Time ru = Util::Time::rusageUserNow() - ru1;
          const Util::Time rs = Util::Time::rusageSysNow() - rs1;
          const Util::Time wc = Util::Time::wallClockNow() - wc1;

          fprintf(stderr, "\tstartup time (tcl+tk) "
                  "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
                  ru.sec(), rs.sec(), wc.sec());
        }

      const Util::Time ru2 = Util::Time::rusageUserNow();
      const Util::Time rs2 = Util::Time::rusageSysNow();
      const Util::Time wc2 = Util::Time::wallClockNow();

      for (size_t i = 0; i < sizeof(DELAYED_PKGS)/sizeof(PackageInfo); ++i)
        {
          Tcl_StaticPackage((Tcl_Interp*) 0, // 0 means this package hasn't
                                             // yet been loaded into any
                                             // interpreter
                            DELAYED_PKGS[i].pkgName,
                            DELAYED_PKGS[i].pkgInitProc,
                            0);

          fstring ifneededcmd("package ifneeded ",
                              DELAYED_PKGS[i].pkgName,
                              " ",
                              DELAYED_PKGS[i].pkgVersion,
                              " {load {} ",
                              DELAYED_PKGS[i].pkgName,
                              " }");

          interp.eval(ifneededcmd);
        }

      for (size_t i = 0; i < sizeof(DELAYED_PKGS)/sizeof(PackageInfo); ++i)
        {
          const char* ver =
            Tcl_PkgRequire(interp.intp(),
                           DELAYED_PKGS[i].pkgName,
                           DELAYED_PKGS[i].pkgVersion,
                           0);

          if (ver == 0)
            {
              std::cerr << "'package require' failed: "
                        << DELAYED_PKGS[i].pkgName << '\n';
              fstring msg = interp.getResult<const char*>();
              if ( !msg.is_empty() )
                std::cerr << '\t' << msg << '\n';
              interp.resetResult();
            }
        }

      if (Tcl::Main::isInteractive())
        {
          const Util::Time ru = Util::Time::rusageUserNow() - ru2;
          const Util::Time rs = Util::Time::rusageSysNow() - rs2;
          const Util::Time wc = Util::Time::wallClockNow() - wc2;

          fprintf(stderr, "\tstartup time (GroovX) "
                  "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
                  ru.sec(), rs.sec(), wc.sec());
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
  return 1;
}

static const char vcid_grshAppInit_cc[] = "$Header$";
