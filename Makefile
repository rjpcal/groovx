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

PROJECT = $(HOME)/grsh
SRC := ./src
DEP := ./dep
OBJ := ./obj/$(ARCH)
LIB := $(PROJECT)/lib/$(ARCH)
LOG := ./logs
DOC := ./doc
IDEP := ./idep
SCRIPTS := ./scripts

#-------------------------------------------------------------------------
#
# C++ Compiler, Architecture flags
#
#-------------------------------------------------------------------------

ARCH_FLAGS := 
CC := 
FILTER := 
MAKEDEP := $(SCRIPTS)/makedep
ifeq ($(ARCH),hp9000s700)
	CC := time aCC
	FILTER := |& sed -e '/Warning.*opt.aCC.include./,/\^\^*/d' \
		-e '/Warning.*usr.include./,/\^\^*/d'
	ARCH_FLAGS := +w +W818,655,392,495,469,361,749,416 -DACC_COMPILER -DHP9000S700
	DEPOPTIONS := -I/opt/aCC/include -I/usr -I/opt/aCC/include/iostream \
	 -I/opt/graphics/OpenGL/include -I/cit/rjpeters/include -I./src
endif
ifeq ($(ARCH),irix6)
	CC := time g++
# This filter removes warnings that are triggered by standard library files
	FILTER := |& sed \
		-e '/g++-3.*warning/d;' \
		-e '/In file included.*g++-3/,/:/d;' \
		-e '/g++-3.*In method/d;' \
		-e '/g++-3.*At top level/d;' \
		-e '/g++-3.*In instantiation of/,/instantiated from here/d'
	ARCH_FLAGS := -Wall -W -Wsign-promo -Weffc++ -DGCC_COMPILER -DIRIX6
endif

#-------------------------------------------------------------------------
#
# Compilation Options
#
#-------------------------------------------------------------------------

ifeq ($(ARCH),hp9000s700)
	DEBUG_OPTIONS := +O1 +Z +p
	DEBUG_LINK_OPTIONS := -Wl,-B,immediate -Wl,+vallcompatwarnings

	PROD_OPTIONS := +O3 +Z +p
	PROD_LINK_OPTIONS := -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	DEBUG_OPTIONS := -O1
	DEBUG_LINK_OPTIONS :=

	PROD_OPTIONS := -O3
	PROD_LINK_OPTIONS := 
endif

#-------------------------------------------------------------------------
#
# Linker Options
#
#-------------------------------------------------------------------------

ifeq ($(ARCH),hp9000s700)
	SHLIB_CMD := $(CC) -b -o
	STATLIB_CMD := ar rus
	SHLIB_EXT := sl
	STATLIB_EXT := a
endif
ifeq ($(ARCH),irix6)
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


ifeq ($(ARCH),irix6)
	STL_INCLUDE_DIR := -I$(HOME)/gcc/include/g++-3
else
	STL_INCLUDE_DIR :=
endif

INCLUDE_DIRS := -I$(HOME)/include -I$(SRC) $(STL_INCLUDE_DIR)

ifeq ($(ARCH),hp9000s700)
	OPENGL_LIB_DIR := -L/opt/graphics/OpenGL/lib
	AUDIO_LIB_DIR := -L/opt/audio/lib
	AUDIO_LIB := -lAlib
	RPATH_DIR := 
endif
ifeq ($(ARCH),irix6)
	OPENGL_LIB_DIR :=
	AUDIO_LIB_DIR :=
	AUDIO_LIB := -laudio -laudiofile
	RPATH_DIR := -Wl,-rpath,$(HOME)/grsh/$(ARCH)
endif

LIB_DIRS :=  -L$(LIB) \
	-L$(HOME)/lib/$(ARCH) \
	$(OPENGL_LIB_DIR) \
	$(AUDIO_LIB_DIR) \
	$(RPATH_DIR)

LIBRARIES := \
	-ltogl -lGLU -lGL \
	-ltk -ltcl -lXmu \
	-lX11 -lXext \
	-lm $(AUDIO_LIB)


#-------------------------------------------------------------------------
#
# List of object files
#
#-------------------------------------------------------------------------

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
	$(OBJ)/grobj.do \
	$(OBJ)/grshAppInit.do \
	$(OBJ)/gtext.do \
	$(OBJ)/house.do \
	$(OBJ)/jitter.do \
	$(OBJ)/maskhatch.do \
	$(OBJ)/morphyface.do \
	$(OBJ)/objtogl.do \
	$(OBJ)/position.do \
	$(OBJ)/tclgl.do \
	$(OBJ)/tlistwidget.do \
	$(OBJ)/toglconfig.do \
	$(OBJ)/xbitmap.do \
	$(OBJ)/xbmaprenderer.do \

