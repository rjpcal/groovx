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

VERSION := 0.8a1
EXTRA_STATISTICS := 0

#-------------------------------------------------------------------------
#
# Directories for different file types
#
#-------------------------------------------------------------------------

ifeq ($(ARCH),hp9000s700)
	PLATFORM := hp9000s700
endif
ifeq ($(ARCH),irix6)
	PLATFORM := irix6
endif

PROJECT = $(HOME)/sorcery/grsh
SRC := src
DEP := ./dep
OBJ := ./obj/$(PLATFORM)
LIB := $(PROJECT)/lib/$(PLATFORM)
LOG := ./logs
DOC := ./doc
IDEP := ./idep
SCRIPTS := ./scripts

BIN_DIR := $(HOME)/local/$(ARCH)/bin

#-------------------------------------------------------------------------
#
# C++ Compiler, Architecture flags
#
#-------------------------------------------------------------------------


ARCH_FLAGS := 
CC := 
FILTER := 
MAKEDEP := $(SCRIPTS)/makedep
ifeq ($(PLATFORM),hp9000s700)
	CC := time aCC
#	FILTER := |& sed -e '/Warning.*opt.aCC.include./,/\^\^*/d' \
#		-e '/Warning.*usr.include./,/\^\^*/d'
	FILTER := 
	ARCH_FLAGS := +w +W829,740,8006,818,655,392,495,469,361,749,416 \
		-DACC_COMPILER -DHP9000S700 -DPRESTANDARD_IOSTREAMS -Dstd=
	DEPOPTIONS := -DACC_COMPILER -DHP9000S700 -DPRESTANDARD_IOSTREAMS \
		-I/opt/aCC/include -I/usr -I/opt/aCC/include/iostream \
		-I/opt/graphics/OpenGL/include -I/cit/rjpeters/include -I./src \
		-I./scripts/spoofdep
endif
ifeq ($(PLATFORM),irix6)
	CC := time /opt/MIPSpro/bin/CC
	FILTER := 
	ARCH_FLAGS := -n32 -DMIPSPRO_COMPILER -DIRIX6 -ptused -no_prelink \
		-DSTD_IO= -DPRESTANDARD_IOSTREAMS \
		-no_auto_include -LANG:std -LANG:exceptions=ON 
	DEPOPTIONS := -DMIPSPRO_COMPILER -DIRIX6 -DPRESTANDARD_IOSTREAMS \
		-I/usr/include/CC -I/cit/rjpeters/include/cppheaders \
		-I/cit/rjpeters/include -I./src -I./scripts/spoofdep
endif
ifeq ($(PLATFORM),irix6-gcc)
	CC := time /cit/rjpeters/gcc-2.95.1/bin/g++
# This filter removes warnings that are triggered by standard library files
	FILTER := |& sed \
		-e '/g++-3.*warning/d;' \
		-e '/In file included.*g++-3/,/:/d;' \
		-e '/g++-3.*In method/d;' \
		-e '/g++-3.*At top level/d;' \
		-e '/g++-3.*In instantiation of/,/instantiated from here/d'
	ARCH_FLAGS := -Wall -W -Wsign-promo -Weffc++ -DGCC_COMPILER -DIRIX6
	DEPOPTIONS := -DGCC_COMPILER -DIRIX6 \
		-I/cit/rjpeters/gcc/include/g++-3 -I/cit/rjpeters/include -I./src \
		-I./scripts/spoofdep
endif

#-------------------------------------------------------------------------
#
# Compilation Options
#
#-------------------------------------------------------------------------

ifeq ($(PLATFORM),hp9000s700)
	DEBUG_OPTIONS := +O1 +Z +p
	DEBUG_LINK_OPTIONS := -Wl,-B,immediate -Wl,+vallcompatwarnings

	PROD_OPTIONS := +O2 +Z +p
	PROD_LINK_OPTIONS := -Wl,+vallcompatwarnings
endif
ifeq ($(PLATFORM),irix6)
	DEBUG_OPTIONS := -g -O0
	DEBUG_LINK_OPTIONS :=

	PROD_OPTIONS := -O0
	PROD_LINK_OPTIONS :=
endif
ifeq ($(PLATFORM),irix6-gcc)
	DEBUG_OPTIONS := -g -O0
	DEBUG_LINK_OPTIONS :=

	PROD_OPTIONS := -O3
	PROD_LINK_OPTIONS :=
endif

