##########################################################################
#
# Makefile 
#
# Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
#
# This is the Makefile for the grsh shell. This shell provides the
# following functionality:
# 
# 	1) Tcl/Tk 8.3.3 core
# 	2) OpenGL graphics
# 	3) A set of extensions for running visual psychophysics experiments
#
#
# $Id$
#
##########################################################################

default: all

VERSION := 0.8a8

TCL_VERSION := 8.3.3
TK_VERSION := 8.3.3

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

LOCAL_ARCH := $(HOME)/local/$(PLATFORM)

SRC := src
DEP := ./dep/$(PLATFORM)
OBJ := obj/$(PLATFORM)
LOGS := ./logs
DOC := ./doc
SCRIPTS := ./scripts

LOCAL_LIB := $(LOCAL_ARCH)/lib
LOCAL_BIN := $(LOCAL_ARCH)/bin
TMP_DIR := ./tmp/$(PLATFORM)
TMP_FILE := $(TMP_DIR)/tmpfile

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

	CPP_DEFINES += -DHP9000S700

	DEFAULT_MODE := debug

	ETAGS := echo "no etags"

	AUDIO_LIB := -lAlib

	LIB_PATH += -L/opt/graphics/OpenGL/lib -L/opt/audio/lib
endif

ifeq ($(PLATFORM),irix6)
	COMPILER := MIPSpro
	SHLIB_EXT := so
	STATLIB_EXT := a

	CPP_DEFINES += -DIRIX6

	DEFAULT_MODE := prod

	AUDIO_LIB := -laudio -laudiofile

	LIB_PATH += -Wl,-rpath,$(LOCAL_LIB)
endif

ifeq ($(PLATFORM),i686)
	COMPILER := g++3
	SHLIB_EXT := so
	STATLIB_EXT := a

	CPP_DEFINES += -DI686 -march=i686

	DEFAULT_MODE := debug

	AUDIO_LIB := -lesd -laudiofile-0.2.1
endif

ifeq ($(PLATFORM),ppc)
	COMPILER := ppc-g++-2
	SHLIB_EXT := dylib
	STATLIB_EXT := a
	CPP_DEFINES += -DPPC
	DEFAULT_MODE := debug
	ETAGS := etags
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
	CC := time /opt/MIPSpro/bin/CC -mips3
	FILTER := |& sed -e '/WARNING/,/vcid_.*_cc/d' \
		-e '/static const char vcid_/,/^ *\^$$/d'
	CC_SWITCHES += -n32 -ptused -no_prelink \
		-no_auto_include -LANG:std -LANG:exceptions=ON 

	CPP_DEFINES += -DMIPSPRO_COMPILER -DSTD_IO= -DPRESTANDARD_IOSTREAMS

	INCLUDE_PATH += -I$(LOCAL_ARCH)/include/cppheaders

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
	endif

# Tests showed that -O3 provided little improvement over -O2 for this app
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
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
	CPP_DEFINES += -DGCC_COMPILER=2 -DSTD_IO= -DPRESTANDARD_IOSTREAMS \
		-DFUNCTIONAL_OK

	INCLUDE_PATH += -I$(HOME)/local/$(PLATFORM)/include/g++-3

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O1
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += -O3
	endif

	SHLIB_CMD := $(CC) -shared -o
	STATLIB_CMD := ar rus
	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)
endif

ifeq ($(COMPILER),ppc-g++-2)
	CC := time g++
# This filter removes warnings that are triggered by standard library files
	FILTER := |& sed \
		-e '/darwin.*warning/d;' \
		-e '/In file included.*darwin/,/:/d;' \
		-e '/darwin.*In method/d;' \
		-e '/darwin.*At top level/d;' \
		-e '/darwin.*In instantiation of/,/instantiated from here/d' \
		-e '/In instantiation of/,/has a non-virtual destructor/d' \
		-e '/has a non-virtual destructor/d'
	CC_SWITCHES += -Wall -W -Wsign-promo -Weffc++
	CPP_DEFINES += -DGCC_COMPILER=2 -DSTD_IO= -DPRESTANDARD_IOSTREAMS \
		-DFUNCTIONAL_OK

	CPP_DEFINES += -Dlrand48=rand
	CC_SWITCHES += -dynamic

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O1
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += -O3
	endif

