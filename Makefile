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
ifeq ($(ARCH),hp9000s700)
	CC := time aCC
	FILTER := |& sed -e '/Warning.*opt.aCC.include./,/\^\^*/d' \
		-e '/Warning.*usr.include./,/\^\^*/d'
	ARCH_FLAGS := +w +W818,655,392,495,469,361,749,416 -DACC_COMPILER -DHP9000S700
	SHLIB_FLAG := -b
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
	SHLIB_FLAG := 
endif

#-------------------------------------------------------------------------
#
# Compilation Options
#
#-------------------------------------------------------------------------

ifeq ($(ARCH),hp9000s700)
	DEBUG_OPTIONS := +Z +p
	DEBUG_OPTIM := +O1
	DEBUG_LINK_OPTIONS := -Wl,-B,immediate -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	DEBUG_OPTIONS :=
	DEBUG_OPTIM := -g -O1
	DEBUG_LINK_OPTIONS :=
endif

ifeq ($(ARCH),hp9000s700)
	PROD_OPTIONS := +p +Z
	PROD_OPTIM := +O3
	PROD_LINK_OPTIONS := -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	PROD_OPTIONS := 
	PROD_OPTIM := -O3
	PROD_LINK_OPTIONS := 
endif


#-------------------------------------------------------------------------
#
# Linker Options
#
#-------------------------------------------------------------------------

SHLIB_LD :=
ifeq ($(ARCH),hp9000s700)
	SHLIB_LD := $(CC) $(SHLIB_FLAG)
	SHLIB_CMD := $(SHLIB_LD) -o
	SHLIB_EXT := sl
	STATLIB_EXT := a
endif
ifeq ($(ARCH),irix6)
#	SHLIB_LD := ld -n32 -shared -check_registry /usr/lib32/so_locations
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

# directories for object files
GRSH := $(OBJ)
UTIL := $(OBJ)/util

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
	$(OBJ)/iostl.do \
	$(OBJ)/ioutils.do \
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

DEBUG_FLAGS := -DPROF -DASSERT -DINVARIANT -DTEST

