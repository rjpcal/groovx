##########################################################################
#
# Makefile 
#
#
# This is the Makefile for the grsh shell. This shell provides the
# following functionality:
# 
# 	1) Tcl/Tk 8.2.1 core
# 	2) OpenGL graphics, via the Togl widget
# 	3) A suite of major extensions that allow creation and execution of
# 		arbitrary visual psychophysics experiments
# 
#
# $Id$
#
##########################################################################

VERSION := 0.8a4
EXTRA_STATISTICS := 0

TCLTK_VERSION := 8.2

#-------------------------------------------------------------------------
#
# Platform selection
#
#-------------------------------------------------------------------------

ETAGS := etags

ifeq ($(ARCH),hp9000s700)
	PLATFORM := hp9000s700
	COMPILER := aCC
	SHLIB_EXT := sl
	STATLIB_EXT := a
	DEFAULT_TARGET := testsh
	ETAGS := echo "no etags"
endif
ifeq ($(ARCH),irix6)
	PLATFORM := irix6
	COMPILER := MIPSpro
	SHLIB_EXT := so
	STATLIB_EXT := a
	DEFAULT_TARGET := grsh
endif

#-------------------------------------------------------------------------
#
# Directories for different file types
#
#-------------------------------------------------------------------------

PROJECT = $(HOME)/sorcery/grsh
SRC := src
DEP := ./dep/$(ARCH)
OBJ := ./obj/$(PLATFORM)
LIB := $(PROJECT)/lib/$(PLATFORM)
LOG := ./logs
DOC := ./doc
IDEP := ./idep
SCRIPTS := ./scripts

LOCAL_ARCH := $(HOME)/local/$(ARCH)

BIN_DIR := $(LOCAL_ARCH)/bin

LOCAL_LIB_DIR := $(LOCAL_ARCH)/lib

#-------------------------------------------------------------------------
#
# C++ Compiler options for compiling and linking
#
#-------------------------------------------------------------------------

ifeq ($(COMPILER),aCC)
	CC := time aCC
#	FILTER := 
	FILTER := |& sed -e '/Warning.*opt.aCC.include./,/\^\^*/d' \
		-e '/Warning.*usr.include./,/\^\^*/d'
# 361 == wrongly complains about falling off end of non-void function
# 392 == 'Conversion unnecessary; expression was already of type...'
	COMPILER_SWITCHES := +w +W361,392
	ARCH_CPP_DEFINES := -DACC_COMPILER -DHP9000S700 -DPRESTANDARD_IOSTREAMS \
		-Dstd= -DSTD_IO= -DHAVE_ZSTREAM
	ARCH_MAKEDEP_INCLUDES := -I/opt/aCC/include -I/usr -I/opt/aCC/include/iostream \
		-I/opt/graphics/OpenGL/include -I./scripts/spoofdep
	STL_INCLUDE_DIR := 

	DEBUG_OPTIONS := -g1 +Z +p
	DEBUG_LINK_OPTIONS := -Wl,-B,immediate -Wl,+vallcompatwarnings

	PROD_OPTIONS := +O2 +Z +p
	PROD_LINK_OPTIONS := -Wl,+vallcompatwarnings

	DEBUGLIB_EXT := $(SHLIB_EXT)
	PRODLIB_EXT := $(SHLIB_EXT)

	SHLIB_CMD := $(CC) -b -o
	STATLIB_CMD := ar rus
endif

ifeq ($(COMPILER),MIPSpro)
	CC := time /opt/MIPSpro/bin/CC
	FILTER := 
	COMPILER_SWITCHES := -n32 -ptused -no_prelink \
		-no_auto_include -LANG:std -LANG:exceptions=ON 
	ARCH_CPP_DEFINES := -DMIPSPRO_COMPILER -DIRIX6 -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	ARCH_MAKEDEP_INCLUDES := -I/usr/include/CC \
		-I/cit/rjpeters/include/cppheaders

	STL_INCLUDE_DIR := -I$(HOME)/include/cppheaders

	DEBUG_OPTIONS := -g -O0
	DEBUG_LINK_OPTIONS :=