GRSH_DYNAMIC_OBJS := \
	$(OBJ)/application.do \
	$(OBJ)/asciistreamreader.do \
	$(OBJ)/asciistreamwriter.do \
	$(OBJ)/bitmaptcl.do \
	$(OBJ)/block.do \
	$(OBJ)/blocklist.do \
	$(OBJ)/blocktcl.do \
	$(OBJ)/bmapdata.do \
	$(OBJ)/bmaprenderer.do \
	$(OBJ)/canvas.do \
	$(OBJ)/cloneface.do \
	$(OBJ)/demangle.do \
	$(OBJ)/util/error.do \
	$(OBJ)/eventresponsehdlr.do \
	$(OBJ)/experiment.do \
	$(OBJ)/exptdriver.do \
	$(OBJ)/expttcl.do \
	$(OBJ)/expttesttcl.do \
	$(OBJ)/facetcl.do \
	$(OBJ)/factory.do \
	$(OBJ)/fishtcl.do \
	$(OBJ)/fixpttcl.do \
	$(OBJ)/gabortcl.do \
	$(OBJ)/grobjtcl.do \
	$(OBJ)/grshapp.do \
	$(OBJ)/gtexttcl.do \
	$(OBJ)/housetcl.do \
	$(OBJ)/io.do \
	$(OBJ)/iofactory.do \
	$(OBJ)/iomgr.do \
	$(OBJ)/ioptrlist.do \
	$(OBJ)/jittertcl.do \
	$(OBJ)/kbdresponsehdlr.do \
	$(OBJ)/masktcl.do \
	$(OBJ)/morphyfacetcl.do \
	$(OBJ)/nullresponsehdlr.do \
	$(OBJ)/objlist.do \
	$(OBJ)/objlisttcl.do \
	$(OBJ)/observable.do \
	$(OBJ)/observer.do \
	$(OBJ)/pbm.do \
	$(OBJ)/positiontcl.do \
	$(OBJ)/poslist.do \
	$(OBJ)/poslisttcl.do \
	$(OBJ)/property.do \
	$(OBJ)/reader.do \
	$(OBJ)/readutils.do \
	$(OBJ)/response.do \
	$(OBJ)/responsehandler.do \
	$(OBJ)/rhlist.do \
	$(OBJ)/rhtcl.do \
	$(OBJ)/sound.do \
	$(OBJ)/soundlist.do \
	$(OBJ)/soundtcl.do \
	$(OBJ)/util/strings.do \
	$(OBJ)/subject.do \
	$(OBJ)/subjecttcl.do \
	$(OBJ)/system.do \
	$(OBJ)/thlist.do \
	$(OBJ)/thtcl.do \
	$(OBJ)/timinghandler.do \
	$(OBJ)/timinghdlr.do \
	$(OBJ)/tlist.do \
	$(OBJ)/tlisttcl.do \
	$(OBJ)/tlistutils.do \
	$(OBJ)/util/trace.do \
	$(OBJ)/trial.do \
	$(OBJ)/trialevent.do \
	$(OBJ)/trialtcl.do \
	$(OBJ)/value.do \
	$(OBJ)/voidptrlist.do \
	$(OBJ)/widget.do \
	$(OBJ)/writer.do \
	$(OBJ)/writeutils.do \

TCLWORKS_OBJS := \
	$(OBJ)/tcl/listpkg.do \
	$(OBJ)/tcl/misctcl.do \
	$(OBJ)/tcl/stringifycmd.do \
	$(OBJ)/tcl/tclcmd.do \
	$(OBJ)/tcl/tcldlist.do \
	$(OBJ)/tcl/tclerror.do \
	$(OBJ)/tcl/tclitempkg.do \
	$(OBJ)/tcl/tclpkg.do \
	$(OBJ)/tcl/tclvalue.do \
	$(OBJ)/tcl/tclveccmds.do \
	$(OBJ)/tcl/tracertcl.do \


#-------------------------------------------------------------------------
#
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

DEBUG_TARGET := $(HOME)/bin/$(ARCH)/testsh

DEBUG_LIBVISX := $(LIB)/libvisx.d.$(SHLIB_EXT)
DEBUG_LIBTCLWORKS := $(LIB)/libtclworks.d.$(SHLIB_EXT)

DEBUG_DEFS := -DPROF -DASSERT -DINVARIANT -DTEST