DEBUG_GRSH_STATIC_OBJS := $(GRSH_STATIC_OBJS)
DEBUG_GRSH_DYNAMIC_OBJS := $(GRSH_DYNAMIC_OBJS)
DEBUG_TCLWORKS_OBJS := $(TCLWORKS_OBJS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET := $(HOME)/bin/$(ARCH)/grsh$(VERSION)

PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(SHLIB_EXT)
PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(SHLIB_EXT)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.
ifeq ($(ARCH),irix6)
	PROD_LIBVISX := $(LIB)/libvisx$(VERSION).$(STATLIB_EXT)
	PROD_LIBTCLWORKS := $(LIB)/libtclworks$(VERSION).$(STATLIB_EXT)
endif

PROD_GRSH_STATIC_OBJS := $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
PROD_GRSH_DYNAMIC_OBJS := $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_TCLWORKS_OBJS := $(DEBUG_TCLWORKS_OBJS:.do=.o)

#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS := $(ARCH_FLAGS) $(INCLUDE_DIRS)

ALL_PROD_OPTIONS := $(COMMON_OPTIONS) $(PROD_OPTIM) $(PROD_OPTIONS) 
ALL_DEBUG_OPTIONS := $(COMMON_OPTIONS) $(DEBUG_OPTIM) $(DEBUG_OPTIONS) $(DEBUG_FLAGS)

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
	rm -f RawCompileOut
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
# Dependency macros for header files
#
#   The idea with the levels: a header file can only depend on headers
#   that are at a strictly lower level than itself. There are no order
#   dependencies within a given level.
#
#-------------------------------------------------------------------------

#
# level 0 headers
#
ARRAYS_H := $(SRC)/util/arrays.h
BEZIER_H := $(SRC)/bezier.h
BITMAPREP_H := $(SRC)/bitmaprep.h
BMAPRENDERER_H := $(SRC)/bmaprenderer.h
CANVAS_H := $(SRC)/canvas.h
DEBUG_H := $(SRC)/util/debug.h
DEMANGLE_H := $(SRC)/demangle.h
DUMBPTR_H := $(SRC)/dumbptr.h
ERRMSG_H := $(SRC)/tcl/errmsg.h
ERROR_H := $(SRC)/util/error.h
EXPERIMENT_H := $(SRC)/experiment.h
IOSTL_H := $(SRC)/iostl.h
IOUTILS_H := $(SRC)/ioutils.h
OBJLISTTCL_H := $(SRC)/objlisttcl.h
OBJTOGL_H := $(SRC)/objtogl.h
OBSERVABLE_H := $(SRC)/observable.h
OBSERVER_H := $(SRC)/observer.h
PIPE_H := $(SRC)/util/pipe.h
POINT_H := $(SRC)/point.h
POINTERS_H := $(SRC)/util/pointers.h
POSLISTTCL_H := $(SRC)/poslisttcl.h
RAND_H := $(SRC)/rand.h
RANDUTILS_H := $(SRC)/randutils.h
STOPWATCH_H := $(SRC)/stopwatch.h
STRINGS_H := $(SRC)/util/strings.h
SYSTEM_H := $(SRC)/system.h
TCLDLIST_H := $(SRC)/tcl/tcldlist.h
TCLOBJLOCK_H := $(SRC)/tcl/tclobjlock.h
TCLPKG_H := $(SRC)/tcl/tclpkg.h
TLISTUTILS_H := $(SRC)/tlistutils.h
TRACE_H := $(SRC)/util/trace.h
TRACER_H := $(SRC)/util/tracer.h
TRACERTCL_H := $(SRC)/tcl/tracertcl.h
WIDGET_H := $(SRC)/widget.h

#
# level 1 headers
#
APPLICATION_H := $(ERROR_H) $(SRC)/application.h
BMAPDATA_H := $(ARRAYS_H) $(POINTERS_H) $(SRC)/bmapdata.h
FACTORY_H := $(ERROR_H) $(DEMANGLE_H) $(SRC)/factory.h
GLBMAPRENDERER_H := $(BMAPRENDERER_H) $(SRC)/glbmaprenderer.h
GLCANVAS_H := $(CANVAS_H) $(SRC)/glcanvas.h
IO_H := $(ERROR_H) $(SRC)/io.h
PBM_H := $(ERROR_H) $(SRC)/pbm.h
READER_H := $(ERROR_H) $(SRC)/reader.h
RECT_H := $(POINT_H) $(SRC)/rect.h
TCLERROR_H := $(ERROR_H) $(SRC)/tcl/tclerror.h
TCLITEMPKGBASE_H := $(TCLPKG_H) $(SRC)/tcl/tclitempkgbase.h
TCLLINK_H := $(STRINGS_H) $(SRC)/tcl/tcllink.h
VALUE_H := $(ERROR_H) $(SRC)/value.h
VOIDPTRLIST_H := $(ERROR_H) $(SRC)/voidptrlist.h
WRITER_H := $(ERROR_H) $(SRC)/writer.h
XBMAPRENDERER_H := $(BMAPRENDERER_H) $(SRC)/xbmaprenderer.h

#
# level 2 headers
#
ASCIISTREAMWRITER_H := $(WRITER_H) $(SRC)/asciistreamwriter.h
BLOCK_H := $(IO_H) $(SRC)/block.h
EXPTDRIVER_H := $(IO_H) $(ERROR_H) $(EXPERIMENT_H) $(SRC)/exptdriver.h
GROBJ_H := $(IO_H) $(OBSERVABLE_H) $(OBSERVER_H) $(TRACER_H) $(SRC)/grobj.h
GRSHAPP_H := $(APPLICATION_H) $(ERROR_H) $(SRC)/grshapp.h
IOFACTORY_H := $(FACTORY_H) $(IO_H) $(SRC)/iofactory.h
IOMGR_H := $(IO_H) $(SRC)/iomgr.h
IOPTRLIST_H := $(VOIDPTRLIST_H) $(IO_H) $(SRC)/ioptrlist.h
POSITION_H := $(IO_H) $(SRC)/position.h
PROPERTY_H := $(IO_H) $(OBSERVABLE_H) $(VALUE_H) $(SRC)/property.h
RESPONSE_H := $(VALUE_H) $(SRC)/response.h
RESPONSEHANDLER_H := $(IO_H) $(SRC)/responsehandler.h 
SOUND_H := $(ERROR_H) $(IO_H) $(SRC)/sound.h
SUBJECT_H := $(STRINGS_H) $(IO_H) $(SRC)/subject.h
TCLEVALCMD_H := $(TCLOBJLOCK_H) $(TCLERROR_H) $(SRC)/tcl/tclevalcmd.h
TCLVALUE_H := $(VALUE_H) $(SRC)/tcl/tclvalue.h
TIMINGHDLR_H := $(IO_H) $(STOPWATCH_H) $(SRC)/timinghdlr.h
TOGLCONFIG_H := $(POINTERS_H) $(RECT_H) $(WIDGET_H) $(SRC)/toglconfig.h
TRIAL_H := $(IO_H) $(VALUE_H) $(SRC)/trial.h
TRIALEVENT_H := $(IO_H) $(STOPWATCH_H) $(SRC)/trialevent.h
WRITEUTILS_H := $(WRITER_H) $(SRC)/writeutils.h

#
# level 3 headers
#
ASCIISTREAMREADER_H := $(READER_H) $(SRC)/asciistreamreader.h
BITMAP_H := $(GROBJ_H) $(SRC)/bitmap.h
EVENTRESPONSEHDLR_H := $(RESPONSEHANDLER_H) $(TRACER_H) $(SRC)/eventresponsehdlr.h
FACE_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/face.h
FISH_H := $(GROBJ_H) $(PROPERTY_H) $(TRACER_H) $(SRC)/fish.h
FIXPT_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/fixpt.h
GABOR_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/gabor.h
GTEXT_H := $(GROBJ_H) $(ERROR_H) $(STRINGS_H) $(SRC)/gtext.h
HOUSE_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/house.h
HPSOUND_CC := $(READER_H) $(WRITER_H) $(STRINGS_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/hpsound.cc
IRIXSOUND_CC := $(READER_H) $(WRITER_H) $(ARRAYS_H) $(STRINGS_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/irixsound.cc
JITTER_H := $(POSITION_H) $(SRC)/jitter.h
MASKHATCH_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/maskhatch.h
MORPHYFACE_H := $(GROBJ_H) $(PROPERTY_H) $(SRC)/morphyface.h
NULLRESPONSEHDLR_H := $(RESPONSEHANDLER_H) $(SRC)/nullresponsehdlr.h
PTRLIST_H := $(DUMBPTR_H) $(IOPTRLIST_H) $(SRC)/ptrlist.h
READUTILS_H := $(READER_H) $(IO_H) $(SRC)/readutils.h
TCLCMD_H := $(TCLVALUE_H) $(TCLERROR_H) $(SRC)/tcl/tclcmd.h
TIMINGHANDLER_H := $(TIMINGHDLR_H) $(SRC)/timinghandler.h
TLISTWIDGET_H := $(TOGLCONFIG_H) $(SRC)/tlistwidget.h

#
# level 4 headers
#
BLOCKLIST_H := $(PTRLIST_H) $(SRC)/blocklist.h
CLONEFACE_H := $(FACE_H) $(SRC)/cloneface.h
GLBITMAP_H := $(BITMAP_H) $(SRC)/glbitmap.h
KBDRESPONSEHDLR_H := $(EVENTRESPONSEHDLR_H) $(SRC)/kbdresponsehdlr.h
OBJLIST_H := $(PTRLIST_H) $(SRC)/objlist.h
POSLIST_H := $(PTRLIST_H) $(SRC)/poslist.h
PTRLIST_CC := $(PTRLIST_H) $(DEMANGLE_H) $(STRINGS_H) $(SRC)/ptrlist.cc
RHLIST_H := $(PTRLIST_H) $(SRC)/rhlist.h
SOUNDLIST_H := $(PTRLIST_H) $(SRC)/soundlist.h
STRINGIFYCMD_H := $(TCLCMD_H) $(SRC)/tcl/stringifycmd.h
THLIST_H := $(PTRLIST_H) $(SRC)/thlist.h
TLIST_H := $(PTRLIST_H) $(IO_H) $(SRC)/tlist.h
TCLITEMPKG_H := $(TCLITEMPKGBASE_H) $(TCLCMD_H) $(PROPERTY_H) \
	$(SRC)/tcl/tclitempkg.h
TCLVECCMDS_H := $(POINTERS_H) $(TCLCMD_H) $(SRC)/tcl/tclveccmds.h
XBITMAP_H := $(BITMAP_H) $(SRC)/xbitmap.h

#
# level 5 headers
#
LISTITEMPKG_H := $(TCLITEMPKG_H) $(DEMANGLE_H) $(SRC)/tcl/listitempkg.h
LISTPKG_H := $(TCLITEMPKG_H) $(IOPTRLIST_H) $(SRC)/tcl/listpkg.h

#
# level 6 headers
#
PROPITEMPKG_H := $(LISTITEMPKG_H) $(IOFACTORY_H) $(SRC)/tcl/propitempkg.h

#-------------------------------------------------------------------------
#
# Dependency macros for implementation files
#
#-------------------------------------------------------------------------

APPLICATION_CC := $(APPLICATION_H) $(TRACE_H) $(SRC)/application.cc

ASCIISTREAMREADER_CC := $(ASCIISTREAMREADER_H) $(IO_H) $(IOMGR_H)	\
	$(VALUE_H) $(TRACE_H) $(DEBUG_H) $(SRC)/asciistreamreader.cc

ASCIISTREAMWRITER_CC := $(ASCIISTREAMWRITER_H) $(IO_H) $(VALUE_H) \
	$(ARRAYS_H) $(STRINGS_H) \
	$(TRACE_H) $(DEBUG_H) $(DEMANGLE_H) $(SRC)/asciistreamwriter.cc

BITMAP_CC := $(BITMAP_H) $(BITMAPREP_H) $(PIPE_H) $(STRINGS_H)	\
	$(TRACE_H) $(DEBUG_H) $(SRC)/bitmap.cc

BITMAPREP_CC := $(BITMAPREP_H) $(APPLICATION_H) $(BMAPDATA_H)		\
	$(EXPERIMENT_H) $(BMAPRENDERER_H) $(CANVAS_H) $(IO_H) $(PBM_H)	\
	$(READER_H) $(RECT_H) $(STRINGS_H)										\
	$(WRITER_H) $(TRACE_H) $(DEBUG_H) $(SRC)/bitmaprep.cc

BITMAPTCL_CC := $(BITMAP_H) $(GLBITMAP_H) $(XBITMAP_H)						\
	$(IOFACTORY_H) $(OBJLIST_H)														\
	$(OBJTOGL_H) $(LISTITEMPKG_H) $(STRINGS_H) $(SYSTEM_H) $(TCLCMD_H)	\
	$(TCLOBJLOCK_H) $(TRACE_H) $(DEBUG_H) $(SRC)/bitmaptcl.cc

BLOCK_CC := $(BLOCK_H) $(EXPERIMENT_H) $(RAND_H) $(IOSTL_H)						\
	$(READER_H) $(READUTILS_H) $(RESPONSE_H) $(STRINGS_H) $(TLIST_H)			\
	$(TRIAL_H) $(WRITER_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/block.cc

BLOCKLIST_CC := $(BLOCKLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(BLOCK_H) $(PTRLIST_CC) $(SRC)/blocklist.cc

BLOCKTCL_CC := $(IOFACTORY_H) $(BLOCKLIST_H) $(BLOCK_H) $(TCLCMD_H)	\
	$(LISTITEMPKG_H) $(LISTPKG_H) $(TRACE_H) $(DEBUG_H) $(SRC)/blocktcl.cc

BMAPDATA_CC := $(BMAPDATA_H) $(TRACE_H) $(DEBUG_H) $(SRC)/bmapdata.cc

BMAPRENDERER_CC := $(BMAPRENDERER_H) $(TRACE_H) $(SRC)/bmaprenderer.cc

CANVAS_CC := $(CANVAS_H) $(SRC)/canvas.cc

CLONEFACE_CC := $(CLONEFACE_H) $(READUTILS_H) $(WRITEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/cloneface.cc

DEMANGLE_CC := $(DEMANGLE_H) $(TRACE_H) $(DEBUG_H) $(SRC)/demangle.cc

ERROR_CC := $(ERROR_H) $(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/util/error.cc

EVENTRESPONSEHDLR_CC := $(EVENTRESPONSEHDLR_H) $(ERROR_H) $(EXPERIMENT_H)	\
	$(SOUND_H) $(SOUNDLIST_H) $(READER_H) $(RESPONSE_H) $(TCLEVALCMD_H)		\
	$(TCLOBJLOCK_H) $(ARRAYS_H) $(STRINGS_H) $(WIDGET_H) $(WRITER_H)			\
	$(TRACE_H) $(DEBUG_H) $(SRC)/eventresponsehdlr.cc

EXPERIMENT_CC := $(EXPERIMENT_H) $(SRC)/experiment.cc

EXPTDRIVER_CC := $(EXPTDRIVER_H) $(BLOCK_H) $(TCLERROR_H)					\
	$(RESPONSEHANDLER_H) $(TCLEVALCMD_H) $(TIMINGHDLR_H) $(TLISTUTILS_H)	\
	$(TRIAL_H) $(OBJLIST_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(POSLIST_H)		\
	$(BLOCKLIST_H) $(RHLIST_H) $(THLIST_H) $(TLIST_H) $(READER_H)			\
	$(ARRAYS_H) $(STRINGS_H) $(WRITER_H)											\
	$(SYSTEM_H) $(STOPWATCH_H) $(TLISTWIDGET_H)									\
	$(TRACE_H) $(DEBUG_H) $(SRC)/exptdriver.cc

EXPTTCL_CC := $(ASCIISTREAMREADER_H) $(ASCIISTREAMWRITER_H) \
	$(GRSHAPP_H) $(TCLEVALCMD_H) $(EXPTDRIVER_H) \
	$(STRINGS_H) $(TCLITEMPKG_H) $(TCLCMD_H) $(WIDGET_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/expttcl.cc

EXPTTESTTCL_CC := $(TCLLINK_H) $(SRC)/expttesttcl.cc

FACE_CC := $(FACE_H) $(CANVAS_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) $(SRC)/face.cc

FACETCL_CC := $(CLONEFACE_H) $(IOFACTORY_) $(IOMGR_H) $(OBJLIST_H) $(FACE_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) $(SRC)/facetcl.cc

FACTORY_CC := $(FACTORY_H) $(SRC)/factory.cc

FISH_CC := $(FISH_H) $(ERROR_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/fish.cc

FISHTCL_CC := $(IOFACTORY_H) $(OBJLIST_H) $(LISTITEMPKG_H) $(FISH_H) \
	$(TRACERTCL_H) $(SRC)/fishtcl.cc

FIXPT_CC := $(FIXPT_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/fixpt.cc

FIXPTTCL_CC := $(OBJLIST_H) $(FIXPT_H) $(PROPITEMPKG_H) \
	$(TRACE_H) $(SRC)/fixpttcl.cc

GABOR_CC := $(GABOR_H) \
	$(RANDUTILS_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/gabor.cc

GABORTCL_CC := $(OBJLIST_H) $(PROPITEMPKG_H) $(GABOR_H) $(TRACE_H) $(SRC)/gabortcl.cc

GLBITMAP_CC := $(GLBITMAP_H) $(GLBMAPRENDERER_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/glbitmap.cc

GLBMAPRENDERER_CC := $(GLBMAPRENDERER_H) $(TRACE_H) $(SRC)/glbmaprenderer.cc

GLCANVAS_CC := $(GLCANVAS_H) $(ERROR_H) $(POINT_H) $(RECT_H) \
	$(DEBUG_H) $(TRACE_H) $(SRC)/glcanvas.cc

GROBJ_CC := $(GROBJ_H) $(BITMAPREP_H) $(CANVAS_H)				\
	$(GLBMAPRENDERER_H) $(ERROR_H) $(RECT_H) $(READER_H)		\
	$(POINTERS_H) $(WRITER_H) $(XBMAPRENDERER_H) $(TRACE_H)	\
	$(DEBUG_H) $(SRC)/grobj.cc

GROBJTCL_CC := $(APPLICATION_H) $(EXPERIMENT_H) $(GROBJ_H)	\
	$(OBJLIST_H) $(LISTITEMPKG_H) $(RECT_H) $(TCLVECCMDS_H)	\
	$(TRACERTCL_H) $(SRC)/grobjtcl.cc

GRSHAPP_CC := $(GRSHAPP_H) $(TRACE_H) $(SRC)/grshapp.cc

GRSHAPPINIT_CC := $(TRACE_H) $(GRSHAPP_H) $(SRC)/grshAppInit.cc

GTEXT_CC := $(GTEXT_H) $(READER_H) $(RECT_H) $(WRITER_H)	\
	$(TRACE_H) $(DEBUG_H) $(SRC)/gtext.cc

GTEXTTCL_CC := $(IOFACTORY_H) $(OBJLIST_H) $(GTEXT_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/gtexttcl.cc

HOUSE_CC := $(HOUSE_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/house.cc

HOUSETCL_CC := $(HOUSE_H) $(OBJLIST_H) $(PROPITEMPKG_H) $(TRACE_H)	\
	$(SRC)/housetcl.cc

IO_CC := $(IO_H) $(DEMANGLE_H) $(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/io.cc

IOFACTORY_CC := $(IOFACTORY_H) $(SRC)/iofactory.cc

IOMGR_CC := $(IOMGR_H) $(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/iomgr.cc

IOPTRLIST_CC := $(IOPTRLIST_H) $(IOMGR_H) \
	$(READUTILS_H) $(ARRAYS_H) $(STRINGS_H) $(WRITEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/ioptrlist.cc

IOSTL_CC := $(IOSTL_H) $(IO_H) $(SRC)/iostl.cc

IOUTILS_CC := $(IOUTILS_H) $(IO_H) $(SRC)/ioutils.cc

JITTER_CC := $(JITTER_H) $(RANDUTILS_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/jitter.cc

JITTERTCL_CC := $(IOFACTORY_H) $(JITTER_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TRACE_H) $(DEBUG_H) $(SRC)/jittertcl.cc

KBDRESPONSEHDLR_CC := $(KBDRESPONSEHDLR_H) $(STRINGS_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/kbdresponsehdlr.cc

LISTPKG_CC := $(LISTPKG_H) $(TRACE_H) $(SRC)/tcl/listpkg.cc

MASKHATCH_CC := $(MASKHATCH_H) $(READER_H) \
	$(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) $(SRC)/maskhatch.cc

MASKTCL_CC := $(OBJLIST_H) $(PROPITEMPKG_H) $(MASKHATCH_H) \
	$(TRACE_H) $(SRC)/masktcl.cc

MISCTCL_CC := $(RANDUTILS_H) $(SRC)/tcl/misctcl.cc

MORPHYFACE_CC := $(MORPHYFACE_H) $(BEZIER_H) $(CANVAS_H) \
	$(READER_H) $(RECT_H) \
	$(WRITER_H) $(TRACE_H) $(DEBUG_H) $(SRC)/morphyface.cc

MORPHYFACETCL_CC := $(OBJLIST_H) $(PROPITEMPKG_H) $(MORPHYFACE_H) \
	$(TRACE_H) $(SRC)/morphyfacetcl.cc

NULLRESPONSEHDLR_CC := $(NULLRESPONSEHDLR_H) \
	$(TRACE_H) $(SRC)/nullresponsehdlr.cc

OBJLIST_CC := $(OBJLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(GROBJ_H) $(PTRLIST_CC) $(SRC)/objlist.cc

OBJLISTTCL_CC := $(GROBJ_H) $(IOMGR_H) $(OBJLIST_H) $(LISTPKG_H) \
	$(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/objlisttcl.cc

OBJTOGL_CC := $(OBJTOGL_H) $(STRINGS_H) $(TCLCMD_H) \
	$(TCLEVALCMD_H) $(TCLITEMPKG_H) $(TLISTWIDGET_H) \
	$(TOGLCONFIG_H) $(XBMAPRENDERER_H)\
	$(TRACE_H) $(DEBUG_H) $(SRC)/objtogl.cc

OBSERVABLE_CC := $(OBSERVABLE_H) $(OBSERVER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/observable.cc

OBSERVER_CC := $(OBSERVER_H) $(TRACE_H) $(SRC)/observer.cc

PBM_CC := $(PBM_H) $(BMAPDATA_H) $(ARRAYS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/pbm.cc

POSITION_CC := $(POSITION_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/position.cc

POSITIONTCL_CC := $(IOFACTORY_H) $(POSITION_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) $(SRC)/positiontcl.cc

POSLIST_CC := $(POSLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(POSITION_H) $(PTRLIST_CC) $(SRC)/poslist.cc

POSLISTTCL_CC := $(POSLIST_H) $(LISTPKG_H) $(TRACE_H) $(SRC)/poslisttcl.cc

PROPERTY_CC := $(PROPERTY_H) $(READER_H) $(WRITER_H) \
	$(STRINGS_H) $(POINTERS_H) $(SRC)/property.cc

READER_CC := $(READER_H) $(STRINGS_H) $(SRC)/reader.cc

READUTILS_CC := $(READUTILS_H) $(STRINGS_H) $(SRC)/readutils.cc

RESPONSE_CC := $(RESPONSE_H) $(TRACE_H) $(SRC)/response.cc

RESPONSEHANDLER_CC := $(RESPONSEHANDLER_H) $(TRACE_H) $(SRC)/responsehandler.cc

RHLIST_CC := $(RHLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(RESPONSEHANDLER_H) $(PTRLIST_CC) $(SRC)/rhlist.cc

RHTCL_CC := $(EVENTRESPONSEHDLR_H) $(IOFACTORY_H) $(RHLIST_H)				\
	$(RESPONSEHANDLER_H) $(KBDRESPONSEHDLR_H)										\
	$(NULLRESPONSEHDLR_H) $(LISTITEMPKG_H) $(LISTPKG_H) $(TRACERTCL_H)	\
	$(TRACE_H) $(DEBUG_H) $(SRC)/rhtcl.cc

ifeq ($(ARCH),hp9000s700)
SOUND_CC := $(SOUND_H) $(HPSOUND_CC) $(SRC)/sound.cc
endif

ifeq ($(ARCH),irix6)
SOUND_CC := $(SOUND_H) $(IRIXSOUND_CC) $(SRC)/sound.cc
endif

SOUNDLIST_CC := $(SOUNDLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(SOUND_H) $(PTRLIST_CC) $(SRC)/soundlist.cc

SOUNDTCL_CC := $(SOUNDLIST_H) $(SOUND_H) $(LISTPKG_H) $(LISTITEMPKG_H) \
	$(TCLLINK_H) $(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/soundtcl.cc

STRINGIFYCMD_CC := $(STRINGIFYCMD_H) $(IO_H) $(ARRAYS_H) \
	$(ASCIISTREAMREADER_H) $(ASCIISTREAMWRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tcl/stringifycmd.cc

STRINGS_CC := $(STRINGS_H) $(SRC)/util/strings.cc

SUBJECT_CC := $(SUBJECT_H) $(IOUTILS_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/subject.cc

SUBJECTTCL_CC := $(ERRMSG_H) $(SUBJECT_H) $(TRACE_H) $(DEBUG_H) $(SRC)/subjecttcl.cc

SYSTEM_CC := $(SYSTEM_H) $(ARRAYS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/system.cc

TCLCMD_CC := $(TCLCMD_H) $(DEMANGLE_H) $(ERRMSG_H) $(TCLVALUE_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tcl/tclcmd.cc

TCLDLIST_CC := $(TCLDLIST_H) $(ERRMSG_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tcl/tcldlist.cc

TCLERROR_CC := $(TCLERROR_H) $(TRACE_H) $(SRC)/tcl/tclerror.cc

TCLGL_CC := $(TCLPKG_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tclgl.cc

TCLITEMPKG_CC := $(TCLITEMPKG_H) $(TCLCMD_H) $(STRINGIFYCMD_H) \
	$(TCLVECCMDS_H) $(STRINGS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/tcl/tclitempkg.cc

TCLPKG_CC := $(TCLPKG_H) $(TCLLINK_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tcl/tclpkg.cc

TCLVALUE_CC := $(TCLVALUE_H) $(TRACE_H) $(DEBUG_H) $(SRC)/tcl/tclvalue.cc

TCLVECCMDS_CC := $(TCLVECCMDS_H) $(TCLITEMPKGBASE_H) $(STRINGS_H) \
	$(DEBUG_H) $(TRACE_H) $(SRC)/tcl/tclveccmds.cc

THLIST_CC := $(THLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(TIMINGHDLR_H) $(PTRLIST_CC) $(SRC)/thlist.cc

THTCL_CC := $(IOFACTORY_H) $(THLIST_H) $(TCLCMD_H) $(TIMINGHDLR_H) \
	$(TIMINGHANDLER_H) $(TRIALEVENT_H) $(LISTITEMPKG_H) $(LISTPKG_H)\
	 $(TRACE_H) $(DEBUG_H) $(SRC)/thtcl.cc

TIMINGHANDLER_CC := $(TIMINGHANDLER_H) $(TRIALEVENT_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/timinghandler.cc

TIMINGHDLR_CC := $(TIMINGHDLR_H) $(IOMGR_H) $(TRIALEVENT_H) \
	$(READUTILS_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/timinghdlr.cc

TLIST_CC := $(TLIST_H) $(TRIAL_H) $(TRACE_H) $(DEBUG_H) \
	$(PTRLIST_CC) $(SRC)/tlist.cc

TLISTTCL_CC := $(APPLICATION_H) $(EXPERIMENT_H) \
	$(TLIST_H) $(TCLCMD_H) $(LISTPKG_H) \
	$(TLISTUTILS_H) $(TRACE_H) $(DEBUG_H) $(SRC)/tlisttcl.cc

TLISTUTILS_CC := $(TLISTUTILS_H) $(CANVAS_H) $(ERROR_H) \
	$(GTEXT_H) $(OBJLIST_H) \
	$(POSITION_H) $(POSLIST_H) $(RECT_H) $(TRIAL_H) $(TLIST_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tlistutils.cc

TLISTWIDGET_CC := $(TLISTWIDGET_H) $(CANVAS_H) $(TLIST_H) $(TRIAL_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/tlistwidget.cc

TOGLCONFIG_CC := $(TOGLCONFIG_H) $(ERROR_H) $(GLCANVAS_H) $(STRINGS_H) \
	$(TCLEVALCMD_H) $(TRACE_H) $(DEBUG_H) $(SRC)/toglconfig.cc

TRACE_CC := $(TRACE_H) $(SRC)/util/trace.cc

TRACERTCL_CC := $(TRACERTCL_H) $(TCLCMD_H) $(TCLPKG_H) $(TRACER_H) \
	$(DEBUG_H) $(SRC)/tcl/tracertcl.cc

TRIAL_CC := $(TRIAL_H) $(CANVAS_H) $(OBJLIST_H) $(POSLIST_H)	\
	$(GROBJ_H) $(POSITION_H) $(READER_H) $(READUTILS_H)			\
	$(RESPONSE_H) $(STRINGS_H) $(WRITER_H) $(WRITEUTILS_H)		\
	$(TRACE_H) $(DEBUG_H) $(SRC)/trial.cc

TRIALEVENT_CC := $(TRIALEVENT_H) $(CANVAS_H) $(DEMANGLE_H) $(ERROR_H) \
	$(EXPERIMENT_H) $(READER_H) \
	$(WRITER_H) $(TRACE_H) $(DEBUG_H) $(SRC)/trialevent.cc

TRIALTCL_CC := $(IOFACTORY_H) $(TRIAL_H) $(TLIST_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/trialtcl.cc

VALUE_CC := $(VALUE_H) $(SRC)/value.cc

VOIDPTRLIST_CC := $(VOIDPTRLIST_H) $(DEMANGLE_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/voidptrlist.cc

WIDGET_CC := $(WIDGET_H) $(SRC)/widget.cc

WRITER_CC := $(WRITER_H) $(STRINGS_H) $(SRC)/writer.cc

WRITEUTILS_CC := $(WRITEUTILS_H) $(STRINGS_H) $(SRC)/writeutils.cc

XBITMAP_CC := $(XBITMAP_H) $(XBMAPRENDERER_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/xbitmap.cc

XBMAPRENDERER_CC := $(XBMAPRENDERER_H) $(CANVAS_H) $(ERROR_H) $(POINT_H) \
	$(TRACE_H) $(DEBUG_H) $(SRC)/xbmaprenderer.cc

#-------------------------------------------------------------------------
#
# Dependency lists for object files
#
#-------------------------------------------------------------------------

$(OBJ)/application.*[ol]:       $(APPLICATION_CC)
$(OBJ)/asciistreamreader.*[ol]: $(ASCIISTREAMREADER_CC)
$(OBJ)/asciistreamwriter.*[ol]: $(ASCIISTREAMWRITER_CC)
$(OBJ)/bitmap.*[ol]:            $(BITMAP_CC)
$(OBJ)/bitmaprep.*[ol]:         $(BITMAPREP_CC)
$(OBJ)/bitmaptcl.*[ol]:         $(BITMAPTCL_CC)
$(OBJ)/block.*[ol]:             $(BLOCK_CC)
$(OBJ)/blocklist.*[ol]:         $(BLOCKLIST_CC)
$(OBJ)/blocktcl.*[ol]:          $(BLOCKTCL_CC)
$(OBJ)/bmapdata.*[ol]:          $(BMAPDATA_CC)
$(OBJ)/bmaprenderer.*[ol]:      $(BMAPRENDERER_CC)
$(OBJ)/canvas.*[ol]:            $(CANVAS_CC)
$(OBJ)/cloneface.*[ol]:         $(CLONEFACE_CC)
$(OBJ)/demangle.*[ol]:          $(DEMANGLE_CC)
$(OBJ)/eventresponsehdlr.*[ol]: $(EVENTRESPONSEHDLR_CC)
$(OBJ)/experiment.*[ol]:        $(EXPERIMENT_CC)
$(OBJ)/exptdriver.*[ol]:        $(EXPTDRIVER_CC)
$(OBJ)/expttcl.*[ol]:           $(EXPTTCL_CC)
$(OBJ)/expttesttcl.*[ol]:       $(EXPTTESTTCL_CC)
$(OBJ)/face.*[ol]:              $(FACE_CC)
$(OBJ)/facetcl.*[ol]:           $(FACETCL_CC)
$(OBJ)/factory.*[ol]:           $(FACTORY_CC)
$(OBJ)/fish.*[ol]:              $(FISH_CC)
$(OBJ)/fishtcl.*[ol]:           $(FISHTCL_CC)
$(OBJ)/fixpt.*[ol]:             $(FIXPT_CC)
$(OBJ)/fixpttcl.*[ol]:          $(FIXPTTCL_CC)
$(OBJ)/gabor.*[ol]:             $(GABOR_CC)
$(OBJ)/gabortcl.*[ol]:          $(GABORTCL_CC)
$(OBJ)/glbitmap.*[ol]:          $(GLBITMAP_CC)
$(OBJ)/glbmaprenderer.*[ol]:    $(GLBMAPRENDERER_CC)
$(OBJ)/glcanvas.*[ol]:          $(GLCANVAS_CC)
$(OBJ)/grobj.*[ol]:             $(GROBJ_CC)
$(OBJ)/grobjtcl.*[ol]:          $(GROBJTCL_CC)
$(OBJ)/grshapp.*[ol]:           $(GRSHAPP_CC)
$(OBJ)/grshAppInit.*[ol]:       $(GRSHAPPINIT_CC)
$(OBJ)/gtext.*[ol]:             $(GTEXT_CC)
$(OBJ)/gtexttcl.*[ol]:          $(GTEXTTCL_CC)
$(OBJ)/house.*[ol]:             $(HOUSE_CC)
$(OBJ)/housetcl.*[ol]:          $(HOUSETCL_CC)
$(OBJ)/io.*[ol]:                $(IO_CC)
$(OBJ)/iofactory.*[ol]:         $(IOFACTORY_CC)
$(OBJ)/iomgr.*[ol]:             $(IOMGR_CC)
$(OBJ)/ioptrlist.*[ol]:         $(IOPTRLIST_CC)
$(OBJ)/iostl.*[ol]:             $(IOSTL_CC)
$(OBJ)/ioutils.*[ol]:           $(IOUTILS_CC)
$(OBJ)/jitter.*[ol]:            $(JITTER_CC)
$(OBJ)/jittertcl.*[ol]:         $(JITTERTCL_CC)
$(OBJ)/kbdresponsehdlr.*[ol]:   $(KBDRESPONSEHDLR_CC)
$(OBJ)/maskhatch.*[ol]:         $(MASKHATCH_CC)
$(OBJ)/masktcl.*[ol]:           $(MASKTCL_CC)
$(OBJ)/morphyface.*[ol]:        $(MORPHYFACE_CC)
$(OBJ)/morphyfacetcl.*[ol]:     $(MORPHYFACETCL_CC)
$(OBJ)/nullresponsehdlr.*[ol]:  $(NULLRESPONSEHDLR_CC)
$(OBJ)/objlist.*[ol]:           $(OBJLIST_CC)
$(OBJ)/objlisttcl.*[ol]:        $(OBJLISTTCL_CC)
$(OBJ)/objtogl.*[ol]:           $(OBJTOGL_CC)
$(OBJ)/observable.*[ol]:        $(OBSERVABLE_CC)
$(OBJ)/observer.*[ol]:          $(OBSERVER_CC)
$(OBJ)/pbm.*[ol]:               $(PBM_CC)
$(OBJ)/position.*[ol]:          $(POSITION_CC)
$(OBJ)/positiontcl.*[ol]:       $(POSITIONTCL_CC)
$(OBJ)/poslist.*[ol]:           $(POSLIST_CC)
$(OBJ)/poslisttcl.*[ol]:        $(POSLISTTCL_CC)
$(OBJ)/property.*[ol]:          $(PROPERTY_CC)
$(OBJ)/reader.*[ol]:            $(READER_CC)
$(OBJ)/readutils.*[ol]:         $(READUTILS_CC)
$(OBJ)/response.*[ol]:          $(RESPONSE_CC)
$(OBJ)/responsehandler.*[ol]:   $(RESPONSEHANDLER_CC)
$(OBJ)/rhlist.*[ol]:            $(RHLIST_CC)
$(OBJ)/rhtcl.*[ol]:             $(RHTCL_CC)
$(OBJ)/sound.*[ol]:             $(SOUND_CC)
$(OBJ)/soundlist.*[ol]:         $(SOUNDLIST_CC)
$(OBJ)/soundtcl.*[ol]:          $(SOUNDTCL_CC)
$(OBJ)/subject.*[ol]:           $(SUBJECT_CC)
$(OBJ)/subjecttcl.*[ol]:        $(SUBJECTTCL_CC)
$(OBJ)/system.*[ol]:            $(SYSTEM_CC)
$(OBJ)/tclgl.*[ol]:             $(TCLGL_CC)
$(OBJ)/thlist.*[ol]:            $(THLIST_CC)
$(OBJ)/thtcl.*[ol]:             $(THTCL_CC)
$(OBJ)/timinghdlr.*[ol]:        $(TIMINGHDLR_CC)
$(OBJ)/timinghandler.*[ol]:     $(TIMINGHANDLER_CC)
$(OBJ)/tlist.*[ol]:             $(TLIST_CC)
$(OBJ)/tlisttcl.*[ol]:          $(TLISTTCL_CC)
$(OBJ)/tlistutils.*[ol]:        $(TLISTUTILS_CC)
$(OBJ)/tlistwidget.*[ol]:       $(TLISTWIDGET_CC)
$(OBJ)/toglconfig.*[ol]:        $(TOGLCONFIG_CC)
$(OBJ)/trial.*[ol]:             $(TRIAL_CC)
$(OBJ)/trialevent.*[ol]:        $(TRIALEVENT_CC)
$(OBJ)/trialtcl.*[ol]:          $(TRIALTCL_CC)
$(OBJ)/value.*[ol]:             $(VALUE_CC)
$(OBJ)/voidptrlist.*[ol]:       $(VOIDPTRLIST_CC)
$(OBJ)/widget.*[ol]:            $(WIDGET_CC)
$(OBJ)/writer.*[ol]:            $(WRITER_CC)
$(OBJ)/writeutils.*[ol]:        $(WRITEUTILS_CC)
$(OBJ)/xbitmap.*[ol]:           $(XBITMAP_CC)
$(OBJ)/xbmaprenderer.*[ol]:     $(XBMAPRENDERER_CC)

$(OBJ)/util/error.*[ol]:        $(ERROR_CC)
$(OBJ)/util/strings.[ol]:       $(STRINGS_CC)
$(OBJ)/util/trace.*[ol]:        $(TRACE_CC)

$(OBJ)/tcl/listpkg.*[ol]:       $(LISTPKG_CC)
$(OBJ)/tcl/misctcl.*[ol]:       $(MISCTCL_CC)
$(OBJ)/tcl/stringifycmd.*[ol]:  $(STRINGIFYCMD_CC)
$(OBJ)/tcl/tclcmd.*[ol]:        $(TCLCMD_CC)
$(OBJ)/tcl/tcldlist.*[ol]:      $(TCLDLIST_CC)
$(OBJ)/tcl/tclerror.*[ol]:      $(TCLERROR_CC)
$(OBJ)/tcl/tclitempkg.*[ol]:    $(TCLITEMPKG_CC)
$(OBJ)/tcl/tclpkg.*[ol]:        $(TCLPKG_CC)
$(OBJ)/tcl/tclvalue.*[ol]:      $(TCLVALUE_CC)
$(OBJ)/tcl/tclveccmds.*[ol]:    $(TCLVECCMDS_CC)
$(OBJ)/tcl/tracertcl.*[ol]:     $(TRACERTCL_CC)

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
	mv $(OBJ)/*.do $(OBJ)/util/*.do trash

# Make clean, and also remove all production object files
clean_o: clean
	mv $(OBJ)/*.o $(OBJ)/util/*.o trash

ALL_SOURCES := $(SRC)/*.[ch]* $(SRC)/util/*.[ch]* $(SRC)/tcl/*.[ch]*

# Generate TAGS file based on all source files
$(SRC)/TAGS: $(ALL_SOURCES)
ifeq ($(ARCH),hp9000s700)
	echo 'etags not installed'
else
	etags -f$(SRC)/TAGS $(ALL_SOURCES)
endif

# Count the lines in all source files
count:
	wc -l $(ALL_SOURCES)

# Count the number of non-commented source lines
ncsl:
	NCSL $(ALL_SOURCES)

# Start emacs and load all source files and Makefile
edit: clean
	emacs $(ALL_SOURCES) Makefile $(SRC)/Log.txt

docs: $(DOC)/DoxygenConfig $(SRC)/*.h $(SRC)/*.doc
	(doxygen $(DOC)/DoxygenConfig > $(DOC)/DocLog) >& $(DOC)/DocErrors

$(IDEP)/AdepAliases: $(IDEP)/FileList $(SRC)/*.h $(SRC)/*.cc
	adep -s -f$(IDEP)/FileList > $(IDEP)/AdepAliases

cdeps: $(ALL_SOURCES)
	cdep -i$(IDEP)/CdepSearchpath $(SRC)/*.h $(SRC)/*.cc > $(IDEP)/CdepDeps

ldeps: cdeps
	ldep -d$(IDEP)/CdepDeps -L > $(IDEP)/Ldeps

backup:
	tclsh $(SCRIPTS)/Backup.tcl

depend:
	makedepend -fDepends \
	-DACC_COMPILER -DLOCAL_PROF -I/opt/aCC/include -I/usr \
	 -I/opt/aCC/include/iostream -I/cit/rjpeters/include -I./util $(SRC)/*.cc
