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
	./config.status --file=Makedefs

config.status: configure
	./config.status --recheck

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

TCLTK_VERSION := 8.4

MAKEFLAGS += --warn-undefined-variables

#-------------------------------------------------------------------------
#
# Directories for different file types
#
#-------------------------------------------------------------------------

ifndef MODE
	MODE := $(DEFAULT_MODE)
endif

SRC := src
BUILD := build/$(ARCH)-$(MODE)
DEP := $(BUILD)/dep
OBJ := $(BUILD)/obj
LOGS := ./logs
DOC := ./doc
SCRIPTS := ./scripts

CPPFLAGS += -I$(SRC)

#-------------------------------------------------------------------------
#
# Options for compiling and linking
#
#-------------------------------------------------------------------------

ifeq ($(MODE),debug)
	LIB_SUFFIX := .d
	DEFS += -DPROF
endif

ifeq ($(MODE),prod)
	LIB_SUFFIX := $(PACKAGE_VERSION)
endif

LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

LIBS += \
	-lGLU -lGL \
	-ltcl$(TCLTK_VERSION) -ltk$(TCLTK_VERSION) \
	-lXmu -lX11 -lXext \
	-lz \
	-lpng \
	-lm

# add -lefence to LIBS for Electric Fence mem debugging


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

ifeq ($(MODE),debug)
	ALL_CXXFLAGS := $(DEBUG_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(DEFS)
endif

ifeq ($(MODE),prod)
	ALL_CXXFLAGS := $(PROD_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(DEFS)
endif

$(OBJ)/%.$(OBJEXT) : $(SRC)/%.cc
	@mkdir -p $(LOGS) $(dir $@)
	@echo $< >> $(LOGS)/CompileStats
	@echo ""
	time $(CXX) $(ALL_CXXFLAGS) \
		-c $< \
		-o $@

# to avoid deleting any intermediate targets
.SECONDARY:

$(SRC)/%.precc : $(SRC)/%.cc
	time $(CXX) -E $< $(ALL_CXXFLAGS) > $@

$(SRC)/%.preh : $(SRC)/%.h
	echo "#include \"$<\"" > .temp.cc
	time $(CXX) -E .temp.cc $(ALL_CXXFLAGS) > $@

#-------------------------------------------------------------------------
#
# Build rules for static and dynamic libraries
#
#-------------------------------------------------------------------------

%.$(SHLIB_EXT):
	time $(SHLIB_CMD) $@ $(LDFLAGS) $^

%.$(STATLIB_EXT):
	time $(STATLIB_CMD) $@ $^

#-------------------------------------------------------------------------
#
# Build rules for directory structure
#
#-------------------------------------------------------------------------

ALL_SOURCES := $(wildcard $(SRC)/[a-z]*/*.cc $(SRC)/[a-z]*/[a-z]*/*.cc)
ALL_HEADERS := $(wildcard $(SRC)/[a-z]*/*.h  $(SRC)/[a-z]*/[a-z]*/*.h)

SRCDIRS := $(sort $(dir $(ALL_SOURCES)))
ALLDIRS := $(subst $(SRC), $(OBJ), $(SRCDIRS)) $(DEP)

.PHONY: dir_structure
dir_structure:
	@echo "building directory structure"
	@for dr in $(ALLDIRS); do if [ ! -d $$dr ]; then mkdir -p $$dr; fi; done

#-------------------------------------------------------------------------
#
# Dependencies 
#
#-------------------------------------------------------------------------

# dependencies of object files on source+header files

$(DEP)/cppdepends: $(DEP)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS)
	time $(SCRIPTS)/cppdeps.tcl --src $(SRC) --objdir $(OBJ)/ > $@

include $(DEP)/cppdepends

# dependencies of package shlib's on object files

VISX_LIB_DIR := $(exec_prefix)/lib/visx

$(DEP)/pkgdepends: $(DEP)/.timestamp $(VISX_LIB_DIR)/.timestamp \
		$(ALL_SOURCES) $(ALL_HEADERS) src/pkgs/buildPkgDeps.tcl
	src/pkgs/buildPkgDeps.tcl \
		--depfile $@ \
		--objdir $(OBJ)/pkgs \
		--objext $(OBJEXT) \
		--pkgdir $(SRC)/pkgs \
		--libdir $(VISX_LIB_DIR)

include $(DEP)/pkgdepends


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

LIB_DEP_CMD := 	$(SCRIPTS)/build_lib_rules.tcl \
		--libdir $(exec_prefix)/lib \
		--libprefix libDeep \
		--libext $(LIB_EXT) \
		--srcroot $(SRC) \
		--objroot $(OBJ) \
		--objext .$(OBJEXT) \
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

$(DEP)/libdepends: $(DEP)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS) \
		$(SCRIPTS)/build_lib_rules.tcl
	$(LIB_DEP_CMD) $@

include $(DEP)/libdepends

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

all:
	make dir_structure
	make TAGS
	make $(ALL_SHLIBS)
	make $(PKG_LIBS)
	make $(EXECUTABLE)
	make check

#build: dir_structure TAGS $(ALL_SHLIBS) $(PKG_LIBS) $(EXECUTABLE)

GRSH_STATIC_OBJS := $(subst .cc,.$(OBJEXT),\
	$(subst $(SRC),$(OBJ), $(wildcard $(SRC)/grsh/*.cc)))

$(EXECUTABLE): $(exec_prefix)/bin/.timestamp $(GRSH_STATIC_OBJS) $(ALL_STATLIBS)
	time $(CXX) -o $@ $(GRSH_STATIC_OBJS) $(LDFLAGS) $(PROJECT_LIBS) $(LIBS)

check:
	$(EXECUTABLE) ./testing/grshtest.tcl

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
	find $(OBJ) -follow -name \*.$(OBJEXT) -exec rm -f {} \;
	find $(OBJ) -follow -name \*.ii -exec rm -f {} \;

# Count the lines in all source files
count: $(LOGS)/.timestamp
	wc $(ALL_SOURCES) $(ALL_HEADERS) | tee $(LOGS)/count

count_sort: $(LOGS)/.timestamp $(ALL_SOURCES) $(ALL_HEADERS)
	wc $(ALL_SOURCES) $(ALL_HEADERS) | sort -n | tee $(LOGS)/count_sort

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

obj_sizes:
	ls -lLR $(OBJ) | grep "\.$(OBJEXT)" | sort -n +4 > obj_sizes

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
	cd snapshots/$(SNAPSHOT); autoconf
	cd snapshots; tar cfz $(SNAPSHOT).tar.gz $(SNAPSHOT)