# Tests showed that -O3 provided little improvement over -O2 for this app
	PROD_OPTIONS := -O2
	PROD_LINK_OPTIONS :=

	DEBUGLIB_EXT := $(SHLIB_EXT)
	PRODLIB_EXT := $(SHLIB_EXT)

	SHLIB_CMD := $(CC) -shared -Wl,-check_registry,/usr/lib32/so_locations -o
	STATLIB_CMD := $(CC) -ar -o
endif

ifeq ($(COMPILER),g++)
	CC := time /cit/rjpeters/gcc-2.95.1/bin/g++
# This filter removes warnings that are triggered by standard library files
	FILTER := |& sed \
		-e '/g++-3.*warning/d;' \
		-e '/In file included.*g++-3/,/:/d;' \
		-e '/g++-3.*In method/d;' \
		-e '/g++-3.*At top level/d;' \
		-e '/g++-3.*In instantiation of/,/instantiated from here/d'
	COMPILER_SWITCHES := -Wall -W -Wsign-promo -Weffc++
	ARCH_CPP_DEFINES := -DGCC_COMPILER -DIRIX6

	ARCH_MAKEDEP_INCLUDES := -I/cit/rjpeters/gcc/include/g++-3

	STL_INCLUDE_DIR := -I$(HOME)/gcc/include/g++-3

	DEBUG_OPTIONS := -g -O0
	DEBUG_LINK_OPTIONS :=

	PROD_OPTIONS := -O3
	PROD_LINK_OPTIONS :=

	DEBUGLIB_EXT := $(STATLIB_EXT)
	PRODLIB_EXT := $(STATLIB_EXT)

	SHLIB_CMD := ld -n32 -shared -check_registry /usr/lib32/so_locations
	STATLIB_CMD := ar rus
endif

CPP_DEFINES := $(ARCH_CPP_DEFINES)
MAKEDEP_INCLUDES := $(ARCH_MAKEDEP_INCLUDES) -I./scripts/spoofdep

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

MY_INCLUDE_PATH := -I$(LOCAL_ARCH)/include -I$(HOME)/include -I$(SRC) \
	$(STL_INCLUDE_DIR)

ifeq ($(ARCH),hp9000s700)
	AUDIO_LIB := -lAlib
	ZSTREAM_LIB := -lzstream

	OPENGL_LIB_DIR := -L/opt/graphics/OpenGL/lib
	AUDIO_LIB_DIR := -L/opt/audio/lib
	RPATH_DIR := 
endif
ifeq ($(ARCH),irix6)
	AUDIO_LIB := -laudio -laudiofile
	ZSTREAM_LIB := 

	OPENGL_LIB_DIR :=
	AUDIO_LIB_DIR :=
	RPATH_DIR := -Wl,-rpath,$(LIB)
endif

MY_LIB_PATH :=  -L$(LIB) \
	-L$(LOCAL_LIB_DIR) \
	$(OPENGL_LIB_DIR) \
	$(AUDIO_LIB_DIR) \
	$(RPATH_DIR)

LIBRARIES := \
	-lGLU -lGL \
	-ltk -ltcl \
	-lXmu -lX11 -lXext \
	$(ZSTREAM_LIB) \
	$(AUDIO_LIB) \
	-lm

#-------------------------------------------------------------------------
#
# List of object files
#
#-------------------------------------------------------------------------

GRSH_MAIN_OBJ := $(OBJ)/grshAppInit.do

GRSH_STATIC_OBJS := \
	$(OBJ)/bitmap.do \
	$(OBJ)/bitmaprep.do \
	$(OBJ)/face.do \
	$(OBJ)/fish.do \
	$(OBJ)/fixpt.do \
	$(OBJ)/gabor.do \
	$(OBJ)/glbitmap.do \
	$(OBJ)/glcanvas.do \
	$(OBJ)/glbmaprenderer.do \
	$(OBJ)/grobjimpl.do \
	$(OBJ)/gtext.do \
	$(OBJ)/house.do \
	$(OBJ)/jitter.do \
	$(OBJ)/maskhatch.do \
	$(OBJ)/morphyface.do \
	$(OBJ)/objtogl.do \
	$(OBJ)/position.do \
	$(OBJ)/tclgl.do \
	$(OBJ)/tlistwidget.do \
	$(OBJ)/togl/togl.do \
	$(OBJ)/toglconfig.do \
	$(OBJ)/xbitmap.do \
	$(OBJ)/xbmaprenderer.do \

