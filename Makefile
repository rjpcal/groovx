##########################################################################
#
# Makefile 
#
#
# This is the Makefile for the grsh shell. This shell provides the
# following functionality:
# 
# 	1) Tcl/Tk 8.1 core
# 	2) OpenGL graphics, via the Togl widget
# 	3) A suite of major extensions that allow creation and execution of
# 		arbitrary visual psychophysics experiments
# 
#
# $Id$
#
##########################################################################

#-------------------------------------------------------------------------
#
# C++ Compiler, Architecture flags
#
#-------------------------------------------------------------------------

ARCH_FLAGS = 
CC = 
ifeq ($(ARCH),hp9000s700)
	CC = aCC
	ARCH_FLAGS = -DACC_COMPILER -DHP9000S700
	SHLIB_FLAG = -b
endif
ifeq ($(ARCH),irix6)
	CC = g++
	ARCH_FLAGS = -ansi -Wall -DGCC_COMPILER -DIRIX6
	SHLIB_FLAG = 
endif

#-------------------------------------------------------------------------
#
# Compilation Options
#
#-------------------------------------------------------------------------

ifeq ($(ARCH),hp9000s700)
	DEBUG_OPTIONS = +Z +p +w +W818,655,392,495,469,361,749,416
	DEBUG_OPTIM = +O1
	DEBUG_LINK_OPTIONS = -Wl,-B,immediate -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	DEBUG_OPTIONS =
	DEBUG_OPTIM = -g -O1
	DEBUG_LINK_OPTIONS =
endif

ifeq ($(ARCH),hp9000s700)
	PROD_OPTIONS = +p +Z
	PROD_OPTIM = +O2
	PROD_LINK_OPTIONS = -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	PROD_OPTIONS = 
	PROD_OPTIM = -O3
	PROD_LINK_OPTIONS = 
endif


#-------------------------------------------------------------------------
#
# Linker Options
#
#-------------------------------------------------------------------------

SHLIB_LD =
ifeq ($(ARCH),hp9000s700)
	SHLIB_LD = $(CC) $(SHLIB_FLAG)
	SHLIB_CMD = $(SHLIB_LD) -o
	SHLIB_EXT = sl
	STATLIB_EXT = a
endif
ifeq ($(ARCH),irix6)
#	SHLIB_LD = ld -n32 -shared -check_registry /usr/lib32/so_locations
	STATLIB_CMD = ar rus
	SHLIB_EXT = so
	STATLIB_EXT = a
endif

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------


ifeq ($(ARCH),irix6)
	STL_INCLUDE_DIR = -I$(HOME)/gcc/include/g++-3
else
	STL_INCLUDE_DIR =
endif

INCLUDE_DIRS = -I$(HOME)/include -I$(HOME)/grsh/util $(STL_INCLUDE_DIR)

ifeq ($(ARCH),hp9000s700)
	OPENGL_LIB_DIR = -L/opt/graphics/OpenGL/lib
	AUDIO_LIB_DIR = -L/opt/audio/lib
	AUDIO_LIB = -lAlib
	RPATH_DIR = 
endif
ifeq ($(ARCH),irix6)
	OPENGL_LIB_DIR =
	AUDIO_LIB_DIR =
	AUDIO_LIB = -laudio -laudiofile
	RPATH_DIR = -Wl,-rpath,$(HOME)/grsh/$(ARCH)
endif

LIB_DIRS =  -L$(HOME)/grsh/$(ARCH) \
	-L$(HOME)/lib/$(ARCH) \
	$(OPENGL_LIB_DIR) \
	$(AUDIO_LIB_DIR) \
	$(RPATH_DIR)

LIBRARIES = \
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
GRSH = ./$(ARCH)
UTIL = ./util/$(ARCH)

GRSH_STATIC_OBJS = \
	$(GRSH)/bitmap.do \
	$(GRSH)/bitmaprep.do \
	$(GRSH)/face.do \
	$(GRSH)/fish.do \
	$(GRSH)/fixpt.do \
	$(GRSH)/gabor.do \
	$(GRSH)/glbitmap.do \
	$(GRSH)/glcanvas.do \
	$(GRSH)/glbmaprenderer.do \
	$(GRSH)/grobj.do \
	$(GRSH)/grshAppInit.do \
	$(GRSH)/gtext.do \
	$(GRSH)/house.do \
	$(GRSH)/jitter.do \
	$(GRSH)/maskhatch.do \
	$(GRSH)/morphyface.do \
	$(GRSH)/objtogl.do \
	$(GRSH)/position.do \
	$(GRSH)/tclgl.do \
	$(GRSH)/tlistwidget.do \
	$(GRSH)/toglconfig.do \
	$(GRSH)/xbitmap.do \
	$(GRSH)/xbmaprenderer.do \

