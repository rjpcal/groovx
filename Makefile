##########################################################################
#
# Makefile 
#
# Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
#
# This is the Makefile for the grsh shell. This shell includes the
# following functionality:
# 
# 	1) Tcl/Tk core
# 	2) OpenGL graphics
# 	3) A set of extensions for running visual psychophysics experiments
#
#
# $Id$
#
##########################################################################

default: all

###
### All configuration options should be selected via the configure script,
### which will generate an appropriate Makedefs file, which is included here:
###

Makedefs: Makedefs.in config.status
	config.status --file=Makedefs

config.status: configure
	config.status --recheck

configure: configure.in
	autoconf

include Makedefs

###
### SHOULDN'T NEED TO MODIFY ANYTHING BELOW THIS POINT
###

#-------------------------------------------------------------------------
#
# Various default values
#
#-------------------------------------------------------------------------

TCL_VERSION := 8.4
TK_VERSION := 8.4

MAKEFLAGS += --warn-undefined-variables

CPP_DEFINES := $(DEFS)
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

SRC := src
DEP := ./dep/$(PLATFORM)
OBJ := obj/$(PLATFORM)
LOGS := ./logs
DOC := ./doc
SCRIPTS := ./scripts

INCLUDE_PATH += -I$(with_tcltk)/include -I$(SRC)

TMP_DIR := ./tmp/$(PLATFORM)
TMP_FILE := $(TMP_DIR)/tmpfile

#-------------------------------------------------------------------------
#
# Platform selection
#
#-------------------------------------------------------------------------

ifeq ($(PLATFORM),irix6)
	COMPILER := MIPSpro
	SHLIB_EXT := so
	STATLIB_EXT := a

	CPP_DEFINES += -DSHORTEN_SYMBOL_NAMES

	DEFAULT_MODE := prod

	AUDIO_LIB := -laudio -laudiofile

	LIB_PATH += -Wl,-rpath,$(exec_prefix)/lib
endif

ifeq ($(PLATFORM),i686)
	COMPILER := g++3
	SHLIB_EXT := so
	STATLIB_EXT := a

# display lists don't work at present with i686/linux/mesa

	CPP_DEFINES += -DBROKEN_GL_DISPLAY_LISTS -march=i686

	DEFAULT_MODE := debug

	AUDIO_LIB := -lesd -laudiofile

	LIB_PATH += -Wl,-rpath,$(with_tcltk)/lib
endif

ifeq ($(PLATFORM),ppc)
	COMPILER := g++2
	SHLIB_EXT := dylib
	STATLIB_EXT := a
	CPP_DEFINES += -DESD_WORKAROUND
	DEFAULT_MODE := debug
	AUDIO_LIB := -lesd -laudiofile
# The /sw/lib and /sw/include directories are managed by Fink
	LIB_PATH += -L/usr/X11R6/lib -L/sw/lib
	INCLUDE_PATH += -I/usr/X11R6/include -I/sw/include
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
	CPP_DEFINES += -DPROF
endif

ifeq ($(MODE),prod)
	OBJ_EXT := .o
	LIB_SUFFIX := $(PACKAGE_VERSION)
endif

LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

ifeq ($(COMPILER),MIPSpro)
	CXX := time /opt/MIPSpro/bin/CC -mips3
	FILTER := |& sed -e '/WARNING/,/vcid_.*_cc/d' \
		-e '/static const char vcid_/,/^ *\^$$/d'
	CC_SWITCHES += -n32 -ptused -no_prelink \
		-no_auto_include -LANG:std -LANG:exceptions=ON 

	CPP_DEFINES += -DMIPSPRO_COMPILER -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	INCLUDE_PATH += -I$(HOME)/local/$(ARCH)/include/cppheaders

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
	endif

# Tests showed that -O3 provided little improvement over -O2 for this app
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
	endif

	SHLIB_CMD := $(CXX) -shared -Wl,-check_registry,/usr/lib32/so_locations -o
	STATLIB_CMD := $(CXX) -ar -o
endif

ifeq ($(COMPILER),g++2)
	CXX := time g++2
	CC_SWITCHES += -Wall -W -Wsign-promo
	CPP_DEFINES += -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O1
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += -O3
	endif

ifeq ($(PLATFORM),ppc)
	CC_SWITCHES += -dynamic

# Need to use -install_name ${LIB_RUNTIME_DIR}/libname?
	SHLIB_CMD := $(CXX) -dynamiclib -flat_namespace -undefined suppress -o
	STATLIB_CMD := libtool -static -o
