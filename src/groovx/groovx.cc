/** @file groovx/groovx.cc main() for groovx script interpreter for
    psychophysics experiments */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2005
// Rob Peters <rjpeters at usc dot edu>
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

#include "rutz/prof.h"
#include "tcl/scriptapp.h"

// #include's for forward decls of all the *_Init() procedures

#include "tcl-gfx/tclpkg-canvas.h"                   // for Canvas_Init(), Glcanvas_Init(),
#include "tcl-gfx/tclpkg-gx.h"                       // for Gx_Init(), Gxnode_Init(), Gxseparator_Init(), Gxcolor_Init(), Gxdrawstyle_Init(), Gxline_Init(), Gxcylinder_Init(), Gxsphere_Init(), Gxlighting_Init(), Gxmaterial_Init(), Gxpointset_Init(), Gxscaler_Init(), Gxemptynode_Init(), Gxtransform_Init(), Gxshapekit_Init(), Gxpixmap_Init(), Gxtext_Init(), Gxfixedscalecamera_Init(), Gxpsyphycamera_Init(), Gxperspectivecamera_Init(), Gxdisk_Init(),
#include "tcl-gfx/tclpkg-toglet.h"                   // for Toglet_Init(),
#include "tcl-io/tclpkg-io.h"                        // for Io_Init(), Outputfile_Init(),
#include "tcl/tclpkg-dlist.h"                    // for Dlist_Init(),
#include "tcl/tclpkg-gtrace.h"                   // for Gtrace_Init(), Prof_Init(),
#include "tcl/tclpkg-log.h"                      // for Log_Init(),
#include "tcl/tclpkg-misc.h"                     // for Misc_Init(),
#include "tcl/tclpkg-obj.h"                      // for Objectdb_Init(), Obj_Init()
#include "tk/tclpkg-widget.h"                    // for Tkwidget_Init(),
#include "visx/tclpkg-block.h"                   // for Block_Init(),
#include "visx/tclpkg-elementcontainer.h"        // for Elementcontainer_Init(),
#include "visx/tclpkg-element.h"                 // for Element_Init(),
#include "visx/tclpkg-expt.h"                    // for Exptdriver_Init(),
#include "visx/tclpkg-face.h"                    // for Face_Init(), Cloneface_Init(),
#include "visx/tclpkg-fish.h"                    // for Fish_Init(),
#include "visx/tclpkg-fixpt.h"                   // for Fixpt_Init(),
#include "visx/tclpkg-gabor.h"                   // for Gabor_Init(), Gaborarray_Init(),
#include "visx/tclpkg-gl.h"                      // for Gl_Init(),
#include "visx/tclpkg-hook.h"                    // for Hook_Init(),
#include "visx/tclpkg-house.h"                   // for House_Init(),
#include "visx/tclpkg-jitter.h"                  // for Jitter_Init(),
#include "visx/tclpkg-mask.h"                    // for Maskhatch_Init(),
#include "visx/tclpkg-morphyface.h"              // for Morphyface_Init(),
#include "visx/tclpkg-rh.h"                      // for Responsehandler_Init(), Eventresponsehdlr_Init(), Kbdresponsehdlr_Init(), Nullresponsehdlr_Init(), Serialrh_Init(),
#include "visx/tclpkg-sound.h"                   // for Sound_Init(),
#include "visx/tclpkg-th.h"                      // for Timinghdlr_Init(), Timinghandler_Init(),
#include "visx/tclpkg-tlist.h"                   // for Tlist_Init(),
#include "visx/tclpkg-trialevent.h"              // for Trialevent_Init(), Nulltrialevent_Init(), Filewriteevent_Init(), Genericevent_Init(), Multievent_Init(),
#include "visx/tclpkg-trial.h"                   // for Trial_Init(),

//
// Info about the packages to be loaded
//

static tcl::package_info GROOVX_PKGS[] =
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
    { "Objectdb",            Objectdb_Init,            "4.0", false },
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
    // WARNING! Keep this entry last
    { 0, 0, 0, 0 }
  };

///////////////////////////////////////////////////////////////////////
//
// main()
//
///////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  GVX_SCRIPT_PROG_BEGIN(app, "GroovX", argc, argv);

  rutz::prof::print_at_exit(true);

  app.pkg_dir(GVX_PKG_LIB_DIR);

  app.splash(PACKAGE_STRING " (" __DATE__ ")\n"
             "\n"
             "Copyright (c) 1998-2005 Rob Peters\n"
             "<http://ilab.usc.edu/rjpeters/groovx/>\n"
             PACKAGE_NAME " is free software, covered by "
             "the GNU General Public License, and you are "
             "welcome to change it and/or distribute copies "
             "of it under certain conditions.\n");

  app.packages(GROOVX_PKGS);

  app.run();

  GVX_SCRIPT_PROG_END(app);
}

static const char vcid_groovx_groovx_groovx_cc_utc20050626084019[] = "$Id$ $HeadURL$";