# My intent is that libvisx should contain the core classes for the
# experiment framework, and that libgrsh should contain the
# application-specific subclasses (like Face, House, etc.). But there
# are some bizarre problems on HPUX that give crashes from OpenGL,
# which are very sensitive to the link order. So for now, all the
# GL-related objects go into libgrsh, and everything else into libvisx.

# Ah, well, now the HP-OpenGL bug is rearing its ugly head again, so
# I'll have to go back to statically linking the OpenGL stuff.

GRSH_DYNAMIC_OBJS := \


VISX_OBJS := \
	$(OBJ)/bitmaptcl.do \
	$(OBJ)/block.do \
	$(OBJ)/blocktcl.do \
	$(OBJ)/bmapdata.do \
	$(OBJ)/bmaprenderer.do \
	$(OBJ)/cloneface.do \
	$(OBJ)/eventresponsehdlr.do \
	$(OBJ)/experiment.do \
	$(OBJ)/exptdriver.do \
	$(OBJ)/expttcl.do \
	$(OBJ)/expttesttcl.do \
	$(OBJ)/facetcl.do \
	$(OBJ)/fishtcl.do \
	$(OBJ)/fixpttcl.do \
	$(OBJ)/gabortcl.do \
	$(OBJ)/grobj.do \
	$(OBJ)/grobjtcl.do \
	$(OBJ)/grshapp.do \
	$(OBJ)/gtexttcl.do \
	$(OBJ)/housetcl.do \
	$(OBJ)/jittertcl.do \
	$(OBJ)/kbdresponsehdlr.do \
	$(OBJ)/masktcl.do \
	$(OBJ)/morphyfacetcl.do \
	$(OBJ)/nullresponsehdlr.do \
	$(OBJ)/objlisttcl.do \
	$(OBJ)/pbm.do \
	$(OBJ)/positiontcl.do \
	$(OBJ)/poslisttcl.do \
	$(OBJ)/response.do \
	$(OBJ)/responsehandler.do \
	$(OBJ)/rhtcl.do \
	$(OBJ)/sound.do \
	$(OBJ)/soundtcl.do \
	$(OBJ)/subject.do \
	$(OBJ)/subjecttcl.do \
	$(OBJ)/thtcl.do \
	$(OBJ)/timinghandler.do \
	$(OBJ)/timinghdlr.do \
	$(OBJ)/tlisttcl.do \
	$(OBJ)/tlistutils.do \
	$(OBJ)/trial.do \
	$(OBJ)/trialbase.do \
	$(OBJ)/trialevent.do \
	$(OBJ)/trialtcl.do \

APPWORKS_OBJS := \
	$(OBJ)/application.do \
	$(OBJ)/factory.do \
	$(OBJ)/iditemutils.do \
	$(OBJ)/ioptrlist.do \
	$(OBJ)/gwt/canvas.do \
	$(OBJ)/gwt/widget.do \
	$(OBJ)/io/asciistreamreader.do \
	$(OBJ)/io/asciistreamwriter.do \
	$(OBJ)/io/io.do \
	$(OBJ)/io/iofactory.do \
	$(OBJ)/io/iolegacy.do \
	$(OBJ)/io/iomap.do \
	$(OBJ)/io/iomgr.do \
	$(OBJ)/io/property.do \
	$(OBJ)/io/reader.do \
	$(OBJ)/io/readutils.do \
	$(OBJ)/io/writer.do \
	$(OBJ)/io/writeutils.do \
	$(OBJ)/system/demangle.do \
	$(OBJ)/system/system.do \
	$(OBJ)/util/debug.do \
	$(OBJ)/util/error.do \
	$(OBJ)/util/errorhandler.do \
	$(OBJ)/util/observable.do \
	$(OBJ)/util/observer.do \
	$(OBJ)/util/ptrhandle.do \
	$(OBJ)/util/refcounted.do \
	$(OBJ)/util/serialport.do \
	$(OBJ)/util/strings.do \
	$(OBJ)/util/trace.do \
	$(OBJ)/util/value.do \


