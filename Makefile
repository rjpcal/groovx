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

VERSION := 0.8a7

TCLTK_VERSION := 8.2

MAKEFLAGS += --warn-undefined-variables

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

	ifndef MODE
		MODE := debug
	endif

	ETAGS := echo "no etags"
endif

ifeq ($(ARCH),irix6)
	PLATFORM := irix6
	COMPILER := MIPSpro
	SHLIB_EXT := so
	STATLIB_EXT := a

	ifndef MODE
		MODE := prod
	endif
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
LOGS := ./logs
DOC := ./doc
IDEP := ./idep
SCRIPTS := ./scripts

LOCAL_ARCH := $(HOME)/local/$(ARCH)

BIN_DIR := $(LOCAL_ARCH)/bin

LOCAL_LIB_DIR := $(LOCAL_ARCH)/lib

#-------------------------------------------------------------------------
#
# Options for compiling and linking
#
#-------------------------------------------------------------------------

CPP_DEFINES :=
CC_SWITCHES :=

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
	CPP_DEFINES += -DACC_COMPILER -DHP9000S700 -DPRESTANDARD_IOSTREAMS \
		-Dstd= -DSTD_IO= -DHAVE_ZSTREAM
	ARCH_MAKEDEP_INCLUDES := -I/opt/aCC/include -I/usr -I/opt/aCC/include/iostream \
		-I/opt/graphics/OpenGL/include -I./scripts/spoofdep
	STL_INCLUDE_DIR := 

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g1 +Z +p
		LD_OPTIONS := -Wl,-B,immediate -Wl,+vallcompatwarnings
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += +O2 +Z +p
		LD_OPTIONS := -Wl,+vallcompatwarnings
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
	CPP_DEFINES += -DMIPSPRO_COMPILER -DIRIX6 -DSTD_IO= \
		-DPRESTANDARD_IOSTREAMS -DHAVE_ZSTREAM

	ARCH_MAKEDEP_INCLUDES := -I/usr/include/CC \
		-I/cit/rjpeters/include/cppheaders

	STL_INCLUDE_DIR := -I$(HOME)/include/cppheaders

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
		LD_OPTIONS :=
	endif

# Tests showed that -O3 provided little improvement over -O2 for this app
	ifeq ($(MODE),prod)
		CC_SWITCHES += -O2
		LD_OPTIONS :=
	endif

	LIB_EXT := $(LIB_SUFFIX).$(SHLIB_EXT)

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
	CC_SWITCHES += -Wall -W -Wsign-promo -Weffc++
	CPP_DEFINES += -DGCC_COMPILER -DIRIX6

	ARCH_MAKEDEP_INCLUDES := -I/cit/rjpeters/gcc/include/g++-3

	STL_INCLUDE_DIR := -I$(HOME)/gcc/include/g++-3

	ifeq ($(MODE),debug)
		CC_SWITCHES += -g -O0
		LD_OPTIONS :=
	endif

	ifeq ($(MODE),prod)
		CC_SWITCHES += -O3
		LD_OPTIONS :=
	endif

	LIB_EXT := $(LIB_SUFFIX).$(STATLIB_EXT)

	SHLIB_CMD := ld -n32 -shared -check_registry /usr/lib32/so_locations
	STATLIB_CMD := ar rus
endif

MAKEDEP_INCLUDES := $(ARCH_MAKEDEP_INCLUDES) -I./scripts/spoofdep

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

INCLUDE_PATH := -I$(LOCAL_ARCH)/include -I$(HOME)/include -I$(SRC) \
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
	ZSTREAM_LIB := -lzstream

	OPENGL_LIB_DIR :=
	AUDIO_LIB_DIR :=
	RPATH_DIR := -Wl,-rpath,$(LIB)
endif

LIB_PATH :=  -L$(LIB) \
	-L$(LOCAL_LIB_DIR) \
	$(OPENGL_LIB_DIR) \
	$(AUDIO_LIB_DIR) \
	$(RPATH_DIR)

EXTERNAL_LIBS := \
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
	ifeq ($(ARCH),hp9000s700)
		GRSH_STATIC_OBJS += /opt/langtools/lib/end.o
	endif
endif

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

all: $(SRC)/TAGS $(ALL_SHLIBS) $(EXECUTABLE)
	$(EXECUTABLE) ./testing/grshtest.tcl

CMDLINE := $(LD_OPTIONS) $(GRSH_STATIC_OBJS) $(LIB_PATH) \
	$(PROJECT_LIBS) $(EXTERNAL_LIBS)

$(EXECUTABLE): $(GRSH_STATIC_OBJS) $(ALL_STATLIBS)
	$(CC) -o $@ $(CMDLINE)

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

$(LIB)/lib%$(LIB_SUFFIX).$(SHLIB_EXT):
	$(SHLIB_CMD) $@ $^

$(LIB)/lib%$(LIB_SUFFIX).$(STATLIB_EXT):
	$(STATLIB_CMD) $@ $^

$(LIBDEEPVISION): $(DEEPVISION_OBJS)
$(LIBDEEPTCL):    $(DEEPTCL_OBJS)
$(LIBDEEPAPPL):   $(DEEPAPPL_OBJS)
$(LIBDEEPUTIL):   $(DEEPUTIL_OBJS)

#-------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------

ALL_SOURCES := $(wildcard $(SRC)/*.cc) $(wildcard $(SRC)/[a-z]*/*.cc)
ALL_HEADERS := $(wildcard $(SRC)/*.h)  $(wildcard $(SRC)/[a-z]*/*.h)

DEPOPTIONS := $(CPP_DEFINES) $(INCLUDE_PATH) $(MAKEDEP_INCLUDES) \
	-DNO_EXTERNAL_INCLUDE_GUARDS 

DEP_FILE := $(DEP)/alldepends

$(DEP_FILE): $(ALL_SOURCES) $(ALL_HEADERS)
	$(SCRIPTS)/makedep $(DEPOPTIONS) $(ALL_SOURCES) > $@

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
