##########################################################################
#
# Makefile 
#
# Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
#
# This is the Makefile for the grsh shell. This shell provides the
# following functionality:
# 
# 	1) Tcl/Tk 8.2.1 core
# 	2) OpenGL graphics, via the Togl widget
# 	3) A set of extensions for running visual psychophysics experiments
#
#
# $Id$
#
##########################################################################

VERSION := 0.8a7

TCLTK_VERSION := 8.2

MAKEFLAGS += --warn-undefined-variables

CPP_DEFINES :=
CC_SWITCHES :=
LD_OPTIONS :=
INCLUDE_PATH :=
LIB_PATH :=

#-------------------------------------------------------------------------
#
# Platform selection
#
#-------------------------------------------------------------------------

PLATFORM := $(ARCH)

#-------------------------------------------------------------------------
#
# Directories for different file types
#
#-------------------------------------------------------------------------

PROJECT = $(HOME)/sorcery/grsh
SRC := src
DEP := ./dep/$(PLATFORM)
OBJ := ./obj/$(PLATFORM)
LIB := $(PROJECT)/lib/$(PLATFORM)
LOGS := ./logs
DOC := ./doc
IDEP := ./idep
SCRIPTS := ./scripts

LOCAL_ARCH := $(HOME)/local/$(PLATFORM)

BIN_DIR := $(LOCAL_ARCH)/bin
TMP_DIR := ./tmp/$(PLATFORM)

#-------------------------------------------------------------------------
#
# Platform selection
#
#-------------------------------------------------------------------------

ETAGS := etags

ifeq ($(PLATFORM),hp9000s700)
	COMPILER := aCC
	SHLIB_EXT := sl
	STATLIB_EXT := a

	CPP_DEFINES += -DHP9000S700 -DHAVE_ZSTREAM

	DEFAULT_MODE := debug

	ETAGS := echo "no etags"

	AUDIO_LIB := -lAlib
	ZSTREAM_LIB := -lzstream

	LIB_PATH += -L/opt/graphics/OpenGL/lib -L/opt/audio/lib
endif

ifeq ($(PLATFORM),irix6)
	COMPILER := MIPSpro
	SHLIB_EXT := so
	STATLIB_EXT := a

	CPP_DEFINES += -DIRIX6 -DHAVE_ZSTREAM

	DEFAULT_MODE := prod

	AUDIO_LIB := -laudio -laudiofile
	ZSTREAM_LIB := -lzstream

	LIB_PATH += -Wl,-rpath,$(LIB)
endif

ifeq ($(PLATFORM),i686)
	COMPILER := g++
	SHLIB_EXT := so
	STATLIB_EXT := a

	CPP_DEFINES += -DI686 -DHAVE_ZSTREAM

	DEFAULT_MODE := debug

	AUDIO_LIB := -lesd -laudiofile
	ZSTREAM_LIB := -lzstream
endif

ifndef MODE
	MODE := $(DEFAULT_MODE)
endif

#-------------------------------------------------------------------------
#
# Options for compiling and linking
#
#-------------------------------------------------------------------------

ifeq ($(MODE),debug)
	OBJ_EXT := .do
	LIB_SUFFIX := .d
endif

ifeq ($(MODE),prod)
	OBJ_EXT := .o
	LIB_SUFFIX := $(VERSION)
endif

ifeq ($(COMPILER),aCC)
	CC := time aCC
#	FILTER := 
	FILTER := |& sed -e '/Warning.*opt.aCC.include./,/\^\^*/d' \
		-e '/Warning.*usr.include./,/\^\^*/d'
# 361 == wrongly complains about falling off end of non-void function
# 392 == 'Conversion unnecessary; expression was already of type...'
	CC_SWITCHES += +w +W361,392
	CPP_DEFINES += -DACC_COMPILER -DPRESTANDARD_IOSTREAMS -Dstd= -DSTD_IO=

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g1 +Z +p
		LD_OPTIONS += -Wl,-B,immediate -Wl,+vallcompatwarnings
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += +O2 +Z +p
		LD_OPTIONS += -Wl,+vallcompatwarnings
	endif

	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

	SHLIB_CMD := $(CC) -b -o
	STATLIB_CMD := ar rus
endif

ifeq ($(COMPILER),MIPSpro)
	CC := time /opt/MIPSpro/bin/CC
	FILTER := |& sed -e '/WARNING/,/vcid_.*_cc/d' \
		-e '/static const char vcid_/,/^ *\^$$/d'
	CC_SWITCHES += -n32 -ptused -no_prelink \
		-no_auto_include -LANG:std -LANG:exceptions=ON 

	CPP_DEFINES += -DMIPSPRO_COMPILER -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	INCLUDE_PATH += -I$(HOME)/include/cppheaders

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
		LD_OPTIONS +=
	endif