DEBUG_GRSH_STATIC_OBJS := $(GRSH_STATIC_OBJS)
DEBUG_GRSH_DYNAMIC_OBJS := $(GRSH_DYNAMIC_OBJS)
DEBUG_TCLWORKS_OBJS := $(TCLWORKS_OBJS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET := $(HOME)/bin/$(ARCH)/grsh$(VERSION)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.
ifeq ($(ARCH),irix6)
	LIBEXT := $(STATLIB_EXT)
endif
ifeq ($(ARCH),hp9000s700)
	LIBEXT := $(SHLIB_EXT)
endif

PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(SHLIB_EXT)
PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(SHLIB_EXT)

PROD_GRSH_STATIC_OBJS := $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
PROD_GRSH_DYNAMIC_OBJS := $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_TCLWORKS_OBJS := $(DEBUG_TCLWORKS_OBJS:.do=.o)

#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS := $(ARCH_FLAGS) $(INCLUDE_DIRS)

ALL_PROD_OPTIONS := $(COMMON_OPTIONS) $(PROD_OPTIONS) 
ALL_DEBUG_OPTIONS := $(COMMON_OPTIONS) $(DEBUG_OPTIONS) $(DEBUG_DEFS)

$(OBJ)/%.o : $(SRC)/%.cc
	echo $(<D) >> $(LOG)/CompileStats
	csh -fc "($(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)) $(FILTER)"

$(OBJ)/%.do : $(SRC)/%.cc
ifeq ($(EXTRA_STATISTICS),1)
	$(CC) -E $< $(ALL_DEBUG_OPTIONS) > .temp.cc
	wc -l $<
	wc -l .temp.cc
endif
	echo $< >> $(LOG)/CompileStats
	$(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)

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

testsh: $(SRC)/TAGS $(DEBUG_TARGET)
	$(DEBUG_TARGET) ./testing/grshtest.tcl

ALL_DEBUG_DEPENDS := \
	$(DEBUG_GRSH_STATIC_OBJS) \
	$(DEBUG_LIBVISX) \
	$(DEBUG_LIBTCLWORKS)

$(DEBUG_TARGET): $(ALL_DEBUG_DEPENDS)
	$(CC) $(DEBUG_LINK_OPTIONS) -o $@ $(DEBUG_GRSH_STATIC_OBJS) \
	 /opt/langtools/lib/end.o \
	$(LIB_DIRS) -lvisx.d -ltclworks.d $(LIBRARIES) 

grsh: $(SRC)/TAGS $(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

ALL_PROD_DEPENDS := \
	$(PROD_GRSH_STATIC_OBJS) \
	$(PROD_LIBVISX) \
	$(PROD_LIBTCLWORKS)

$(PROD_TARGET): $(ALL_PROD_DEPENDS)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_GRSH_STATIC_OBJS) \
	$(LIB_DIRS) -lvisx$(VERSION) -ltclworks$(VERSION) $(LIBRARIES)

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

#-------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------

ALL_SOURCES := \
	$(wildcard $(SRC)/*.cc) \
	$(wildcard $(SRC)/util/*.cc) \
	$(wildcard $(SRC)/tcl/*.cc) 

ALL_HEADERS := $(SRC)/*.h $(SRC)/util/*.h $(SRC)/tcl/*.h
DEP_TEMP := $(patsubst %.cc,%.d,$(ALL_SOURCES))
ALL_DEPS := $(subst src/,dep/,$(DEP_TEMP))

$(DEP)/%.d : $(SRC)/%.cc
	$(MAKEDEP) $(DEPOPTIONS) $< > $@

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
	$(MAKEDEP) $(DEPOPTIONS) $+ > $@

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
		$(SRC)/*~ $(SCRIPTS)/*~ ./testing/*~

# Make clean, and also remove all debug object files
clean_do: clean
	mv $(OBJ)/*.do $(OBJ)/util/*.do $(OBJ)/tcl/*.do trash

# Make clean, and also remove all production object files
clean_o: clean
	mv $(OBJ)/*.o $(OBJ)/util/*.o $(OBJ)/tcl/*.o trash

clean_dep: clean
	mv $(DEP)/*.d $(DEP)/util/*.d $(DEP)/tcl/*.d trash

# Generate TAGS file based on all source files
$(SRC)/TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
ifeq ($(ARCH),hp9000s700)
	echo 'etags not installed'
else
	etags -f$(SRC)/TAGS $(ALL_SOURCES) $(ALL_HEADERS)
endif

# Count the lines in all source files
count:
	wc -l $(ALL_SOURCES) $(ALL_HEADERS)

# Count the number of non-commented source lines
ncsl:
	NCSL $(ALL_SOURCES) $(ALL_HEADERS)

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