else
	SHLIB_CMD := $(CXX) -shared -o
	STATLIB_CMD := ar rus
endif
endif

ifeq ($(COMPILER),g++3)
	CXX := time g++-3
# Filter the compiler output...
	WARNINGS := -W -Wdeprecated -Wno-system-headers -Wall -Wsign-promo -Wwrite-strings
	CC_SWITCHES += $(WARNINGS)
	CPP_DEFINES += -DSTD_IO=std

	ifeq ($(MODE),debug)
		CC_SWITCHES += -O1 -g
	endif

# Need this to allow symbols from the executable to be accessed by loaded
# dynamic libraries; this is needed e.g. for the matlab libut.so library to
# find the "_start" symbol.
	LD_OPTIONS += -Wl,--export-dynamic

# can't use -O3 with g++301, since we get core dumps...
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
	endif

	SHLIB_CMD := $(CXX) -shared -o
	STATLIB_CMD := ar rus
endif

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

LIB_PATH += -L$(exec_prefix)/lib -L$(with_tcltk)/lib

EXTERNAL_LIBS := \
	-lGLU -lGL \
	-ltcl$(TCL_VERSION) -ltk$(TK_VERSION) \
	-lXmu -lX11 -lXext \
	-lz \
	-lpng \
	$(AUDIO_LIB) \
	-lm

ifeq ($(enable_readline), yes)
	EXTERNAL_LIBS += -lreadline -ltermcap
	CPP_DEFINES += -DWITH_READLINE
endif

ifeq ($(enable_matlab), yes)
	LIB_PATH += -Wl,-rpath,$(exec_prefix)/lib

	INCLUDE_PATH += -I/usr/local/matlab/extern/include
	LIB_PATH += -L/usr/local/matlab/extern/lib/glnx86
	LIB_PATH += -Wl,-rpath,/usr/local/matlab/extern/lib/glnx86
	CPP_DEFINES += -DWITH_MATLAB
endif

ifeq ($(PLATFORM),ppc)
	EXTERNAL_LIBS += -lcc_dynamic
endif

ifeq ($(PLATFORM),i686)
	LIB_PATH += -L/usr/X11R6/lib
endif

# add -lefence to EXTERNAL_LIBS for Electric Fence mem debugging


#-------------------------------------------------------------------------
#
# Build rules for object files
#
#-------------------------------------------------------------------------

# This build rule helps to create subdirectories that don't need to be part of
# the CVS repository, but do need to exist to hold generated files during the
# build process in sandboxes..
%.timestamp:
	@[ -d ${@D} ] || mkdir -p ${@D}
	@[ -f $@ ] || touch $@

ALL_CC_OPTIONS := $(CC_SWITCHES) $(INCLUDE_PATH) $(CPP_DEFINES)

$(OBJ)/%.o : $(SRC)/%.cc
	@mkdir -p $(LOGS)
	@echo $< >> $(LOGS)/CompileStats
	@echo ""
	$(CXX) $(ALL_CC_OPTIONS) \
		-c $< \
		-o $@

$(OBJ)/%.do : $(SRC)/%.cc
	@mkdir -p $(LOGS)
	@echo $< >> $(LOGS)/CompileStats
	@echo ""
	$(CXX) $(ALL_CC_OPTIONS) \
		-c $< \
		-o $@

# to avoid deleting any intermediate targets
.SECONDARY:

$(SRC)/%.precc : $(SRC)/%.cc
	$(CXX) -E $< $(ALL_CC_OPTIONS) > $@

$(SRC)/%.preh : $(SRC)/%.h
	echo "#include \"$<\"" > .temp.cc
	$(CXX) -E .temp.cc $(ALL_CC_OPTIONS) > $@

#-------------------------------------------------------------------------
#
# Build rules for static and dynamic libraries
#
#-------------------------------------------------------------------------

ifneq ($(PLATFORM),ppc)
%.$(SHLIB_EXT):
	$(SHLIB_CMD) $(TMP_DIR)/$(notdir $@) $(LIB_PATH) $^ && mv $(TMP_DIR)/$(notdir $@) $@
endif

ifeq ($(PLATFORM),ppc)
%.$(SHLIB_EXT):
	$(SHLIB_CMD) $@ $(LIB_PATH) $^ -lcc_dynamic
endif

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $(TMP_DIR)/$(notdir $@) $^ && mv $(TMP_DIR)/$(notdir $@) $@