GRSH_DYNAMIC_OBJS = \
	$(GRSH)/application.do \
	$(GRSH)/asciistreamreader.do \
	$(GRSH)/asciistreamwriter.do \
	$(GRSH)/bitmaptcl.do \
	$(GRSH)/block.do \
	$(GRSH)/blocklist.do \
	$(GRSH)/blocktcl.do \
	$(GRSH)/bmapdata.do \
	$(GRSH)/bmaprenderer.do \
	$(GRSH)/canvas.do \
	$(GRSH)/cloneface.do \
	$(GRSH)/demangle.do \
	$(UTIL)/error.do \
	$(GRSH)/eventresponsehdlr.do \
	$(GRSH)/experiment.do \
	$(GRSH)/exptdriver.do \
	$(GRSH)/expttcl.do \
	$(GRSH)/expttesttcl.do \
	$(GRSH)/facetcl.do \
	$(GRSH)/factory.do \
	$(GRSH)/fishtcl.do \
	$(GRSH)/fixpttcl.do \
	$(GRSH)/gabortcl.do \
	$(GRSH)/grobjtcl.do \
	$(GRSH)/grshapp.do \
	$(GRSH)/gtexttcl.do \
	$(GRSH)/housetcl.do \
	$(GRSH)/io.do \
	$(GRSH)/iofactory.do \
	$(GRSH)/iomgr.do \
	$(GRSH)/ioptrlist.do \
	$(GRSH)/iostl.do \
	$(GRSH)/ioutils.do \
	$(GRSH)/jittertcl.do \
	$(GRSH)/kbdresponsehdlr.do \
	$(GRSH)/masktcl.do \
	$(GRSH)/morphyfacetcl.do \
	$(GRSH)/nullresponsehdlr.do \
	$(GRSH)/objlist.do \
	$(GRSH)/objlisttcl.do \
	$(GRSH)/observable.do \
	$(GRSH)/observer.do \
	$(GRSH)/pbm.do \
	$(GRSH)/positiontcl.do \
	$(GRSH)/poslist.do \
	$(GRSH)/poslisttcl.do \
	$(GRSH)/property.do \
	$(GRSH)/reader.do \
	$(GRSH)/readutils.do \
	$(GRSH)/responsehandler.do \
	$(GRSH)/rhlist.do \
	$(GRSH)/rhtcl.do \
	$(GRSH)/sound.do \
	$(GRSH)/soundlist.do \
	$(GRSH)/soundtcl.do \
	$(GRSH)/subject.do \
	$(GRSH)/subjecttcl.do \
	$(GRSH)/system.do \
	$(GRSH)/thlist.do \
	$(GRSH)/thtcl.do \
	$(GRSH)/timinghandler.do \
	$(GRSH)/timinghdlr.do \
	$(GRSH)/tlist.do \
	$(GRSH)/tlisttcl.do \
	$(GRSH)/tlistutils.do \
	$(UTIL)/trace.do \
	$(GRSH)/trial.do \
	$(GRSH)/trialevent.do \
	$(GRSH)/trialtcl.do \
	$(GRSH)/value.do \
	$(GRSH)/voidptrlist.do \
	$(GRSH)/widget.do \
	$(GRSH)/writer.do \
	$(GRSH)/writeutils.do \

TCLWORKS_OBJS = \
	$(GRSH)/listpkg.do \
	$(GRSH)/misctcl.do \
	$(GRSH)/stringifycmd.do \
	$(GRSH)/tclcmd.do \
	$(GRSH)/tcldlist.do \
	$(GRSH)/tclerror.do \
	$(GRSH)/tclitempkg.do \
	$(GRSH)/tclpkg.do \
	$(GRSH)/tclvalue.do \
	$(GRSH)/tclveccmds.do \
	$(GRSH)/tracertcl.do \


#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS = $(ARCH_FLAGS) $(INCLUDE_DIRS)

ALL_PROD_OPTIONS = $(COMMON_OPTIONS) $(PROD_OPTIM) $(PROD_OPTIONS) 
ALL_DEBUG_OPTIONS = $(COMMON_OPTIONS) $(DEBUG_OPTIM) $(DEBUG_OPTIONS) $(DEBUG_FLAGS)

$(GRSH)/%.o : %.cc
	$(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)

$(UTIL)/%.o : util/%.cc
	$(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)

$(GRSH)/%.do : %.cc
	$(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)

$(UTIL)/%.do : util/%.cc
	$(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)

#-------------------------------------------------------------------------
#
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

DEBUG_TARGET = $(HOME)/bin/$(ARCH)/testsh

DEBUG_LIBVISX = $(GRSH)/libvisx.d.$(SHLIB_EXT)
DEBUG_LIBTCLWORKS = $(GRSH)/libtclworks.d.$(SHLIB_EXT)

DEBUG_FLAGS = -DPROF -DASSERT -DINVARIANT -DTEST