TCLWORKS_OBJS := \
	$(OBJ)/tcl/listpkg.do \
	$(OBJ)/tcl/misctcl.do \
	$(OBJ)/tcl/stringifycmd.do \
	$(OBJ)/tcl/tclcmd.do \
	$(OBJ)/tcl/tcldlist.do \
	$(OBJ)/tcl/tclerror.do \
	$(OBJ)/tcl/tclitempkg.do \
	$(OBJ)/tcl/tclitempkgbase.do \
	$(OBJ)/tcl/tclpkg.do \
	$(OBJ)/tcl/tclutil.do \
	$(OBJ)/tcl/tclvalue.do \
	$(OBJ)/tcl/tclveccmds.do \
	$(OBJ)/tcl/tracertcl.do \


#-------------------------------------------------------------------------
#
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

DEBUG_TARGET := $(BIN_DIR)/testsh

DEBUG_DEFS := -DPROF -DASSERT -DINVARIANT -DTEST

DEBUG_GRSH_MAIN_OBJ := $(GRSH_MAIN_OBJ)
DEBUG_GRSH_STATIC_OBJS := $(GRSH_STATIC_OBJS)
DEBUG_GRSH_DYNAMIC_OBJS := $(GRSH_DYNAMIC_OBJS)
DEBUG_VISX_OBJS := $(VISX_OBJS)
DEBUG_TCLWORKS_OBJS := $(TCLWORKS_OBJS)
DEBUG_APPWORKS_OBJS := $(APPWORKS_OBJS)

DEBUG_LIBGRSH := $(LIB)/libgrsh.d.$(DEBUGLIB_EXT)
DEBUG_LIBVISX := $(LIB)/libvisx.d.$(DEBUGLIB_EXT)
DEBUG_LIBTCLWORKS := $(LIB)/libtclworks.d.$(DEBUGLIB_EXT)
DEBUG_LIBAPPWORKS := $(LIB)/libappworks.d.$(DEBUGLIB_EXT)

ALL_DEBUG_LIBS := $(DEBUG_LIBVISX) $(DEBUG_LIBTCLWORKS) $(DEBUG_LIBAPPWORKS)

ALL_DEBUG_STATLIBS := $(filter %.$(STATLIB_EXT),$(ALL_DEBUG_LIBS))
ALL_DEBUG_SHLIBS   := $(filter %.$(SHLIB_EXT),$(ALL_DEBUG_LIBS))

DEBUG_AUX_OBJ :=
ifeq ($(ARCH),hp9000s700)
	DEBUG_AUX_OBJ := /opt/langtools/lib/end.o
endif

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET := $(BIN_DIR)/grsh$(VERSION)

PROD_DEFS := -DASSERT -DINVARIANT

PROD_GRSH_MAIN_OBJ := $(GRSH_MAIN_OBJ:.do=.o)
PROD_GRSH_STATIC_OBJS := $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
#PROD_GRSH_DYNAMIC_OBJS := $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_VISX_OBJS := $(DEBUG_VISX_OBJS:.do=.o)
PROD_TCLWORKS_OBJS := $(DEBUG_TCLWORKS_OBJS:.do=.o)
PROD_APPWORKS_OBJS := $(DEBUG_APPWORKS_OBJS:.do=.o)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.

#PROD_LIBGRSH := $(LIB)/libgrsh$(VERSION).$(PRODLIB_EXT)
PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(PRODLIB_EXT)
PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(PRODLIB_EXT)
PROD_LIBAPPWORKS := $(LIB)/libappworks$(VERSION).$(PRODLIB_EXT)

ALL_PROD_LIBS := $(PROD_LIBVISX) $(PROD_LIBTCLWORKS) $(PROD_LIBAPPWORKS)

ALL_PROD_STATLIBS := $(filter %.$(STATLIB_EXT),$(ALL_PROD_LIBS))
ALL_PROD_SHLIBS   := $(filter %.$(SHLIB_EXT),$(ALL_PROD_LIBS))

#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS := $(COMPILER_SWITCHES) $(CPP_DEFINES) $(MY_INCLUDE_PATH)

ALL_PROD_OPTIONS := $(COMMON_OPTIONS) $(PROD_OPTIONS) $(PROD_DEFS)
ALL_DEBUG_OPTIONS := $(COMMON_OPTIONS) $(DEBUG_OPTIONS) $(DEBUG_DEFS)