# this is just a convenience target so that we don't have to specify
# the entire pathnames of the different library targets
lib%: $(exec_prefix)/lib/.timestamp $(exec_prefix)/lib/lib%$(LIB_EXT)
	true

#-------------------------------------------------------------------------
#
# Build rules for directory structure
#
#-------------------------------------------------------------------------

ALL_SOURCES := $(wildcard $(SRC)/[a-z]*/*.cc $(SRC)/[a-z]*/[a-z]*/*.cc)
ALL_HEADERS := $(wildcard $(SRC)/[a-z]*/*.h  $(SRC)/[a-z]*/[a-z]*/*.h)

SRCDIRS := $(sort $(dir $(ALL_SOURCES)))
ALLDIRS := $(subst $(SRC), $(OBJ), $(SRCDIRS)) $(TMP_DIR) $(DEP)

.PHONY: dir_structure
dir_structure:
	@echo "building directory structure"
	@for dr in $(ALLDIRS); do if [ ! -d $$dr ]; then mkdir -p $$dr; fi; done

#-------------------------------------------------------------------------
#
# Dependencies 
#
#-------------------------------------------------------------------------

# For each of the dependencies that get built and then "include"d into the
# Makefile, we have to use a two-target method to get them built, in order
# to avoid some thorny issues with time stamps etc. Without the two-step
# method, the potential problem is the following: if we are doing either a
# "make --assume-new" or if there is any clock skew involving one of the ,
# then we get an infinite loop of rebuilding the file to be "include"d,
# because after the inclusion, the entire makefile is reprocessed again,
# which forces the included file to be rebuilt, and so on. In the two-step
# process, the dependencies apply to a dummy file that is NOT "include"d
# into the makefile, and then the "include"d file depends on the dummy
# file. This avoids infinite looping.

# dependencies of object files on source+header files

DEP_FILE := $(DEP)/alldepends.$(MODE)

$(DEP_FILE).deps: $(DEP)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS)
	touch $@

$(DEP_FILE): $(DEP_FILE).deps
	time $(SCRIPTS)/cppdeps.tcl --src $(SRC) --objdir obj/$(ARCH)/ > $@

include $(DEP_FILE)

# dependencies of package shlib's on object files

VISX_LIB_DIR := $(exec_prefix)/lib/visx

PKG_DEP_FILE := $(DEP)/pkgdepends.$(MODE)

$(PKG_DEP_FILE).deps: $(DEP)/.timestamp $(VISX_LIB_DIR)/.timestamp \
		$(ALL_SOURCES) $(ALL_HEADERS) src/pkgs/buildPkgDeps.tcl
	touch $@

$(PKG_DEP_FILE): $(PKG_DEP_FILE).deps
	src/pkgs/buildPkgDeps.tcl $@

include $(PKG_DEP_FILE)


$(exec_prefix)/lib/visx/mtx.so: \
	/usr/local/matlab/extern/lib/glnx86/libmx.so \
	/usr/local/matlab/extern/lib/glnx86/libmatlb.so

$(exec_prefix)/lib/visx/matlabengine.so: \
	/usr/local/matlab/extern/lib/glnx86/libeng.so \
	/usr/local/matlab/extern/lib/glnx86/libmx.so \
	/usr/local/matlab/extern/lib/glnx86/libut.so \
	/usr/local/matlab/extern/lib/glnx86/libmat.so \
	/usr/local/matlab/extern/lib/glnx86/libmi.so \
	/usr/local/matlab/extern/lib/glnx86/libmatlb.so


# dependencies of main project shlib's on object files

LIB_DEP_FILE := $(DEP)/libdepends.$(MODE)

LIB_DEP_CMD := 	$(SCRIPTS)/build_lib_rules.tcl \
		--libdir $(exec_prefix)/lib \
		--libprefix libDeep \
		--libext $(LIB_EXT) \
		--srcroot $(SRC) \
		--objroot $(OBJ) \
		--objext $(OBJ_EXT) \
		--module Visx \
		--module Gfx \
		--module GWT \
		--module Tcl \
		--module IO \
		--module Gx \
		--module Togl \
		--module System \
		--module Util \
		>

$(LIB_DEP_FILE).deps: $(DEP)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS) \
		$(SCRIPTS)/build_lib_rules.tcl
	touch $@

$(LIB_DEP_FILE): $(LIB_DEP_FILE).deps
	$(LIB_DEP_CMD) $@

include $(LIB_DEP_FILE)

cdeps: $(ALL_SOURCES) $(ALL_HEADERS) $(LOGS)/.timestamp
	time cdeplevels.py $(SRC) -L 1000 > $(LOGS)/cdeps

ldeps: $(ALL_SOURCES) $(ALL_HEADERS) $(LOGS)/.timestamp
	time ldeplevels.py $(SRC) -L 1000 > $(LOGS)/ldeps

#-------------------------------------------------------------------------
#
# Build rules for production and debug executables
#
#-------------------------------------------------------------------------

ALL_STATLIBS := $(filter %.$(STATLIB_EXT),$(PROJECT_LIBS))
ALL_SHLIBS   := $(filter %.$(SHLIB_EXT),$(PROJECT_LIBS))

ifeq ($(MODE),debug)
	EXECUTABLE := $(exec_prefix)/bin/testsh
endif
ifeq ($(MODE),prod)
	EXECUTABLE := $(exec_prefix)/bin/grsh$(PACKAGE_VERSION)
endif

all: build
	$(EXECUTABLE) ./testing/grshtest.tcl

build: dir_structure TAGS $(ALL_SHLIBS) $(PKG_LIBS) $(EXECUTABLE)

GRSH_STATIC_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/grsh/*.cc)))

CMDLINE := $(LD_OPTIONS) $(GRSH_STATIC_OBJS) $(LIB_PATH) \
	$(PROJECT_LIBS) $(EXTERNAL_LIBS)

$(EXECUTABLE): $(exec_prefix)/bin/.timestamp $(GRSH_STATIC_OBJS) $(ALL_STATLIBS)
	$(CXX) -o $(TMP_FILE) $(CMDLINE) && mv $(TMP_FILE) $@

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

backup:
	tclsh $(SCRIPTS)/Backup.tcl

benchmarks: $(EXECUTABLE) $(LOGS)/.timestamp
	$(EXECUTABLE) $(SCRIPTS)/benchmarks.tcl -output $(LOGS)/benchmarks.txt

# Remove all backups, temporaries, and coredumps
clean:
	find . -name \*~ -exec rm -f {} \;
	rm -f ./expt*2002.asw ./resp*2002 ./\#* ./core

# Make clean, and also remove all debug object files
cleaner: clean
	find $(OBJ) -follow -name \*$(OBJ_EXT) -exec rm -f {} \;
	find $(OBJ) -follow -name \*.ii -exec rm -f {} \;

# Count the lines in all source files
count: $(LOGS)/.timestamp
	wc $(ALL_SOURCES) $(ALL_HEADERS) | tee $(LOGS)/count

count_sort: $(LOGS)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS)
	wc $(ALL_SOURCES) $(ALL_HEADERS) | sort -n | tee $(LOGS)/count_sort

do_sizes:
	ls -lLR obj/$(PLATFORM) | grep "\.do" | sort -n +4 > do_sizes

docs: $(DOC)/DoxygenConfig $(DOC)/*.doc $(ALL_HEADERS)
	(doxygen $(DOC)/DoxygenConfig > $(DOC)/DocLog) >& $(DOC)/DocErrors
	cd ~/www/grsh; chmod -R og+r *

# Generate tags file based only on header files
H_TAGS: $(ALL_HEADERS)
	find $(SRC) -name \*.h | etags - -o $@

# Count the number of non-commented source lines
ncsl: $(LOGS)/.timestamp
	NCSL $(ALL_SOURCES) $(ALL_HEADERS) | tee $(LOGS)/ncsl

# Count the number of non-commented source lines and sort
ncsl_sort: $(LOGS)/.timestamp
	NCSL $(ALL_SOURCES) $(ALL_HEADERS) | sort -n | tee $(LOGS)/ncsl_sort

o_sizes:
	ls -lLR obj/$(PLATFORM) | grep "\.o" | sort -n +4 > o_sizes

.PHONY: showpid
showpid:
	ps -ef | grep make

# Generate TAGS file based on all source files
TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
	find $(SRC) -name \*.h -or -name \*.cc | etags - -o $@

SNAPSHOT := grsh-`date +%Y_%m_%d`

export: snapshots/.timestamp
	cvs -d `cat ./CVS/Root` export -r HEAD -d snapshots/$(SNAPSHOT) grsh
	rm -r snapshots/$(SNAPSHOT)/logs
	rm -r snapshots/$(SNAPSHOT)/old_src
	rm -r snapshots/$(SNAPSHOT)/future_src
	cd snapshots; tar cfz $(SNAPSHOT).tar.gz $(SNAPSHOT)
