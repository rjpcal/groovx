-*- text -*-

GroovX - A framework for visual experiments
===========================================

Copyright (c) 1998-2004 California Institute of Technology
Copyright (c) 2004-2007 University of Southern California
Rob Peters

README GroovX readme file
-------------------------

See also the TUTORIAL file for more getting-started help.

Table of contents:

- [overview](#1-overview)
- [files](#2-organization-of-the-files)
- [dependencies](#3-dependencies)
- [configuration](#4-configuration)
- [building](#5-building)
- [portability](#6-portability)

-----------------------------------------------------------

1. Overview
-----------

   This is a C++ toolkit for writing visual psychophysics
   experiments. Various abstractions relating to experiments (visual
   objects, trials, observer responses, etc.) are implemented in C++,
   and are exposed through a Tcl interface as scriptable objects.

   Note that sitting beside this README file is a TUTORIAL
   file which should contain some useful examples of how to get
   started actually using the program.

-----------------------------------------------------------

2. Organization of the files
----------------------------

   The `src/` directory contains subdirectories for each of the various
   components of the system. Packages that are configured to be
   loadable at run-time are found in `src/pkgs/`.

   When the system is built, the object files go into a hierarchy
   within `build/obj/` that parallels `src/`. That is, a source file
   `src/foo/bar.cc` will be compiled to `build/obj/foo/bar.o`.

-----------------------------------------------------------

3. Dependencies
---------------

   - Assume `/path/to/install` is where you will install local packages
   - Assume `/path/to/source` is where you unpack and build these packages


   This software requires these development packages:

   - Tcl/Tk (version 8.5 or later)
   - X11 windowing system OR Mac OS X's Aqua windowing system
   - OpenGL (3-D graphics library)
   - libz (gzip library)
   - libexpat (XML parsing library)


   The following additional packages are optional. The configure
   script should generally be able to detect whether you have them
   installed or not and set up the Makefile appropriately.

   - libpng (portable network graphics library)
   - libjpeg (jpeg image handling library)
   - matlab (allows running a matlab engine inside the groovx shell)
   - libesd+libaudiofile (for simple sound file playback on Linux)
   - Quicktime (for simple sound file playback on Mac OS X)
   - libreadline (for command-line editing and history)


   The following packages are optional, but are used in some of the
   scripts that use the groovx software.

   - itcl and itk (itcl3.2.1_src.tgz)

     Note that the itcl+itk configure script expects to find the
     tcl+tk sources sitting in the location where you built
     them... so, don't remove the tcl+tk sources until after you
     build and install itcl+itk.

     ```bash
     cd /path/to/source
     tar xfz itcl3.2.1_src.tgz
     cd itcl3.2.1
     ./configure  --prefix=/path/to/install
     make
     make install
     ```

     Note, you might need to do this:

     ```bash
            export SHLIB_LDFLAGS=-L/path/to/install/lib
     ```

     before running ./configure so that the configuration script
     looks for e.g. libtclstub8.x.a in the install directory rather
     than in the tcl's apparent build directory.


   - iwidgets (iwidgets4.0.1.tar.gz)

     Again, note that iwidgets expects to still be able to find the
     source code from the itcl/itk build, so don't remove the
     itcl+itk sources until after you've configured and installed
     iwidgets.

     ```bash
     cd /path/to/source
     tar xfz iwidgets4.0.1.tar.gz
     cd iwidgets4.0.1
     ./configure  --prefix=/path/to/install \
            --with-itcl=/path/to/source/itcl3.2.1 \
            --with-itk=/path/to/source/itcl3.2.1
     ```

     Then, there is no need to do `make` for iwidgets; just do `make
     install`:

     ```bash
     make install
     ```

   - Mac OS X Tcl/Tk build notes

     Notes for doing a custom Tcl/Tk build for Aqua (i.e. not for
     X11 on Mac OS X).

     Used the following Makefile fragment for Tcl (in
     pathtosrc/tcl/macosx/Makefile):

     ```bash
     DESTDIR                 ?=
     INSTALL_ROOT            ?= ${DESTDIR}

     BUILD_DIR               ?= ${CURDIR}/../../build
     SYMROOT                 ?= ${BUILD_DIR}/${PROJECT}
     OBJROOT                 ?= ${SYMROOT}

     EXTRA_CONFIGURE_ARGS    ?=
     EXTRA_MAKE_ARGS         ?=

     INSTALL_PATH            ?= ${HOME}/Library/Frameworks
     PREFIX                  ?= ${HOME}/local/tcl8.5a1-aqua
     BINDIR                  ?= ${PREFIX}/bin
     MANDIR                  ?= ${PREFIX}/man
     ```

     Used the following Makefile fragment for Tk (in
     pathtosrc/tk/macosx/Makefile):

     ```bash
     DESTDIR                 ?=
     INSTALL_ROOT            ?= ${DESTDIR}

     BUILD_DIR               ?= ${CURDIR}/../../build
     SYMROOT                 ?= ${BUILD_DIR}/${PROJECT}
     OBJROOT                 ?= ${SYMROOT}

     EXTRA_MAKE_ARGS         ?=

     INSTALL_PATH            ?= ${HOME}/Library/Frameworks
     APPLICATION_INSTALL_PATH ?= ${HOME}/Applications/Utilities

     PREFIX                  ?= ${HOME}/local/tcl8.5a1-aqua
     BINDIR                  ?= ${PREFIX}/bin

     TCL_FRAMEWORK_DIR       ?= ${BUILD_DIR}/tcl
     TCLSH_DIR               ?= ${TCL_FRAMEWORK_DIR}
     ```

     Problems:

     - Tk install requires 'chmod' with '-RH' options. But, the
       Fink version of chmod in /sw/bin/chmod doesn't recognize
   	  the '-H' option. So, need to point the installation to
   	  /bin/chmod instead. For now, just put a link from
   	  ~/local/bin/chmod to /bin/chmod, since ~/local/bin is
   	  higher in $PATH than is /sw/bin.

     - Tk install expects to find 'Wish Shell.app' in
       ~/Library/Frameworks/Tk.Frame/Versions/8.5/Resources/. But,
       the Makefile fragment above puts 'Wish Shell.app' in
       ~/Applications/Utilities instead. So, for now just do:

       ```bash
       ln -s ~/Applications/Utilities/Wish\ Shell.app \
               ~/Library/Frameworks/Tk.framework/Versions/8.5/Resources/
       ```

     - Problem with how tk.dylib gets built by default. See http://ewen.mcneill.gen.nz/blog/entry/2012-12-28-os-x-dylib-care-and-feeding/

       Need to patch tk's unix/Makefile.in before
   	  building+installing tk like this:

       ```Diff
       --- Makefile.in~	2016-07-26 07:07:50.000000000 -0700
       +++ Makefile.in		2016-10-15 20:59:54.000000000 -0700
       @@ -283,7 +283,7 @@
        LD_SEARCH_FLAGS = @LD_SEARCH_FLAGS@

        # support for embedded libraries on Darwin / Mac OS X
       -DYLIB_INSTALL_DIR	= ${LIB_RUNTIME_DIR}
       +DYLIB_INSTALL_DIR	= $(libdir)

        # support for building the Aqua resource file
        TK_RSRC_FILE  		= @TK_RSRC_FILE@
       ```


-----------------------------------------------------------

4. Configuration
----------------

   Configuration of GroovX is done with a standard autoconf-generated
   configure script. To run the script with the default options, just
   do:

   ```bash
       ./configure
   ```

   To see a description of available configuration options, do:

   ```bash
       ./configure --help
   ```

   Some important configuration options:

   * --enable-debug={no|yes} [default is yes]

     determines whether to include runtime debuggability
     into the executable. If 'yes' (the default), then certain
     commands will be available in the shell to control debug
     verbosity and stack tracing.

   * --enable-aqua={no|yes} [default is no]

     determines whether to use Aqua windows or X11 windows
     on a Mac OS X build; default is X11

   * --enable-readline={no|yes} [default is yes]

     hether to enable readline support, which allows for
     command-line editing and a command history

   * --enable-rgba={no|yes} [default is yes]

     whether to run OpenGL graphics in rgba true-color mode
     (instead of color-index mode)

   * --enable-double-buffer={no|yes} [default is yes]

     whether to run OpenGL graphics in double-buffered mode
     (instead of single-buffer mode); double-buffering allows
     for smoother screen redraws with less flicker and "tearing"

   * --enable-direct-render={no|yes} [default is yes]

     whether to run OpenGL graphics in a direct-rendering
     context; this means that 3-D graphics calls will use the
     hardware acceleration of the grapics card, if possible

   * --enable-werror={no|yes} [default is yes]

     whether to force all compiler warnings to be errors

   * --with-matlab=/path/to/matlab [default is /usr/local/matlab]

     give the location of a matlab installation against
     which some matlab interfaces can be built; if no matlab
     directory is given, then only stub matlab interfaces will
     be set up... the code will compile, but attempts to use a
     matlab interface will trigger a run-time error

   * --with-html=/path/to/html [default is ~/www/groovx/]

     specifies where HTML documentation should be installed
     during a "make docs" invocation

-----------------------------------------------------------

5. Building
-----------

   Once you have all package dependencies installed, and run the
   configure script, just type "make" and everything should build and
   install. A short test suite (takes ~10 seconds on a 1GHz Pentium
   III Linux machine) is run at the end of every "make" invocation.

   - environment variables

     * LD_LIBRARY_PATH

       On Linux, you need to make sure that your LD_LIBRARY_PATH
       environment variable includes the paths to wherever you have
       installed the libraries associated with this package; e.g. in
       bash:

       ```bash
       export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/path/to/install"
       ```

-----------------------------------------------------------

6. Portability
--------------

   This software has been built and run successfully under:

   - RedHat Linux 7.x (i686)
   - RedHat Linux 8.x (i686)
   - RedHat Linux 9.x (i686)
   - Fedore Core 1 (i686)
   - Fedore Core 2 (i686)
   - Fedore Core 3 (i686)
   - YellowDog Linux 3.0.1 (ppc)
   - Mac OS X 10.1.x
   - Mac OS X 10.2.x
   - Mac OS X 10.3.x


   Note that for Mac OS X, you can build either for X11 (if you are
   running an X server such as OroborosX on your OSX box) or for Aqua
   (OSX's native windowing system). By default the configure script
   will setup for X11, but you can pass the --enable-aqua option to
   tell it to set up for Aqua instead.

   Earlier versions of the software have been built successfully under
   SGI's IRIX6 and HP's HPUX 10.x, but these configurations have not
   been tested recently.

   The software requires a reasonably C++ std-compliant
   compiler. Compilers that have worked successfully in the past
   include:

   - g++ 2.95.x (linux, Mac OS X)
   - g++ 3.0.x (linux)
   - g++ 3.1.x (linux, Mac OS X)
   - g++ 3.2.x (linux)
   - g++ 3.3.x (linux, Mac OS X)
   - g++ 3.4.x (linux)
   - g++ 4.0-prerelease
   - HP's aCC (hpux)
   - Intel's icc 6.x/7.x/8.x (linux)
   - SGI's MIPSpro (irix6)


   but note that the older compilers (especially g++ 2.95.x, HP aCC,
   and SGI MIPSpro) haven't been tested lately so it's likely that a
   few things will not work out-of-the-box with those compilers.