$(OBJ)/%.o : $(SRC)/%.cc
	echo $< >> $(LOG)/CompileStats
	csh -fc "($(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)) $(FILTER)"

$(OBJ)/%.do : $(SRC)/%.cc
ifeq ($(EXTRA_STATISTICS),1)
	$(CC) -E $< $(ALL_DEBUG_OPTIONS) > .temp.cc
	wc -l $<
	wc -l .temp.cc
endif
	echo $< >> $(LOG)/CompileStats
	csh -fc "($(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)) $(FILTER)"

$(SRC)/%.precc : $(SRC)/%.cc
	$(CC) -E $< $(ALL_DEBUG_OPTIONS) > $@

$(SRC)/%.preh : $(SRC)/%.h
	echo "#include \"$<\"" > .temp.cc
	$(CC) -E .temp.cc $(ALL_DEBUG_OPTIONS) > $@

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

default: $(DEFAULT_TARGET)

testsh: $(SRC)/TAGS $(ALL_DEBUG_SHLIBS) $(DEBUG_TARGET)
	$(DEBUG_TARGET) ./testing/grshtest.tcl

DEBUG_CMDLINE := $(DEBUG_LINK_OPTIONS) $(DEBUG_GRSH_STATIC_OBJS) \
	 $(DEBUG_GRSH_MAIN_OBJ) $(DEBUG_AUX_OBJ) \
	$(MY_LIB_PATH) -lvisx.d -ltclworks.d -lappworks.d $(LIBRARIES) 

$(DEBUG_TARGET): $(DEBUG_GRSH_MAIN_OBJ) \
		$(DEBUG_GRSH_STATIC_OBJS) $(ALL_DEBUG_STATLIBS)
	$(CC) -o $@ $(DEBUG_CMDLINE)

