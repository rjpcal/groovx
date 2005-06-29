///////////////////////////////////////////////////////////////////////
//
// tclscriptapp.cc
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun 27 13:34:19 2005
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

#ifndef GROOVX_TCL_SCRIPTAPP_CC_UTC20050628162421_DEFINED
#define GROOVX_TCL_SCRIPTAPP_CC_UTC20050628162421_DEFINED

#include "tcl/scriptapp.h"

#include "nub/objfactory.h"
#include "tcl/list.h"
#include "tcl/eventloop.h"
#include "tcl/pkg.h"
#include "tcl/interp.h"

#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <tk.h>

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

namespace
{

  bool havearg(char** args, const char* arg)
  {
    for ( ; *args != 0; ++args)
      if (strcmp(*args, arg) == 0)
        return true;

    return false;
  }

  std::string centerline(unsigned int totallen,
                         const char* pfx, const char* sfx,
                         std::string txt)
  {
    if (strlen(pfx) + strlen(sfx) >= totallen)
      {
        // ok, the line's too long, so don't do any centering, just
        // keep it as short as possible:
        std::string out(pfx);
        out += txt;
        out += sfx;
        return out;
      }

    unsigned int midlen = totallen - strlen(pfx) - strlen(sfx);

    std::string out(pfx);

    if (txt.length() < midlen)
      {
        int c = midlen - txt.length();
        while (--c >= 0)
          { if (c % 2) txt += ' '; else out += ' '; }
      }

    out += txt;
    out += sfx;

    return out;
  }

  std::string wrapstring(unsigned int totallen,
                         const char* pfx, const char* sfx,
                         const std::string& s)
  {
    GVX_ASSERT(strlen(pfx) + strlen(sfx) < totallen);
    unsigned int len = totallen - strlen(pfx) - strlen(sfx);

    std::istringstream strm(s);
    std::string out;
    std::string line;
    std::string word;
    while (strm >> word)
      {
        if (word.length() + line.length() + 1 <= len)
          {
            if (line.length() > 0)
              line += ' ';
            line += word;
          }
        else
          {
            out += line;
            out += '\n';
            line = word;
          }
      }

    out += line;

    return out;
  }

  std::string centerlines(unsigned int totallen,
                          const char* pfx, const char* sfx,
                          std::string ss)
  {
    if (ss.length() == 0)
      {
        return centerline(totallen, pfx, sfx, ss);
      }

    std::istringstream strm(ss);
    std::string line;
    std::string out;
    bool first = true;
    while (getline(strm, line))
      {
        if (!first) out += '\n';
        first = false;
        out += centerline(totallen, pfx, sfx, line);
      }

    return out;
  }