#-------------------------------------------------------------------------
#
# Linker Options
#
#-------------------------------------------------------------------------

ifeq ($(PLATFORM),hp9000s700)
	SHLIB_CMD := $(CC) -b -o
	STATLIB_CMD := ar rus
	SHLIB_EXT := sl
	STATLIB_EXT := a
endif
ifeq ($(PLATFORM),irix6)
	SHLIB_CMD := ld -n32 -shared -check_registry /usr/lib32/so_locations
	STATLIB_CMD := /opt/MIPSpro/bin/CC -ar -o
	SHLIB_EXT := so
	STATLIB_EXT := a
endif
ifeq ($(PLATFORM),irix6-gcc)
	SHLIB_CMD := ld -n32 -shared -check_registry /usr/lib32/so_locations
	STATLIB_CMD := ar rus
	SHLIB_EXT := so
	STATLIB_EXT := a
endif

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------


ifeq ($(PLATFORM),irix6-gcc)
	STL_INCLUDE_DIR := -I$(HOME)/gcc/include/g++-3
endif
ifeq ($(PLATFORM),irix6)
	STL_INCLUDE_DIR := -I$(HOME)/include/cppheaders
endif

INCLUDE_DIRS := -I$(HOME)/include -I$(SRC) $(STL_INCLUDE_DIR)

ifeq ($(PLATFORM),hp9000s700)
	OPENGL_LIB_DIR := -L/opt/graphics/OpenGL/lib
	AUDIO_LIB_DIR := -L/opt/audio</lib
	AUDIO_LIB := -lAlib
	RPATH_DIR := 
	LOCAL_LIB_DIR := $(HOME)/lib/$(ARCH)
endif
ifeq ($(PLATFORM),irix6)
	OPENGL_LIB_DIR :=
	AUDIO_LIB_DIR :=
	AUDIO_LIB := -laudio -laudiofile
	RPATH_DIR := -Wl,-rpath,$(LIB)
	LOCAL_LIB_DIR := $(HOME)/local/$(ARCH)/lib
endif
ifeq ($(PLATFORM),irix6-gcc)
	OPENGL_LIB_DIR :=
	AUDIO_LIB_DIR :=
	AUDIO_LIB := -laudio -laudiofile
	RPATH_DIR := -Wl,-rpath,$(LIB)
	LOCAL_LIB_DIR := $(HOME)/lib/$(ARCH)
endif

LIB_DIRS :=  -L$(LIB) \
	-L$(LOCAL_LIB_DIR) \
	$(OPENGL_LIB_DIR) \
	$(AUDIO_LIB_DIR) \
	$(RPATH_DIR)

LIBRARIES := \
	-lGLU \
	-lGL \
	-ltk -ltcl -lXmu \
	-lX11 -lXext \
	-lm $(AUDIO_LIB)

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


GRSH_DYNAMIC_OBJS := \
	$(OBJ)/application.do \
	$(OBJ)/bitmaptcl.do \
	$(OBJ)/block.do \
	$(OBJ)/blocklist.do \
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
	$(OBJ)/ioptrlist.do \
	$(OBJ)/jittertcl.do \
	$(OBJ)/kbdresponsehdlr.do \
	$(OBJ)/masktcl.do \
	$(OBJ)/morphyfacetcl.do \
	$(OBJ)/nullresponsehdlr.do \
	$(OBJ)/objlist.do \
	$(OBJ)/objlisttcl.do \
	$(OBJ)/pbm.do \
	$(OBJ)/positiontcl.do \
	$(OBJ)/poslist.do \
	$(OBJ)/poslisttcl.do \
	$(OBJ)/response.do \
	$(OBJ)/responsehandler.do \
	$(OBJ)/rhlist.do \
	$(OBJ)/rhtcl.do \
	$(OBJ)/sound.do \
	$(OBJ)/soundlist.do \
	$(OBJ)/soundtcl.do \
	$(OBJ)/subject.do \
	$(OBJ)/subjecttcl.do \
	$(OBJ)/thlist.do \
	$(OBJ)/thtcl.do \
	$(OBJ)/timinghandler.do \
	$(OBJ)/timinghdlr.do \
	$(OBJ)/tlist.do \
	$(OBJ)/tlisttcl.do \
	$(OBJ)/tlistutils.do \
	$(OBJ)/trial.do \
	$(OBJ)/trialbase.do \
	$(OBJ)/trialevent.do \
	$(OBJ)/trialtcl.do \
	$(OBJ)/voidptrlist.do \