# Need to use -install_name ${LIB_RUNTIME_DIR}/libname?
	SHLIB_CMD := c++ -dynamiclib -flat_namespace -undefined suppress -o
	STATLIB_CMD := libtool -static -o
	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)
endif

ifeq ($(COMPILER),g++3)
	CC := time g++302
# Filter the compiler output...
	FILTER := |& $(SCRIPTS)/filter_gcc_v3

	WARNINGS := -W -Wdeprecated -Wno-system-headers -Wall -Wsign-promo -Wwrite-strings -Weffc++
	CC_SWITCHES += $(WARNINGS)
	CPP_DEFINES += -DGCC_COMPILER=3 -DSTD_IO=std -DFUNCTIONAL_OK

	ifeq ($(MODE),debug)
		CC_SWITCHES += -O1 -g
	endif

# can't use -O3 with g++301, since we get core dumps...
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
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

LIB_PATH += -L$(LOCAL_LIB)

EXTERNAL_LIBS := \
	-lGLU -lGL \
	-ltcl$(TCL_VERSION) -ltk$(TK_VERSION) \
	-lXmu -lX11 -lXext \
	-lz \
	$(AUDIO_LIB) \
	-lm

ifneq ($(PLATFORM),ppc)
	LIB_PATH += -Wl,-rpath,$(LOCAL_LIB)

	INCLUDE_PATH += -I/usr/local/matlab/extern/include
	LIB_PATH += -L/usr/local/matlab/extern/lib/glnx86
	LIB_PATH += -Wl,-rpath,/usr/local/matlab/extern/lib/glnx86
	CPP_DEFINES += -DHAVE_MATLAB

	EXTERNAL_LIBS += -leng -lmx -lut -lmat -lmi -lmatlb
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

ALL_CC_OPTIONS := $(CC_SWITCHES) $(INCLUDE_PATH) $(CPP_DEFINES)

$(OBJ)/%$(OBJ_EXT) : $(SRC)/%.cc
	echo $< >> $(LOGS)/CompileStats
	csh -fc "($(CC) -c $< -o $@ $(ALL_CC_OPTIONS)) $(FILTER)"

# to avoid deleting any intermediate targets
.SECONDARY:

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

ifneq ($(PLATFORM),ppc)
%.$(SHLIB_EXT):
	$(SHLIB_CMD) $(TMP_DIR)/$(notdir $@) $^ && mv $(TMP_DIR)/$(notdir $@) $@
endif

ifeq ($(PLATFORM),ppc)
%.$(SHLIB_EXT):
	$(SHLIB_CMD) $@ $^ -lcc_dynamic
endif

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $(TMP_DIR)/$(notdir $@) $^ && mv $(TMP_DIR)/$(notdir $@) $@

# this is just a convenience target so that we don't have to specify
# the entire pathnames of the different library targets
lib%: $(LOCAL_LIB)/lib%$(LIB_EXT)
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
	for dr in $(ALLDIRS); do if [ ! -d $$dr ]; then mkdir -p $$dr; fi; done

#-------------------------------------------------------------------------
#
# Dependencies 
#
#-------------------------------------------------------------------------


# dependencies of object files on source+header files

DEP_FILE := $(DEP)/alldepends.$(MODE)

$(DEP):
	mkdir -p $@

$(DEP_FILE): $(DEP) $(ALL_SOURCES) $(ALL_HEADERS)
	time pydep.py $(SRC) --objdir obj/$(ARCH)/ > $@

include $(DEP_FILE)


# dependencies of package shlib's on object files

PKG_DEP_FILE := $(DEP)/pkgdepends.$(MODE)

$(PKG_DEP_FILE): $(DEP) $(ALL_SOURCES) $(ALL_HEADERS) \
	Makefile src/pkgs/buildPkgDeps.tcl
	src/pkgs/buildPkgDeps.tcl

include $(PKG_DEP_FILE)


# dependencies of main project shlib's on object files

LIB_DEP_FILE := $(DEP)/libdepends.$(MODE)

$(LIB_DEP_FILE): $(DEP) $(ALL_SOURCES) $(ALL_HEADERS) \
  Makefile $(SCRIPTS)/build_lib_rules.tcl
	$(SCRIPTS)/build_lib_rules.tcl \
		--libdir $(LOCAL_LIB) \
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
		> $@

include $(LIB_DEP_FILE)


cdeps: $(ALL_SOURCES) $(ALL_HEADERS)
	time cdeplevels.py $(SRC) -L 1000 > $(LOGS)/cdeps