DEBUG_GRSH_STATIC_OBJS = $(GRSH_STATIC_OBJS)
DEBUG_GRSH_DYNAMIC_OBJS = $(GRSH_DYNAMIC_OBJS)
DEBUG_TCLWORKS_OBJS = $(TCLWORKS_OBJS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET = $(HOME)/bin/$(ARCH)/grsh0.7a2

PROD_LIBVISX = $(GRSH)/libvisx.$(SHLIB_EXT)
PROD_LIBTCLWORKS = $(GRSH)/libtclworks.$(SHLIB_EXT)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.
ifeq ($(ARCH),irix6)
	PROD_LIBVISX = $(GRSH)/libvisx.$(STATLIB_EXT)
	PROD_LIBTCLWORKS = $(GRSH)/libtclworks.$(STATLIB_EXT)
endif

PROD_GRSH_STATIC_OBJS = $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
PROD_GRSH_DYNAMIC_OBJS = $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_TCLWORKS_OBJS = $(DEBUG_TCLWORKS_OBJS:.do=.o)

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

testsh: TAGS $(DEBUG_TARGET)
	$(DEBUG_TARGET) ./testing/grshtest.tcl

ALL_DEBUG_DEPENDS = \
	$(DEBUG_GRSH_STATIC_OBJS) \
	$(DEBUG_LIBVISX) \
	$(DEBUG_LIBTCLWORKS)

$(DEBUG_TARGET): $(ALL_DEBUG_DEPENDS)
	$(CC) $(DEBUG_LINK_OPTIONS) -o $@ $(DEBUG_GRSH_STATIC_OBJS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) -lvisx.d -ltclworks.d $(LIBRARIES) 

grsh: TAGS $(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

ALL_PROD_DEPENDS = \
	$(PROD_GRSH_STATIC_OBJS) \
	$(PROD_LIBVISX) \
	$(PROD_LIBTCLWORKS)

$(PROD_TARGET): $(ALL_PROD_DEPENDS)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_GRSH_STATIC_OBJS) \
	$(LIB_DIRS) -lvisx -ltclworks $(LIBRARIES)

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
BEZIER_H = bezier.h
BMAPDATA_H = bmapdata.h
BMAPRENDERER_H = bmaprenderer.h
CANVAS_H = canvas.h
DEBUG_H = util/debug.h
DEMANGLE_H = demangle.h
DUMBPTR_H = dumbptr.h
ERRMSG_H = errmsg.h
ERROR_H = util/error.h
EXPERIMENT_H = experiment.h
IOSTL_H = iostl.h
IOUTILS_H = ioutils.h
OBJLISTTCL_H = objlisttcl.h
OBJTOGL_H = objtogl.h
OBSERVABLE_H = observable.h
OBSERVER_H = observer.h
PIPE_H = util/pipe.h
POINT_H = point.h
POSLISTTCL_H = poslisttcl.h
RAND_H = rand.h
RANDUTILS_H = randutils.h
STOPWATCH_H = stopwatch.h
SYSTEM_H = system.h
TCLDLIST_H = tcldlist.h
TCLLINK_H = tcllink.h
TCLOBJLOCK_H = tclobjlock.h
TCLPKG_H = tclpkg.h
TLISTUTILS_H = tlistutils.h
TRACE_H = util/trace.h
TRACER_H = util/tracer.h
TRACERTCL_H = tracertcl.h
WIDGET_H = widget.h

#
# level 1 headers
#
APPLICATION_H = $(ERROR_H) application.h
BITMAPREP_H = $(BMAPDATA_H) bitmaprep.h
FACTORY_H = $(ERROR_H) $(DEMANGLE_H) factory.h
GLBMAPRENDERER_H = $(BMAPRENDERER_H) glbmaprenderer.h
GLCANVAS_H = $(CANVAS_H) glcanvas.h
IO_H = $(ERROR_H) io.h
PBM_H = $(ERROR_H) $(BMAPDATA_H) pbm.h
READER_H = $(ERROR_H) reader.h
RECT_H = $(POINT_H) rect.h
TCLERROR_H = $(ERROR_H) tclerror.h
TCLITEMPKGBASE_H = $(TCLPKG_H) tclitempkgbase.h
VALUE_H = $(ERROR_H) value.h
VOIDPTRLIST_H = $(ERROR_H) voidptrlist.h
WRITER_H = $(ERROR_H) writer.h
XBMAPRENDERER_H = $(BMAPRENDERER_H) xbmaprenderer.h

#
# level 2 headers
#
ASCIISTREAMWRITER_H = $(WRITER_H) asciistreamwriter.h
BLOCK_H = $(IO_H) $(STOPWATCH_H) block.h
EXPTDRIVER_H = $(IO_H) $(ERROR_H) $(EXPERIMENT_H) exptdriver.h
GROBJ_H = $(IO_H) $(OBSERVABLE_H) $(OBSERVER_H) $(TRACER_H) grobj.h
GRSHAPP_H = $(APPLICATION_H) $(ERROR_H) grshapp.h
IOFACTORY_H = $(FACTORY_H) $(IO_H) iofactory.h
IOPTRLIST_H = $(VOIDPTRLIST_H) $(IO_H) ioptrlist.h
POSITION_H = $(IO_H) position.h
PROPERTY_H = $(IO_H) $(OBSERVABLE_H) $(VALUE_H) property.h
RESPONSEHANDLER_H = $(IO_H) responsehandler.h 
SOUND_H = $(ERROR_H) $(IO_H) sound.h
SUBJECT_H = $(IO_H) subject.h
TCLEVALCMD_H = $(TCLOBJLOCK_H) $(TCLERROR_H) tclevalcmd.h
TCLVALUE_H = $(VALUE_H) tclvalue.h
TIMINGHDLR_H = $(IO_H) $(STOPWATCH_H) timinghdlr.h
TOGLCONFIG_H = $(RECT_H) $(WIDGET_H) toglconfig.h
TRIAL_H = $(IO_H) $(ERROR_H) $(VALUE_H) trial.h
TRIALEVENT_H = $(IO_H) $(STOPWATCH_H) trialevent.h
WRITEUTILS_H = $(WRITER_H) writeutils.h

#
# level 3 headers
#
ASCIISTREAMREADER_H = $(READER_H) asciistreamreader.h
BITMAP_H = $(GROBJ_H) bitmap.h
EVENTRESPONSEHDLR_H = $(RESPONSEHANDLER_H) eventresponsehdlr.h
FACE_H = $(GROBJ_H) $(PROPERTY_H) face.h
FISH_H = $(GROBJ_H) $(PROPERTY_H) $(TRACER_H) fish.h
FIXPT_H = $(GROBJ_H) $(PROPERTY_H) fixpt.h
GABOR_H = $(GROBJ_H) $(PROPERTY_H) gabor.h
GTEXT_H = $(GROBJ_H) $(ERROR_H) gtext.h
HOUSE_H = $(GROBJ_H) $(PROPERTY_H) house.h
HPSOUND_CC = $(TRACE_H) $(DEBUG_H) $(READER_H) $(WRITER_H) hpsound.cc
IOMGR_H = $(IO_H) $(IOFACTORY_H) iomgr.h
IRIXSOUND_CC = $(TRACE_H) $(DEBUG_H) $(READER_H) $(WRITER_H) irixsound.cc
JITTER_H = $(POSITION_H) jitter.h
MASKHATCH_H = $(GROBJ_H) $(PROPERTY_H) maskhatch.h
MORPHYFACE_H = $(GROBJ_H) $(PROPERTY_H) morphyface.h
NULLRESPONSEHDLR_H = $(RESPONSEHANDLER_H) nullresponsehdlr.h
PTRLIST_H = $(DUMBPTR_H) $(IOPTRLIST_H) ptrlist.h
READUTILS_H = $(READER_H) $(IO_H) readutils.h
TCLCMD_H = $(TCLVALUE_H) $(TCLERROR_H) tclcmd.h
TIMINGHANDLER_H = $(TIMINGHDLR_H) timinghandler.h
TLISTWIDGET_H = $(TOGLCONFIG_H) tlistwidget.h

#
# level 4 headers
#
BLOCKLIST_H = $(PTRLIST_H) blocklist.h
CLONEFACE_H = $(FACE_H) cloneface.h
GLBITMAP_H = $(BITMAP_H) glbitmap.h
KBDRESPONSEHDLR_H = $(EVENTRESPONSEHDLR_H) kbdresponsehdlr.h
OBJLIST_H = $(PTRLIST_H) objlist.h
POSLIST_H = $(PTRLIST_H) poslist.h
PTRLIST_CC = $(PTRLIST_H) $(DEMANGLE_H) ptrlist.cc
RHLIST_H = $(PTRLIST_H) rhlist.h
SOUNDLIST_H = $(PTRLIST_H) soundlist.h
STRINGIFYCMD_H = $(TCLCMD_H) stringifycmd.h
THLIST_H = $(PTRLIST_H) thlist.h
TLIST_H = $(PTRLIST_H) $(IO_H) tlist.h
TCLITEMPKG_H = $(TCLITEMPKGBASE_H) $(TCLCMD_H) $(PROPERTY_H) tclitempkg.h
TCLVECCMDS_H = $(TCLCMD_H) tclveccmds.h
XBITMAP_H = $(BITMAP_H) xbitmap.h

#
# level 5 headers
#
LISTITEMPKG_H = $(TCLITEMPKG_H) $(DEMANGLE_H) listitempkg.h
LISTPKG_H = $(TCLITEMPKG_H) $(IOPTRLIST_H) listpkg.h

#
# level 6 headers
#
PROPITEMPKG_H = $(LISTITEMPKG_H) $(IOFACTORY_H) propitempkg.h

#-------------------------------------------------------------------------
#
# Dependency macros for implementation files
#
#-------------------------------------------------------------------------

APPLICATION_CC = $(APPLICATION_H) $(TRACE_H) application.cc

ASCIISTREAMREADER_CC = $(ASCIISTREAMREADER_H) $(IO_H) $(IOMGR_H) $(VALUE_H) \
	$(TRACE_H) $(DEBUG_H) asciistreamreader.cc

ASCIISTREAMWRITER_CC = $(ASCIISTREAMWRITER_H) $(IO_H) $(VALUE_H) \
	$(TRACE_H) $(DEBUG_H) $(DEMANGLE_H) asciistreamwriter.cc

BITMAP_CC = $(BITMAP_H) $(BITMAPREP_H) $(PIPE_H) $(TRACE_H) $(DEBUG_H) bitmap.cc

BITMAPREP_CC = $(BITMAPREP_H) $(APPLICATION_H) \
	$(EXPERIMENT_H) $(BMAPRENDERER_H) $(CANVAS_H) \
	$(ERROR_H) $(IO_H) $(PBM_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) bitmaprep.cc

BITMAPTCL_CC = $(BITMAP_H) $(GLBITMAP_H) $(XBITMAP_H) \
	$(IOFACTORY_H) $(OBJLIST_H) \
	$(OBJTOGL_H) $(LISTITEMPKG_H) $(SYSTEM_H) $(TCLCMD_H) \
	$(TCLOBJLOCK_H) $(TRACE_H) $(DEBUG_H) bitmaptcl.cc

BLOCK_CC = $(BLOCK_H) $(EXPERIMENT_H) $(RAND_H) $(IOSTL_H) \
	$(READER_H) $(READUTILS_H) $(TLIST_H) $(TRIAL_H) \
	$(WRITER_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) block.cc

BLOCKLIST_CC = $(BLOCKLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(BLOCK_H) $(PTRLIST_CC) blocklist.cc

BLOCKTCL_CC = $(IOFACTORY_H) $(BLOCKLIST_H) $(BLOCK_H) $(TCLCMD_H) \
	$(LISTITEMPKG_H) $(LISTPKG_H) $(TRACE_H) $(DEBUG_H) blocktcl.cc

BMAPDATA_CC = $(BMAPDATA_H) $(TRACE_H) $(DEBUG_H) bmapdata.cc

BMAPRENDERER_CC = $(BMAPRENDERER_H) $(TRACE_H) bmaprenderer.cc

CANVAS_CC = $(CANVAS_H) canvas.cc

CLONEFACE_CC = $(CLONEFACE_H) $(READUTILS_H) $(WRITEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) cloneface.cc

DEMANGLE_CC = $(DEMANGLE_H) $(TRACE_H) $(DEBUG_H) demangle.cc

ERROR_CC = $(ERROR_H) $(TRACE_H) $(DEBUG_H) util/error.cc

EVENTRESPONSEHDLR_CC = $(EVENTRESPONSEHDLR_H) $(ERROR_H) $(EXPERIMENT_H) \
	$(SOUND_H) $(SOUNDLIST_H) $(READER_H) \
	$(TCLEVALCMD_H) $(TCLOBJLOCK_H) $(WIDGET_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) eventresponsehdlr.cc

EXPERIMENT_CC = $(EXPERIMENT_H) experiment.cc

EXPTDRIVER_CC = $(EXPTDRIVER_H) $(BLOCK_H) $(TCLERROR_H) $(READER_H) \
	$(RESPONSEHANDLER_H) $(TCLEVALCMD_H) $(TIMINGHDLR_H) $(TLISTUTILS_H) \
	$(TRIAL_H) $(OBJLIST_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(POSLIST_H) \
	$(BLOCKLIST_H) $(RHLIST_H) $(THLIST_H) $(TLIST_H) $(WRITER_H) \
	$(SYSTEM_H) $(STOPWATCH_H) $(TLISTWIDGET_H) \
	$(TRACE_H) $(DEBUG_H) exptdriver.cc

EXPTTCL_CC = $(ASCIISTREAMREADER_H) $(ASCIISTREAMWRITER_H) \
	$(GRSHAPP_H) $(TCLEVALCMD_H) $(EXPTDRIVER_H) \
	$(TCLITEMPKG_H) $(TCLCMD_H) $(WIDGET_H) \
	$(TRACE_H) $(DEBUG_H) expttcl.cc

EXPTTESTTCL_CC = $(TCLLINK_H) expttesttcl.cc

FACE_CC = $(FACE_H) $(CANVAS_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) face.cc

FACETCL_CC = $(CLONEFACE_H) $(IOMGR_H) $(OBJLIST_H) $(FACE_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) facetcl.cc

FACTORY_CC = $(FACTORY_H) factory.cc

FISH_CC = $(FISH_H) $(ERROR_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) fish.cc

FISHTCL_CC = $(IOFACTORY_H) $(OBJLIST_H) $(LISTITEMPKG_H) $(FISH_H) \
	$(TRACERTCL_H) fishtcl.cc

FIXPT_CC = $(FIXPT_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) fixpt.cc

FIXPTTCL_CC = $(OBJLIST_H) $(FIXPT_H) $(PROPITEMPKG_H) \
	$(TRACE_H) fixpttcl.cc

GABOR_CC = $(GABOR_H) $(RANDUTILS_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) gabor.cc

GABORTCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(GABOR_H) $(TRACE_H) gabortcl.cc

GLBITMAP_CC = $(GLBITMAP_H) $(GLBMAPRENDERER_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) glbitmap.cc

GLBMAPRENDERER_CC = $(GLBMAPRENDERER_H) $(TRACE_H) glbmaprenderer.cc

GLCANVAS_CC = $(GLCANVAS_H) $(ERROR_H) $(POINT_H) $(RECT_H) \
	$(DEBUG_H) $(TRACE_H) glcanvas.cc

GROBJ_CC = $(GROBJ_H) $(BITMAPREP_H) $(CANVAS_H) \
	$(GLBMAPRENDERER_H) $(ERROR_H) $(RECT_H) $(READER_H) \
	$(WRITER_H) $(XBMAPRENDERER_H) $(TRACE_H) $(DEBUG_H) grobj.cc

GROBJTCL_CC = $(APPLICATION_H) $(EXPERIMENT_H) $(GROBJ_H) \
	$(OBJLIST_H) $(LISTITEMPKG_H) $(RECT_H) $(TCLVECCMDS_H) \
	$(TRACERTCL_CC) grobjtcl.cc

GRSHAPP_CC = $(GRSHAPP_H) $(TRACE_H) grshapp.cc

GRSHAPPINIT_CC = $(TRACE_H) $(GRSHAPP_H) grshAppInit.cc

GTEXT_CC = $(GTEXT_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) gtext.cc

GTEXTTCL_CC = $(IOFACTORY_H) $(OBJLIST_H) $(GTEXT_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) gtexttcl.cc

HOUSE_CC = $(HOUSE_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) house.cc

HOUSETCL_CC = $(HOUSE_H) $(OBJLIST_H) $(PROPITEMPKG_H) $(TRACE_H) housetcl.cc

IO_CC = $(IO_H) $(DEMANGLE_H) $(TRACE_H) $(DEBUG_H) io.cc

IOFACTORY_CC = $(IOFACTORY_H) iofactory.cc

IOMGR_CC = $(IOMGR_H) $(TRACE_H) $(DEBUG_H) iomgr.cc

IOPTRLIST_CC = $(IOPTRLIST_H) $(DEMANGLE_H) $(IOMGR_H) \
	$(READUTILS_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) ioptrlist.cc

IOSTL_CC = $(IOSTL_H) $(IO_H) iostl.cc

IOUTILS_CC = $(IOUTILS_H) $(IO_H) ioutils.cc

JITTER_CC = $(JITTER_H) $(RANDUTILS_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) jitter.cc

JITTERTCL_CC = $(IOFACTORY_H) $(JITTER_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TRACE_H) $(DEBUG_H) jittertcl.cc

KBDRESPONSEHDLR_CC = $(KBDRESPONSEHDLR_H) \
	$(TRACE_H) $(DEBUG_H) kbdresponsehdlr.cc

LISTPKG_CC = $(LISTPKG_H) $(TRACE_H) listpkg.cc

MASKHATCH_CC = $(MASKHATCH_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) maskhatch.cc

MASKTCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(MASKHATCH_H) \
	$(TRACE_H) masktcl.cc

MISCTCL_CC = $(RANDUTILS_H) misctcl.cc

MORPHYFACE_CC = $(MORPHYFACE_H) $(BEZIER_H) $(CANVAS_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) morphyface.cc

MORPHYFACETCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(MORPHYFACE_H) \
	$(TRACE_H) morphyfacetcl.cc

NULLRESPONSEHDLR_CC = $(NULLRESPONSEHDLR_H) \
	$(TRACE_H) nullresponsehdlr.cc

OBJLIST_CC = $(OBJLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(GROBJ_H) $(PTRLIST_CC) objlist.cc

OBJLISTTCL_CC = $(GROBJ_H) $(IOMGR_H) $(OBJLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) objlisttcl.cc

OBJTOGL_CC = $(OBJTOGL_H) $(TCLCMD_H) \
	$(TCLEVALCMD_H) $(TCLITEMPKG_H) $(TLISTWIDGET_H) \
	$(TOGLCONFIG_H) $(XBMAPRENDERER_H)\
	$(TRACE_H) $(DEBUG_H) objtogl.cc

OBSERVABLE_CC = $(OBSERVABLE_H) $(OBSERVER_H) \
	$(TRACE_H) $(DEBUG_H) observable.cc

OBSERVER_CC = $(OBSERVER_H) $(TRACE_H) observer.cc

PBM_CC = $(PBM_H) $(TRACE_H) $(DEBUG_H) pbm.cc

POSITION_CC = $(POSITION_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) position.cc

POSITIONTCL_CC = $(IOFACTORY_H) $(POSITION_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) positiontcl.cc

POSLIST_CC = $(POSLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(POSITION_H) $(PTRLIST_CC) poslist.cc

POSLISTTCL_CC = $(POSLIST_H) $(LISTPKG_H) $(TRACE_H) poslisttcl.cc

PROPERTY_CC = $(PROPERTY_H) property.cc

READER_CC = $(READER_H) reader.cc

READUTILS_CC = $(READUTILS_H) readutils.cc

RESPONSEHANDLER_CC = $(RESPONSEHANDLER_H) $(TRACE_H) responsehandler.cc

RHLIST_CC = $(RHLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(RESPONSEHANDLER_H) $(PTRLIST_CC) rhlist.cc

RHTCL_CC = $(EVENTRESPONSEHDLR_H) $(IOFACTORY_H) $(RHLIST_H) \
	$(RESPONSEHANDLER_H) $(KBDRESPONSEHDLR_H) \
	$(NULLRESPONSEHDLR_H) $(LISTITEMPKG_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) rhtcl.cc

ifeq ($(ARCH),hp9000s700)
SOUND_CC = $(SOUND_H) $(HPSOUND_CC) sound.cc
endif

ifeq ($(ARCH),irix6)
SOUND_CC = $(SOUND_H) $(IRIXSOUND_CC) sound.cc
endif

SOUNDLIST_CC = $(SOUNDLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(SOUND_H) $(PTRLIST_CC) soundlist.cc

SOUNDTCL_CC = $(SOUNDLIST_H) $(SOUND_H) $(LISTPKG_H) $(LISTITEMPKG_H) \
	$(TCLLINK_H) $(TRACE_H) $(DEBUG_H) soundtcl.cc

STRINGIFYCMD_CC = $(STRINGIFYCMD_H) $(IO_H) \
	$(ASCIISTREAMREADER_H) $(ASCIISTREAMWRITER_H) \
	$(TRACE_H) $(DEBUG_H) stringifycmd.cc

SUBJECT_CC = $(SUBJECT_H) $(IOUTILS_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) subject.cc

SUBJECTTCL_CC = $(ERRMSG_H) $(SUBJECT_H) $(TRACE_H) $(DEBUG_H) subjecttcl.cc

SYSTEM_CC = $(SYSTEM_H) $(TRACE_H) $(DEBUG_H) system.cc

TCLCMD_CC = $(TCLCMD_H) $(DEMANGLE_H) $(ERRMSG_H) $(TCLVALUE_H) \
	$(TRACE_H) $(DEBUG_H) tclcmd.cc

TCLDLIST_CC = $(TCLDLIST_H) $(ERRMSG_H) $(TRACE_H) $(DEBUG_H) tcldlist.cc

TCLERROR_CC = $(TCLERROR_H) $(TRACE_H) tclerror.cc

TCLGL_CC = $(TCLPKG_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclgl.cc

TCLITEMPKG_CC = $(TCLITEMPKG_H) $(TCLCMD_H) $(STRINGIFYCMD_H) \
	$(TCLVECCMDS_H) $(TRACE_H) $(DEBUG_H) tclitempkg.cc

TCLPKG_CC = $(TCLPKG_H) $(TCLLINK_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclpkg.cc

TCLVALUE_CC = $(TCLVALUE_H) $(TRACE_H) $(DEBUG_H) tclvalue.cc

TCLVECCMDS_CC = $(TCLVECCMDS_H) $(TCLITEMPKGBASE_H) \
	$(DEBUG_H) $(TRACE_H) tclveccmds.cc

THLIST_CC = $(THLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(TIMINGHDLR_H) $(PTRLIST_CC) thlist.cc

THTCL_CC = $(IOFACTORY_H) $(THLIST_H) $(TCLCMD_H) $(TIMINGHDLR_H) \
	$(TIMINGHANDLER_H) $(TRIALEVENT_H) $(LISTITEMPKG_H) $(LISTPKG_H)\
	 $(TRACE_H) $(DEBUG_H) thtcl.cc

TIMINGHANDLER_CC = $(TIMINGHANDLER_H) $(TRIALEVENT_H) \
	$(TRACE_H) $(DEBUG_H) timinghandler.cc

TIMINGHDLR_CC = $(TIMINGHDLR_H) $(IOMGR_H) $(TRIALEVENT_H) \
	$(READUTILS_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) timinghdlr.cc

TLIST_CC = $(TLIST_H) $(TRIAL_H) $(TRACE_H) $(DEBUG_H) \
	$(PTRLIST_CC) tlist.cc

TLISTTCL_CC = $(APPLICATION_H) $(EXPERIMENT_H) \
	$(TLIST_H) $(TCLCMD_H) $(LISTPKG_H) \
	$(TLISTUTILS_H) $(TRACE_H) $(DEBUG_H) tlisttcl.cc

TLISTUTILS_CC = $(TLISTUTILS_H) $(CANVAS_H) $(ERROR_H) \
	$(GTEXT_H) $(OBJLIST_H) \
	$(POSITION_H) $(POSLIST_H) $(RECT_H) $(TRIAL_H) $(TLIST_H) \
	$(TRACE_H) $(DEBUG_H) tlistutils.cc

TLISTWIDGET_CC = $(TLISTWIDGET_H) $(CANVAS_H) $(TLIST_H) $(TRIAL_H) \
	$(TRACE_H) $(DEBUG_H) tlistwidget.cc

TOGLCONFIG_CC = $(TOGLCONFIG_H) $(ERROR_H) $(GLCANVAS_H) \
	$(TCLEVALCMD_H) $(TRACE_H) $(DEBUG_H) toglconfig.cc

TRACE_CC = $(TRACE_H) util/trace.cc

TRACERTCL_CC = $(TRACERTCL_H) $(TCLCMD_H) $(TCLPKG_H) $(TRACER_H) \
	$(DEBUG_H) tracertcl.cc

TRIAL_CC = $(TRIAL_H) $(CANVAS_H) \
	$(OBJLIST_H) $(POSLIST_H) $(GROBJ_H) $(POSITION_H) \
	$(READER_H) $(READUTILS_H) $(WRITER_H) $(WRITEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) trial.cc

TRIALEVENT_CC = $(TRIALEVENT_H) $(CANVAS_H) $(DEMANGLE_H) $(ERROR_H) \
	$(EXPERIMENT_H) $(READER_H) \
	$(WRITER_H) $(TRACE_H) $(DEBUG_H) trialevent.cc

TRIALTCL_CC = $(IOFACTORY_H) $(TRIAL_H) $(TLIST_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) trialtcl.cc

VALUE_CC = $(VALUE_H) value.cc

VOIDPTRLIST_CC = $(VOIDPTRLIST_H) $(DEMANGLE_H) \
	$(TRACE_H) $(DEBUG_H) voidptrlist.cc

WIDGET_CC = $(WIDGET_H) widget.cc

WRITER_CC = $(WRITER_H) writer.cc

WRITEUTILS_CC = $(WRITEUTILS_H) writeutils.cc

XBITMAP_CC = $(XBITMAP_H) $(XBMAPRENDERER_H) \
	$(TRACE_H) $(DEBUG_H) xbitmap.cc

XBMAPRENDERER_CC = $(XBMAPRENDERER_H) $(CANVAS_H) $(ERROR_H) $(POINT_H) \
	$(TRACE_H) $(DEBUG_H) xbmaprenderer.cc

#-------------------------------------------------------------------------
#
# Dependency lists for object files
#
#-------------------------------------------------------------------------

$(GRSH)/application.*[ol]:       $(APPLICATION_CC)
$(GRSH)/asciistreamreader.*[ol]: $(ASCIISTREAMREADER_CC)
$(GRSH)/asciistreamwriter.*[ol]: $(ASCIISTREAMWRITER_CC)
$(GRSH)/bitmap.*[ol]:            $(BITMAP_CC)
$(GRSH)/bitmaprep.*[ol]:         $(BITMAPREP_CC)
$(GRSH)/bitmaptcl.*[ol]:         $(BITMAPTCL_CC)
$(GRSH)/block.*[ol]:             $(BLOCK_CC)
$(GRSH)/blocklist.*[ol]:         $(BLOCKLIST_CC)
$(GRSH)/blocktcl.*[ol]:          $(BLOCKTCL_CC)
$(GRSH)/bmapdata.*[ol]:          $(BMAPDATA_CC)
$(GRSH)/bmaprenderer.*[ol]:      $(BMAPRENDERER_CC)
$(GRSH)/canvas.*[ol]:            $(CANVAS_CC)
$(GRSH)/cloneface.*[ol]:         $(CLONEFACE_CC)
$(GRSH)/demangle.*[ol]:          $(DEMANGLE_CC)
$(UTIL)/error.*[ol]:             $(ERROR_CC)
$(GRSH)/eventresponsehdlr.*[ol]: $(EVENTRESPONSEHDLR_CC)
$(GRSH)/experiment.*[ol]:        $(EXPERIMENT_CC)
$(GRSH)/exptdriver.*[ol]:        $(EXPTDRIVER_CC)
$(GRSH)/expttcl.*[ol]:           $(EXPTTCL_CC)
$(GRSH)/expttesttcl.*[ol]:       $(EXPTTESTTCL_CC)
$(GRSH)/face.*[ol]:              $(FACE_CC)
$(GRSH)/facetcl.*[ol]:           $(FACETCL_CC)
$(GRSH)/factory.*[ol]:           $(FACTORY_CC)
$(GRSH)/fish.*[ol]:              $(FISH_CC)
$(GRSH)/fishtcl.*[ol]:           $(FISHTCL_CC)
$(GRSH)/fixpt.*[ol]:             $(FIXPT_CC)
$(GRSH)/fixpttcl.*[ol]:          $(FIXPTTCL_CC)
$(GRSH)/gabor.*[ol]:             $(GABOR_CC)
$(GRSH)/gabortcl.*[ol]:          $(GABORTCL_CC)
$(GRSH)/glbitmap.*[ol]:          $(GLBITMAP_CC)
$(GRSH)/glbmaprenderer.*[ol]:    $(GLBMAPRENDERER_CC)
$(GRSH)/glcanvas.*[ol]:          $(GLCANVAS_CC)
$(GRSH)/grobj.*[ol]:             $(GROBJ_CC)
$(GRSH)/grobjtcl.*[ol]:          $(GROBJTCL_CC)
$(GRSH)/grshapp.*[ol]:           $(GRSHAPP_CC)
$(GRSH)/grshAppInit.*[ol]:       $(GRSHAPPINIT_CC)
$(GRSH)/gtext.*[ol]:             $(GTEXT_CC)
$(GRSH)/gtexttcl.*[ol]:          $(GTEXTTCL_CC)
$(GRSH)/house.*[ol]:             $(HOUSE_CC)
$(GRSH)/housetcl.*[ol]:          $(HOUSETCL_CC)
$(GRSH)/io.*[ol]:                $(IO_CC)
$(GRSH)/iofactory.*[ol]:         $(IOFACTORY_CC)
$(GRSH)/iomgr.*[ol]:             $(IOMGR_CC)
$(GRSH)/ioptrlist.*[ol]:         $(IOPTRLIST_CC)
$(GRSH)/iostl.*[ol]:             $(IOSTL_CC)
$(GRSH)/ioutils.*[ol]:           $(IOUTILS_CC)
$(GRSH)/jitter.*[ol]:            $(JITTER_CC)
$(GRSH)/jittertcl.*[ol]:         $(JITTERTCL_CC)
$(GRSH)/kbdresponsehdlr.*[ol]:   $(KBDRESPONSEHDLR_CC)
$(GRSH)/listpkg.*[ol]:           $(LISTPKG_CC)
$(GRSH)/maskhatch.*[ol]:         $(MASKHATCH_CC)
$(GRSH)/masktcl.*[ol]:           $(MASKTCL_CC)
$(GRSH)/misctcl.*[ol]:           $(MISCTCL_CC)
$(GRSH)/morphyface.*[ol]:        $(MORPHYFACE_CC)
$(GRSH)/morphyfacetcl.*[ol]:     $(MORPHYFACETCL_CC)
$(GRSH)/nullresponsehdlr.*[ol]:  $(NULLRESPONSEHDLR_CC)
$(GRSH)/objlist.*[ol]:           $(OBJLIST_CC)
$(GRSH)/objlisttcl.*[ol]:        $(OBJLISTTCL_CC)
$(GRSH)/objtogl.*[ol]:           $(OBJTOGL_CC)
$(GRSH)/observable.*[ol]:        $(OBSERVABLE_CC)
$(GRSH)/observer.*[ol]:          $(OBSERVER_CC)
$(GRSH)/pbm.*[ol]:               $(PBM_CC)
$(GRSH)/position.*[ol]:          $(POSITION_CC)
$(GRSH)/positiontcl.*[ol]:       $(POSITIONTCL_CC)
$(GRSH)/poslist.*[ol]:           $(POSLIST_CC)
$(GRSH)/poslisttcl.*[ol]:        $(POSLISTTCL_CC)
$(GRSH)/property.*[ol]:          $(PROPERTY_CC)
$(GRSH)/reader.*[ol]:            $(READER_CC)
$(GRSH)/readutils.*[ol]:         $(READUTILS_CC)
$(GRSH)/responsehandler.*[ol]:   $(RESPONSEHANDLER_CC)
$(GRSH)/rhlist.*[ol]:            $(RHLIST_CC)
$(GRSH)/rhtcl.*[ol]:             $(RHTCL_CC)
$(GRSH)/sound.*[ol]:             $(SOUND_CC)
$(GRSH)/soundlist.*[ol]:         $(SOUNDLIST_CC)
$(GRSH)/soundtcl.*[ol]:          $(SOUNDTCL_CC)
$(GRSH)/stringifycmd.*[ol]:      $(STRINGIFYCMD_CC)
$(GRSH)/subject.*[ol]:           $(SUBJECT_CC)
$(GRSH)/subjecttcl.*[ol]:        $(SUBJECTTCL_CC)
$(GRSH)/system.*[ol]:            $(SYSTEM_CC)
$(GRSH)/tcldlist.*[ol]:          $(TCLDLIST_CC)
$(GRSH)/tclcmd.*[ol]:            $(TCLCMD_CC)
$(GRSH)/tclerror.*[ol]:          $(TCLERROR_CC)
$(GRSH)/tclgl.*[ol]:             $(TCLGL_CC)
$(GRSH)/tclitempkg.*[ol]:        $(TCLITEMPKG_CC)
$(GRSH)/tclpkg.*[ol]:            $(TCLPKG_CC)
$(GRSH)/tclvalue.*[ol]:          $(TCLVALUE_CC)
$(GRSH)/tclveccmds.*[ol]:        $(TCLVECCMDS_CC)
$(GRSH)/thlist.*[ol]:            $(THLIST_CC)
$(GRSH)/thtcl.*[ol]:             $(THTCL_CC)
$(GRSH)/timinghdlr.*[ol]:        $(TIMINGHDLR_CC)
$(GRSH)/timinghandler.*[ol]:     $(TIMINGHANDLER_CC)
$(GRSH)/tlist.*[ol]:             $(TLIST_CC)
$(GRSH)/tlisttcl.*[ol]:          $(TLISTTCL_CC)
$(GRSH)/tlistutils.*[ol]:        $(TLISTUTILS_CC)
$(GRSH)/tlistwidget.*[ol]:       $(TLISTWIDGET_CC)
$(GRSH)/toglconfig.*[ol]:        $(TOGLCONFIG_CC)
$(UTIL)/trace.*[ol]:             $(TRACE_CC)
$(GRSH)/tracertcl.*[ol]:         $(TRACERTCL_CC)
$(GRSH)/trial.*[ol]:             $(TRIAL_CC)
$(GRSH)/trialevent.*[ol]:        $(TRIALEVENT_CC)
$(GRSH)/trialtcl.*[ol]:          $(TRIALTCL_CC)
$(GRSH)/value.*[ol]:             $(VALUE_CC)
$(GRSH)/voidptrlist.*[ol]:       $(VOIDPTRLIST_CC)
$(GRSH)/widget.*[ol]:            $(WIDGET_CC)
$(GRSH)/writer.*[ol]:            $(WRITER_CC)
$(GRSH)/writeutils.*[ol]:        $(WRITEUTILS_CC)
$(GRSH)/xbitmap.*[ol]:           $(XBITMAP_CC)
$(GRSH)/xbmaprenderer.*[ol]:     $(XBMAPRENDERER_CC)

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Remove all object files and build a new production executable from scratch
new: cleaner $(PROD_TARGET)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f *~ \#* core

# Make clean, and also remove all object files
cleaner: clean
	rm -f *.o

# Generate TAGS file based on all source files
ALL_SOURCES = *\.[ch]* util/*\.[ch]*
TAGS: $(ALL_SOURCES)
ifeq ($(ARCH),hp9000s700)
	echo 'etags $(ALL_SOURCES)'
else
	etags $(ALL_SOURCES)
endif

# Count the lines in all source files
count:
	wc -l $(ALL_SOURCES)

# Count the number of non-commented source lines
ncsl:
	NCSL $(ALL_SOURCES)

# Start emacs and load all source files and Makefile
edit: clean
	emacs $(ALL_SOURCES) Makefile testing/grshtest.tcl Log.txt

docs: DoxygenConfig *.h
	(doxygen DoxygenConfig > DocLog) >& DocErrors

AdepAliases: FileList *.h *.cc
	adep -s -fFileList > AdepAliases

cdeps: $(ALL_SOURCES)
	cdep -iCdepSearchpath *.h *.cc > CdepDeps

ldeps: cdeps
	ldep -dCdepDeps -L > Ldeps

backup:
	tclsh Backup.tcl