APPWORKS_OBJS := \
	$(OBJ)/factory.do \
	$(OBJ)/gwt/canvas.do \
	$(OBJ)/gwt/widget.do \
	$(OBJ)/io/asciistreamreader.do \
	$(OBJ)/io/asciistreamwriter.do \
	$(OBJ)/io/io.do \
	$(OBJ)/io/iofactory.do \
	$(OBJ)/io/iomgr.do \
	$(OBJ)/io/property.do \
	$(OBJ)/io/reader.do \
	$(OBJ)/io/readutils.do \
	$(OBJ)/io/writer.do \
	$(OBJ)/io/writeutils.do \
	$(OBJ)/system/demangle.do \
	$(OBJ)/system/system.do \
	$(OBJ)/util/error.do \
	$(OBJ)/util/errorhandler.do \
	$(OBJ)/util/observable.do \
	$(OBJ)/util/observer.do \
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
DEBUG_TCLWORKS_OBJS := $(TCLWORKS_OBJS)
DEBUG_APPWORKS_OBJS := $(APPWORKS_OBJS)

ifeq ($(PLATFORM),irix6)
	DEBUG_LIBVISX := $(LIB)/libvisx.d.$(STATLIB_EXT)
	DEBUG_LIBTCLWORKS := $(LIB)/libtclworks.d.$(STATLIB_EXT)
	DEBUG_LIBAPPWORKS := $(LIB)/libappworks.d.$(STATLIB_EXT)
	DEBUG_AUX_OBJ :=
	ALL_DEBUG_STATLIBS := $(DEBUG_LIBVISX) $(DEBUG_LIBTCLWORKS) \
		$(DEBUG_LIBAPPWORKS)
	ALL_DEBUG_SHLIBS :=
endif
ifeq ($(PLATFORM),irix6-gcc)
	DEBUG_LIBVISX := $(LIB)/libvisx.d.$(STATLIB_EXT)
	DEBUG_LIBTCLWORKS := $(LIB)/libtclworks.d.$(STATLIB_EXT)
	DEBUG_LIBAPPWORKS := $(LIB)/libappworks.d.$(STATLIB_EXT)
	DEBUG_AUX_OBJ :=
	ALL_DEBUG_STATLIBS := $(DEBUG_LIBVISX) $(DEBUG_LIBTCLWORKS) \
		$(DEBUG_LIBAPPWORKS)
	ALL_DEBUG_SHLIBS :=
endif
ifeq ($(PLATFORM),hp9000s700)
	DEBUG_LIBVISX := $(LIB)/libvisx.d.$(SHLIB_EXT)
	DEBUG_LIBTCLWORKS := $(LIB)/libtclworks.d.$(SHLIB_EXT)
	DEBUG_LIBAPPWORKS := $(LIB)/libappworks.d.$(SHLIB_EXT)
	DEBUG_AUX_OBJ := /opt/langtools/lib/end.o
	ALL_DEBUG_STATLIBS :=
	ALL_DEBUG_SHLIBS := $(DEBUG_LIBVISX) $(DEBUG_LIBTCLWORKS) \
		$(DEBUG_LIBAPPWORKS)
endif

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET := $(BIN_DIR)/grsh$(VERSION)

PROD_DEFS := -DASSERT -DINVARIANT

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.

ifeq ($(PLATFORM),irix6)
	PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(STATLIB_EXT)
	PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(STATLIB_EXT)
	PROD_LIBAPPWORKS := $(LIB)/libappworks$(VERSION).$(STATLIB_EXT)
	ALL_PROD_STATLIBS := $(PROD_LIBVISX) $(PROD_LIBTCLWORKS) \
		$(PROD_LIBAPPWORKS)
	ALL_PROD_SHLIBS :=
endif
ifeq ($(PLATFORM),irix6-gcc)
	PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(STATLIB_EXT)
	PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(STATLIB_EXT)
	PROD_LIBAPPWORKS := $(LIB)/libappworks$(VERSION).$(STATLIB_EXT)
	ALL_PROD_STATLIBS := $(PROD_LIBVISX) $(PROD_LIBTCLWORKS) \
		$(PROD_LIBAPPWORKS)
	ALL_PROD_SHLIBS :=