ldeps: $(ALL_SOURCES) $(ALL_HEADERS)
	time ldeplevels.py $(SRC) -L 1000 > $(LOGS)/ldeps

#-------------------------------------------------------------------------
#
# Build rules for production and debug executables
#
#-------------------------------------------------------------------------

ALL_STATLIBS := $(filter %.$(STATLIB_EXT),$(PROJECT_LIBS))
ALL_SHLIBS   := $(filter %.$(SHLIB_EXT),$(PROJECT_LIBS))

ifeq ($(MODE),debug)
	EXECUTABLE := $(LOCAL_BIN)/testsh
endif
ifeq ($(MODE),prod)
	EXECUTABLE := $(LOCAL_BIN)/grsh$(VERSION)
endif

all: build
	$(EXECUTABLE) ./testing/grshtest.tcl

build: dir_structure TAGS $(ALL_SHLIBS) $(PKG_LIBS) $(EXECUTABLE)

GRSH_STATIC_OBJS := $(subst .cc,$(OBJ_EXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/grsh/*.cc)))

ifeq ($(MODE),debug)
	ifeq ($(PLATFORM),hp9000s700)
		GRSH_STATIC_OBJS += /opt/langtools/lib/end.o
	endif
endif

CMDLINE := $(LD_OPTIONS) $(GRSH_STATIC_OBJS) $(LIB_PATH) \
	$(PROJECT_LIBS) $(EXTERNAL_LIBS)
$(EXECUTABLE): $(GRSH_STATIC_OBJS) $(ALL_STATLIBS)
	$(CC) -o $(TMP_FILE) $(CMDLINE) && mv $(TMP_FILE) $@


#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

backup:
	tclsh $(SCRIPTS)/Backup.tcl

benchmarks: $(EXECUTABLE)
	$(EXECUTABLE) $(SCRIPTS)/benchmarks.tcl -output $(LOGS)/benchmarks.txt

# Remove all backups, temporaries, and coredumps
clean:
	find . -name \*~ -exec rm -f {} \;
	rm -f ./expt*2001.asw ./resp*2001 ./\#* ./core

# Make clean, and also remove all debug object files
cleaner: clean
	find $(OBJ) -name \*$(OBJ_EXT) -exec rm -f {} \;
	find $(OBJ) -name \*.ii -exec rm -f {} \;

# Count the lines in all source files
count: $(ALL_SOURCES) $(ALL_HEADERS)
	wc -lc $+

counts: $(ALL_SOURCES) $(ALL_HEADERS)
	wc -lc $+ | sort -n > counts

do_sizes:
	ls -lLR obj/$(PLATFORM) | grep "\.do" | sort -n +4 > do_sizes

docs: $(DOC)/DoxygenConfig $(DOC)/*.doc $(ALL_HEADERS)
	(doxygen $(DOC)/DoxygenConfig > $(DOC)/DocLog) >& $(DOC)/DocErrors
	cd ~/www/grsh; chmod -R og+r *

# Generate tags file based only on header files
H_TAGS: $(ALL_HEADERS)
	$(ETAGS) -fH_TAGS $(ALL_HEADERS)

# Count the number of non-commented source lines
ncsl: $(ALL_SOURCES) $(ALL_HEADERS)
	NCSL $+

# Count the number of non-commented source lines
ncsls: $(ALL_SOURCES) $(ALL_HEADERS)
	NCSL $+ | sort -n > ncsls

# Remove all object files and build a new production executable from scratch
new: cleaner $(EXECUTABLE)

o_sizes:
	ls -lLR obj/$(PLATFORM) | grep "\.o" | sort -n +4 > o_sizes

.PHONY: showpid
showpid:
	ps -ef | grep make

# Generate TAGS file based on all source files
TAGS: $(ALL_SOURCES) $(ALL_HEADERS)
	$(ETAGS) -fTAGS $(ALL_SOURCES) $(ALL_HEADERS)

tardist: clean
	touch diststamp
	cd ..; tar cvfz grsh.tar.gz grsh \
		--exclude *.o --exclude *.do \
		--exclude *.a --exclude *.sl \
		--exclude *,v --exclude *~ --exclude a.out
	rm diststamp

distpatch: clean
	cd ..; tar cvfz patch.tar.gz `find grsh -newer grsh/diststamp -type f`