  std::string wrapcenterlines(unsigned int totallen,
                              const char* pfx, const char* sfx,
                              std::string ss, std::string emptyline)
  {
    std::istringstream strm(ss);
    std::string line;
    std::string out;
    while (getline(strm, line))
      {
        if (line.length() == 0)
          {
            out += emptyline;
            out += '\n';
          }
        else
          {
            out += centerlines(totallen, pfx, sfx,
                               wrapstring(totallen, pfx, sfx, line));
            out += '\n';
          }
      }

    return out;
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

  void sigHandler(int signum)
  {
    switch (signum)
      {
        case SIGSEGV: GVX_PANIC("Segmentation fault (SIGSEGV)");
        case SIGFPE:  GVX_PANIC("Floating point exception (SIGFPE)");
        case SIGBUS:  GVX_PANIC("Bus error (SIGBUS)");
      }
    GVX_ASSERT(0);
  }

}

void Tcl::ScriptApp::init_in_macro_only()
{
  signal(SIGSEGV, &sigHandler);
  signal(SIGFPE, &sigHandler);
  signal(SIGBUS, &sigHandler);
}

void Tcl::ScriptApp::handle_exception_in_macro_only
                                         (const std::exception* e)
{
  if (e != 0)
    std::cerr << "caught in main: ("
              << rutz::demangled_name(typeid(*e))
              << "): " << e->what() << '\n';
  else
    std::cerr << "caught in main: (an exception of unknown type)\n";
}

Tcl::ScriptApp::ScriptApp(const char* appname,
                          int argc_, char**argv_) throw()
  :
  appname(appname),
  argc(argc_),
  argv(argv_),
  minimal(havearg(argv_, "-minimal")),
  nowindow(havearg(argv_, "-nw")),
  splashmsg(),
  pkgdir(),
  pkgs(0),
  exitcode(0)
{
  // Quick check argv to optionally turn on global tracing and/or set
  // the global debug level. This method is particularly useful for
  // helping to diagnose problems that are occurring during
  // application startup, before we have a chance to get to the
  // command-line prompt and do a "::gtrace 1" or a "::dbgLevel 9".
  for (int i = 0; i < this->argc; ++i)
    {
      if (strcmp(this->argv[i], "-dbglevel") == 0)
        {
          if (this->argv[i+1] != 0)
            rutz::debug::set_global_level( atoi(this->argv[i+1]) );
        }
      else if (strcmp(this->argv[i], "-gtrace") == 0)
        {
          rutz::trace::set_global_trace(true);
        }
      else if (strcmp(this->argv[i], "-showinit") == 0)
        {
          Tcl::Pkg::verboseInit(true);
        }
    }
}

Tcl::ScriptApp::~ScriptApp() throw()
{}

void Tcl::ScriptApp::run()
{
  Tcl::Main tclmain(this->argc, this->argv, this->nowindow);

  if (Tcl::Main::isInteractive())
    {
      const char* const pfx = "###  ";
      const char* const sfx = "  ###";
      const unsigned int linelen = 75;
      std::string hashes(linelen, '#');

      std::cerr << hashes
                << '\n'
                << wrapcenterlines(linelen, pfx, sfx,
                                   splashmsg.c_str(), hashes)
                << hashes << '\n' << '\n';
    }

  Tcl::Interp& interp = tclmain.interp();

  Nub::ObjFactory::theOne().set_fallback(&factoryPkgLoader);
  Nub::setDefaultRefVis(Nub::PUBLIC);

  const rutz::time ru1 = rutz::time::user_rusage();
  const rutz::time rs1 = rutz::time::sys_rusage();
  const rutz::time wc1 = rutz::time::wall_clock_now();

  PackageInfo IMMEDIATE_PKGS[] =
    {
      { "Tcl",      Tcl_Init,  "", false },
      { "Tk",       Tk_Init,   "", true },
    };

  for (size_t i = 0; i < sizeof(IMMEDIATE_PKGS)/sizeof(PackageInfo); ++i)
    {
      if (nowindow && IMMEDIATE_PKGS[i].requiresGui)
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

          this->exitcode = 2; return;
        }
    }

  if (Tcl::Main::isInteractive())
    {
      const rutz::time ru = rutz::time::user_rusage() - ru1;
      const rutz::time rs = rutz::time::sys_rusage() - rs1;
      const rutz::time wc = rutz::time::wall_clock_now() - wc1;

      fprintf(stderr, "\tstartup time (%6s) "
              "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
              "tcl+tk", ru.sec(), rs.sec(), wc.sec());
    }

  const rutz::time ru2 = rutz::time::user_rusage();
  const rutz::time rs2 = rutz::time::sys_rusage();
  const rutz::time wc2 = rutz::time::wall_clock_now();

  for (const PackageInfo* pkg = pkgs; pkg->pkgName != 0; ++pkg)
    {
      Tcl_StaticPackage(static_cast<Tcl_Interp*>(0),
                        // (Tcl_Interp*) 0 means this package
                        // hasn't yet been loaded into any
                        // interpreter
                        pkg->pkgName,
                        pkg->pkgInitProc,
                        0);

      rutz::fstring ifneededcmd("package ifneeded ",
                                pkg->pkgName, " ", pkg->pkgVersion,
                                " {load {} ", pkg->pkgName, " }");

      interp.eval(ifneededcmd);
    }

  if (!minimal)
    {
      for (const PackageInfo* pkg = pkgs; pkg->pkgName != 0; ++pkg)
        {
          if (nowindow && pkg->requiresGui)
            continue;

          const char* ver =
            Tcl_PkgRequire(interp.intp(),
                           pkg->pkgName,
                           pkg->pkgVersion,
                           0);

          if (ver == 0)
            {
              std::cerr << "initialization error (package '"
                        << pkg->pkgName << "'):\n";
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

      fprintf(stderr, "\tstartup time (%6s) "
              "%6.3fs (user) %6.3fs (sys) %6.3fs (wall)\n",
              appname.c_str(), ru.sec(), rs.sec(), wc.sec());
    }

  Tcl::List path = interp.getGlobalVar<Tcl::List>("auto_path");

  if (pkgdir.length() > 0)
    path.append(pkgdir);

  interp.setGlobalVar("auto_path", path.asObj());

  // specifies a file to be 'source'd upon startup
  interp.setGlobalVar("tcl_rcFileName",
                      Tcl::toTcl("./groovx_startup.tcl"));

  tclmain.run();
}

static const char vcid_groovx_tcl_scriptapp_cc_utc20050628162421[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_SCRIPTAPP_CC_UTC20050628162421_DEFINED