grsh: $(SRC)/TAGS $(ALL_PROD_SHLIBS) $(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

PROD_CMDLINE := $(PROD_LINK_OPTIONS) $(PROD_GRSH_STATIC_OBJS) \
	$(PROD_GRSH_MAIN_OBJ) $(MY_LIB_PATH) \
	-lvisx$(VERSION) -ltclworks$(VERSION) -lappworks$(VERSION) \
	$(LIBRARIES) \

$(PROD_TARGET): $(PROD_GRSH_MAIN_OBJ) \
		$(PROD_GRSH_STATIC_OBJS) $(ALL_PROD_STATLIBS)
	$(CC) -o $@ $(PROD_CMDLINE)

#-------------------------------------------------------------------------
#
# Build rules for production and debug shared libraries
#
#-------------------------------------------------------------------------

%.$(SHLIB_EXT):
	$(SHLIB_CMD) $@ $^

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $@ $^

$(DEBUG_LIBGRSH):      $(DEBUG_GRSH_DYNAMIC_OBJS)
#$(PROD_LIBGRSH):       $(PROD_GRSH_DYNAMIC_OBJS)
$(DEBUG_LIBVISX):      $(DEBUG_VISX_OBJS)
$(PROD_LIBVISX):       $(PROD_VISX_OBJS)
$(DEBUG_LIBTCLWORKS):  $(DEBUG_TCLWORKS_OBJS)
$(PROD_LIBTCLWORKS):   $(PROD_TCLWORKS_OBJS)
$(DEBUG_LIBAPPWORKS):  $(DEBUG_APPWORKS_OBJS)
$(PROD_LIBAPPWORKS):   $(PROD_APPWORKS_OBJS)

#-------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------

ALL_SOURCES := \
	$(wildcard $(SRC)/*.cc) \
	$(wildcard $(SRC)/gwt/*.cc) \
	$(wildcard $(SRC)/io/*.cc) \
	$(wildcard $(SRC)/system/*.cc) \
	$(wildcard $(SRC)/tcl/*.cc) \
	$(wildcard $(SRC)/togl/*.cc) \
	$(wildcard $(SRC)/util/*.cc) \

ALL_HEADERS := \
	$(SRC)/*.h \
	$(SRC)/gwt/*.h \
	$(SRC)/io/*.h \
	$(SRC)/system/*.h \
	$(SRC)/tcl/*.h \
	$(SRC)/togl/*.h \
	$(SRC)/util/*.h

DEP_TEMP := $(patsubst %.cc,%.d,$(ALL_SOURCES))
ALL_DEPS := $(subst src/,dep/,$(DEP_TEMP))

MAKEDEP := $(SCRIPTS)/makedep

DEPOPTIONS := $(CPP_DEFINES) $(MY_INCLUDE_PATH) $(MAKEDEP_INCLUDES)

$(DEP)/%.d : $(SRC)/%.cc
	$(MAKEDEP) -DNO_EXTERNAL_INCLUDE_GUARDS $(DEPOPTIONS) $< > $@

# One way to handle the dependencies is to have a separate .d file for
# each source, but this can be very inefficient since it requires
# 'makedepend' to reopen and reparse the system headers for each
# source file... this means that remaking all the .d files for the
# project can take on the order of 60 seconds. So, for now, we do not
# '#include $(ALL_DEPS)'; instead we make and include one huge
# dependency file for the whole project, named 'alldepends', and this
# must be done every time the makefile is run. However this is quite
# fast, and can be done in under 5 seconds.

#include $(ALL_DEPS)

DEP_FILE := $(DEP)/alldepends

$(DEP_FILE): $(ALL_SOURCES) $(ALL_HEADERS)
	$(MAKEDEP) -DNO_EXTERNAL_INCLUDE_GUARDS $(DEPOPTIONS) $(ALL_SOURCES) > $@

include $(DEP_FILE)

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Remove all object files and build a new production executable from scratch
new: cleaner $(PROD_TARGET)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f ./*~ ./expt*2000.asw ./resp*2000 ./\#* ./core $(DOC)/*~ $(LOGS)/*~ \
		$(SRC)/*~ $(SRC)/*/*~ $(SCRIPTS)/*~ ./testing/*~

# Make clean, and also remove all debug object files
clean_do: clean
	rm -f $(OBJ)/*.do $(OBJ)/*/*.do

# Make clean, and also remove all production object files
clean_o: clean
	rm -f $(OBJ)/*.o $(OBJ)/*/*.o $(OBJ)/ii_files/*.ii $(OBJ)/*/ii_files/*.ii

clean_dep: clean
	rm $(DEP)/*.d $(DEP)/util/*.d $(DEP)/tcl/*.d

# Generate TAGS file based on all source files
$(SRC)/TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
	$(ETAGS) -f$(SRC)/TAGS $(ALL_SOURCES) $(ALL_HEADERS)

$(SRC)/H_TAGS: $(ALL_HEADERS)
	$(ETAGS) -f$(SRC)/H_TAGS $(ALL_HEADERS)

# Count the lines in all source files
count:
	wc -l $(ALL_SOURCES) $(ALL_HEADERS)

# Count the number of non-commented source lines
ncsl:
	NCSL $(ALL_SOURCES) $(ALL_HEADERS)

do_sizes:
	ls -lR obj/$(PLATFORM) | grep \.do | sort -n +5 > do_sizes

o_sizes:
	ls -lR obj/$(PLATFORM) | grep "\.o" | sort -n +5 > o_sizes

# Start emacs and load all source files and Makefile
edit: clean
	emacs $(ALL_SOURCES) $(ALL_HEADERS) Makefile $(SRC)/Log.txt

docs: $(DOC)/DoxygenConfig $(SRC)/*.h $(SRC)/*.doc
	(doxygen $(DOC)/DoxygenConfig > $(DOC)/DocLog) >& $(DOC)/DocErrors

$(IDEP)/AdepAliases: $(IDEP)/FileList $(ALL_SOURCES) $(ALL_HEADERS)
	adep -s -f$(IDEP)/FileList > $(IDEP)/AdepAliases

cdeps: $(ALL_SOURCES) $(ALL_HEADERS)
	cdep -i$(IDEP)/CdepSearchpath $+ > $(IDEP)/CdepDeps

ldeps: cdeps
	ldep -d$(IDEP)/CdepDeps -U./src -U./src/tcl -U./src/util  \
		> $(IDEP)/Ldeps || /bin/true

backup:
	tclsh $(SCRIPTS)/Backup.tcl