endif
ifeq ($(PLATFORM),hp9000s700)
	PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(SHLIB_EXT)
	PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(SHLIB_EXT)
	PROD_LIBAPPWORKS := $(LIB)/libappworks$(VERSION).$(SHLIB_EXT)
	ALL_PROD_STATLIBS :=
	ALL_PROD_SHLIBS := $(PROD_LIBVISX) $(PROD_LIBTCLWORKS) \
		$(PROD_LIBAPPWORKS)
endif

PROD_GRSH_MAIN_OBJ := $(GRSH_MAIN_OBJ:.do=.o)
PROD_GRSH_STATIC_OBJS := $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
PROD_GRSH_DYNAMIC_OBJS := $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_TCLWORKS_OBJS := $(DEBUG_TCLWORKS_OBJS:.do=.o)
PROD_APPWORKS_OBJS := $(DEBUG_APPWORKS_OBJS:.do=.o)

#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS := $(ARCH_FLAGS) $(INCLUDE_DIRS)

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

#	$(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)

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

ifeq ($(ARCH),hp9000s700)
default: testsh
endif
ifeq ($(ARCH),irix6)
default: grsh
endif

testsh: $(SRC)/TAGS $(ALL_DEBUG_SHLIBS) $(DEBUG_TARGET)
	$(DEBUG_TARGET) ./testing/grshtest.tcl

$(DEBUG_TARGET): $(DEBUG_GRSH_MAIN_OBJ) \
		$(DEBUG_GRSH_STATIC_OBJS) $(ALL_DEBUG_STATLIBS)
	$(CC) $(DEBUG_LINK_OPTIONS) -o $@ $(DEBUG_GRSH_STATIC_OBJS) \
	 $(DEBUG_GRSH_MAIN_OBJ) $(DEBUG_AUX_OBJ) \
	$(LIB_DIRS) -lvisx.d -ltclworks.d -lappworks.d $(LIBRARIES) 

grsh: $(SRC)/TAGS $(ALL_PROD_SHLIBS) $(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

#	echo "exit" | $(PROD_TARGET)


$(PROD_TARGET): $(PROD_GRSH_MAIN_OBJ) \
		$(PROD_GRSH_STATIC_OBJS) $(ALL_PROD_STATLIBS)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_GRSH_STATIC_OBJS) \
	$(PROD_GRSH_MAIN_OBJ) $(LIB_DIRS) \
	-lvisx$(VERSION) -ltclworks$(VERSION) -lappworks$(VERSION) \
	$(LIBRARIES) \

#-------------------------------------------------------------------------
#
# Build rules for production and debug shared libraries
#
#-------------------------------------------------------------------------

%.$(SHLIB_EXT):
	$(SHLIB_CMD) $@ $^

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $@ $^

$(DEBUG_LIBVISX):      $(DEBUG_GRSH_DYNAMIC_OBJS)
$(PROD_LIBVISX):       $(PROD_GRSH_DYNAMIC_OBJS)
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

alldepends: $(ALL_SOURCES) $(ALL_HEADERS)
	$(MAKEDEP) -DNO_EXTERNAL_INCLUDE_GUARDS $(DEPOPTIONS) $(ALL_SOURCES) > $@

include alldepends

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Remove all object files and build a new production executable from scratch
new: cleaner $(PROD_TARGET)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f ./*~ ./\#* ./core $(DOC)/*~ $(LOGS)/*~ \
		$(SRC)/*~ $(SRC)/*/*~ $(SCRIPTS)/*~ ./testing/*~

# Make clean, and also remove all debug object files
clean_do: clean
	rm $(OBJ)/*.do $(OBJ)/*/*.do

# Make clean, and also remove all production object files
clean_o: clean
	rm -f $(OBJ)/*.o $(OBJ)/*/*.o $(OBJ)/ii_files/*.ii $(OBJ)/*/ii_files/*.ii

clean_dep: clean
	rm $(DEP)/*.d $(DEP)/util/*.d $(DEP)/tcl/*.d

# Generate TAGS file based on all source files
$(SRC)/TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
ifeq ($(ARCH),hp9000s700)
	echo 'etags not installed'
else
	etags -f$(SRC)/TAGS $(ALL_SOURCES) $(ALL_HEADERS)
endif

$(SRC)/H_TAGS: $(ALL_HEADERS)
ifeq ($(ARCH),hp9000s700)
	echo 'etags not installed'
else
	etags -f$(SRC)/H_TAGS $(ALL_HEADERS)
endif

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