# Tests showed that -O3 provided little improvement over -O2 for this app
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
		LD_OPTIONS +=
	endif

	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

	SHLIB_CMD := $(CC) -shared -Wl,-check_registry,/usr/lib32/so_locations -o
	STATLIB_CMD := $(CC) -ar -o
endif

ifeq ($(COMPILER),g++)
	CC := time g++
# This filter removes warnings that are triggered by standard library files
	FILTER := |& sed \
		-e '/g++-3.*warning/d;' \
		-e '/In file included.*g++-3/,/:/d;' \
		-e '/g++-3.*In method/d;' \
		-e '/g++-3.*At top level/d;' \
		-e '/g++-3.*In instantiation of/,/instantiated from here/d' \
		-e '/In instantiation of/,/has a non-virtual destructor/d' \
		-e '/has a non-virtual destructor/d'
	CC_SWITCHES += -Wall -W -Wsign-promo -Weffc++
	CPP_DEFINES += -DGCC_COMPILER -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	INCLUDE_PATH += -I$(HOME)/local/$(PLATFORM)/include/g++-3

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
		LD_OPTIONS +=
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += -O3
		LD_OPTIONS +=
	endif

	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

	SHLIB_CMD := $(CC) -shared -o
	STATLIB_CMD := ar rus
endif

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

INCLUDE_PATH += -I$(LOCAL_ARCH)/include -I$(SRC)

LIB_PATH += -L$(LIB) -L$(LOCAL_ARCH)/lib

ifeq ($(PLATFORM),i686)
	LIB_PATH += -L/usr/X11R6/lib
endif

EXTERNAL_LIBS := \
	-lGLU -lGL \
	$(LOCAL_ARCH)/lib/libtcl.$(SHLIB_EXT) $(LOCAL_ARCH)/lib/libtk.$(SHLIB_EXT) \
	-lXmu -lX11 -lXext \
	$(ZSTREAM_LIB) \
	$(AUDIO_LIB) \
	-lm

#-------------------------------------------------------------------------
#
# List of object files
#
#-------------------------------------------------------------------------

# My intent is that libvisx should contain the core classes for the
# experiment framework, and that libgrsh should contain the
# application-specific subclasses (like Face, House, etc.). But there
# are some bizarre problems on HPUX that give crashes from OpenGL,
# which are very sensitive to the link order. So for now, all the
# GL-related objects go into libgrsh, and everything else into libvisx.

# Ah, well, now the HP-OpenGL bug is rearing its ugly head again, so
# I'll have to go back to statically linking the OpenGL stuff.

#
# static objects
#

TOGL_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/togl/*.cc)))

STATIC_SRCS := \
	$(shell grep -l 'GL/gl\.h' src/*.cc) \
	$(shell grep -l 'int main' src/*.cc)

STATIC_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(STATIC_SRCS)))

GRSH_STATIC_OBJS := $(STATIC_OBJS) $(TOGL_OBJS)

#
# libDeepVision
#

DEEPVISION_SRCS := $(filter-out $(STATIC_SRCS),$(wildcard src/*.cc))
DEEPVISION_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(DEEPVISION_SRCS)))

LIBDEEPVISION := $(LIB)/libDeepVision$(LIB_EXT)

#
# libDeepAppl
#

GWT_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/gwt/*.cc)))

GX_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/gx/*.cc)))

IO_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/io/*.cc)))

DEEPAPPL_OBJS := $(GWT_OBJS) $(GX_OBJS) $(IO_OBJS)

LIBDEEPAPPL := $(LIB)/libDeepAppl$(LIB_EXT)

#
# libDeepUtil
#

SYS_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/system/*.cc)))

DEEPUTIL_OBJS := $(SYS_OBJS) \
	$(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/util/*.cc)))

LIBDEEPUTIL := $(LIB)/libDeepUtil$(LIB_EXT)

#
# libDeepTcl
#

DEEPTCL_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/tcl/*.cc)))

LIBDEEPTCL := $(LIB)/libDeepTcl$(LIB_EXT)



PROJECT_LIBS := $(LIBDEEPVISION) $(LIBDEEPTCL) $(LIBDEEPAPPL) $(LIBDEEPUTIL)

#-------------------------------------------------------------------------
#
# Info for executable targets
#
#-------------------------------------------------------------------------

ifeq ($(MODE),debug)
	EXECUTABLE := $(BIN_DIR)/testsh
	CPP_DEFINES += -DPROF -DASSERT -DINVARIANT -DTEST -DDEBUG_BUILD
endif
ifeq ($(MODE),prod)
	EXECUTABLE := $(BIN_DIR)/grsh$(VERSION)
	CPP_DEFINES += -DASSERT -DINVARIANT
endif

ALL_STATLIBS := $(filter %.$(STATLIB_EXT),$(PROJECT_LIBS))
ALL_SHLIBS   := $(filter %.$(SHLIB_EXT),$(PROJECT_LIBS))

ifeq ($(MODE),debug)
	ifeq ($(PLATFORM),hp9000s700)
		GRSH_STATIC_OBJS += /opt/langtools/lib/end.o
	endif
endif

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

all: showpid TAGS $(ALL_SHLIBS) $(EXECUTABLE)
	$(EXECUTABLE) ./testing/grshtest.tcl

.PHONY: showpid
showpid:
ifeq ($(PLATFORM),i686)
	ps -ef | grep make
endif

CMDLINE := $(LD_OPTIONS) $(GRSH_STATIC_OBJS) $(LIB_PATH) \
	$(PROJECT_LIBS) $(EXTERNAL_LIBS)

$(EXECUTABLE): $(GRSH_STATIC_OBJS) $(ALL_STATLIBS)
	$(CC) -o $(TMP_DIR)/tmpfile $(CMDLINE); mv $(TMP_DIR)/tmpfile $@

#-------------------------------------------------------------------------
#
# Build rules for object files
#
#-------------------------------------------------------------------------

ALL_CC_OPTIONS := $(CC_SWITCHES) $(INCLUDE_PATH) $(CPP_DEFINES)

$(OBJ)/%$(OBJ_EXT) : $(SRC)/%.cc
	echo $< >> $(LOGS)/CompileStats
	csh -fc "($(CC) -c $< -o $@ $(ALL_CC_OPTIONS)) $(FILTER)"

$(SRC)/%.precc : $(SRC)/%.cc
	$(CC) -E $< $(ALL_CC_OPTIONS) > $@

$(SRC)/%.preh : $(SRC)/%.h
	echo "#include \"$<\"" > .temp.cc
	$(CC) -E .temp.cc $(ALL_CC_OPTIONS) > $@

#-------------------------------------------------------------------------
#
# Build rules for static and dynamic libraries
#
#-------------------------------------------------------------------------

%.$(SHLIB_EXT):
	$(SHLIB_CMD) $(TMP_DIR)/tmpfile $^; mv $(TMP_DIR)/tmpfile $@

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $(TMP_DIR)/tmpfile $^; mv $(TMP_DIR)/tmpfile $@

$(LIBDEEPVISION): $(DEEPVISION_OBJS)
$(LIBDEEPTCL):    $(DEEPTCL_OBJS)
$(LIBDEEPAPPL):   $(DEEPAPPL_OBJS)
$(LIBDEEPUTIL):   $(DEEPUTIL_OBJS)

# this is just a convenience target so that we don't have to specify
# the entire pathnames of the different library targets
lib%: $(LIB)/lib%$(LIB_EXT)
	true

#-------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------

ALL_SOURCES := $(wildcard $(SRC)/*.cc) $(wildcard $(SRC)/[a-z]*/*.cc)
ALL_HEADERS := $(wildcard $(SRC)/*.h)  $(wildcard $(SRC)/[a-z]*/*.h)

DEP_FILE := $(DEP)/alldepends

# (1) We don't need to give any -D options to mkdep.pl, since it is
# conservative and assumes that all #include directives are actually
# in effect, even if they might actually be conditionally compiled
# away. (2) We don't need to give any -I options to mkdep.pl, since we
# are only interested in dependencies within the current project (and
# not on the standard headers, for example).

$(DEP_FILE): $(ALL_SOURCES) $(ALL_HEADERS)
	$(SCRIPTS)/splitnewlines $+ > make.files
	mkdep.pl | $(SCRIPTS)/filterdep > $@
	rm make.files make.ofiles

include $(DEP_FILE)

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Remove all object files and build a new production executable from scratch
new: cleaner $(EXECUTABLE)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f ./*~ ./expt*2001.asw ./resp*2001 ./\#* ./core $(DOC)/*~ $(LOGS)/*~ \
		$(SRC)/*~ $(SRC)/*/*~ $(SCRIPTS)/*~ ./testing/*~

# Make clean, and also remove all debug object files
cleaner: clean
	rm -f $(OBJ)/*$(OBJ_EXT) $(OBJ)/*/*$(OBJ_EXT) \
	 $(OBJ)/ii_files/*.ii $(OBJ)/*/ii_files/*.ii

# Generate TAGS file based on all source files
TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
	$(ETAGS) -fTAGS $(ALL_SOURCES) $(ALL_HEADERS)

H_TAGS: $(ALL_HEADERS)
	$(ETAGS) -fH_TAGS $(ALL_HEADERS)

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

benchmarks: $(EXECUTABLE)
	$(EXECUTABLE) $(SCRIPTS)/benchmarks.tcl -output $(LOGS)/benchmarks.txt
