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

GRSH_STATIC_OBJS = \
	$(ARCH)/bitmap.do \
	$(ARCH)/bitmaprep.do \
	$(ARCH)/face.do \
	$(ARCH)/fish.do \
	$(ARCH)/fixpt.do \
	$(ARCH)/gabor.do \
	$(ARCH)/glbitmap.do \
	$(ARCH)/glcanvas.do \
	$(ARCH)/glbmaprenderer.do \
	$(ARCH)/grobj.do \
	$(ARCH)/grshAppInit.do \
	$(ARCH)/gtext.do \
	$(ARCH)/house.do \
	$(ARCH)/jitter.do \
	$(ARCH)/maskhatch.do \
	$(ARCH)/morphyface.do \
	$(ARCH)/objtogl.do \
	$(ARCH)/position.do \
	$(ARCH)/tclgl.do \
	$(ARCH)/tlistwidget.do \
	$(ARCH)/toglconfig.do \
	$(ARCH)/xbitmap.do \
	$(ARCH)/xbmaprenderer.do \

UTIL = util/$(ARCH)

GRSH_DYNAMIC_OBJS = \
	$(ARCH)/application.do \
	$(ARCH)/asciistreamreader.do \
	$(ARCH)/asciistreamwriter.do \
	$(ARCH)/bitmaptcl.do \
	$(ARCH)/block.do \
	$(ARCH)/blocklist.do \
	$(ARCH)/blocktcl.do \
	$(ARCH)/bmaprenderer.do \
	$(ARCH)/canvas.do \
	$(ARCH)/cloneface.do \
	$(ARCH)/demangle.do \
	$(UTIL)/error.do \
	$(ARCH)/eventresponsehdlr.do \
	$(ARCH)/experiment.do \
	$(ARCH)/exptdriver.do \
	$(ARCH)/expttcl.do \
	$(ARCH)/expttesttcl.do \
	$(ARCH)/facetcl.do \
	$(ARCH)/factory.do \
	$(ARCH)/fishtcl.do \
	$(ARCH)/fixpttcl.do \
	$(ARCH)/gabortcl.do \
	$(ARCH)/grobjtcl.do \
	$(ARCH)/grshapp.do \
	$(ARCH)/gtexttcl.do \
	$(ARCH)/housetcl.do \
	$(ARCH)/io.do \
	$(ARCH)/iofactory.do \
	$(ARCH)/iomgr.do \
	$(ARCH)/ioptrlist.do \
	$(ARCH)/iostl.do \
	$(ARCH)/ioutils.do \
	$(ARCH)/jittertcl.do \
	$(ARCH)/kbdresponsehdlr.do \
	$(ARCH)/masktcl.do \
	$(ARCH)/morphyfacetcl.do \
	$(ARCH)/nullresponsehdlr.do \
	$(ARCH)/objlist.do \
	$(ARCH)/objlisttcl.do \
	$(ARCH)/observable.do \
	$(ARCH)/observer.do \
	$(ARCH)/pbm.do \
	$(ARCH)/positiontcl.do \
	$(ARCH)/poslist.do \
	$(ARCH)/poslisttcl.do \
	$(ARCH)/property.do \
	$(ARCH)/reader.do \
	$(ARCH)/readutils.do \
	$(ARCH)/responsehandler.do \
	$(ARCH)/rhlist.do \
	$(ARCH)/rhtcl.do \
	$(ARCH)/sound.do \
	$(ARCH)/soundlist.do \
	$(ARCH)/soundtcl.do \
	$(ARCH)/subject.do \
	$(ARCH)/subjecttcl.do \
	$(ARCH)/system.do \
	$(ARCH)/thlist.do \
	$(ARCH)/thtcl.do \
	$(ARCH)/timinghandler.do \
	$(ARCH)/timinghdlr.do \
	$(ARCH)/tlist.do \
	$(ARCH)/tlisttcl.do \
	$(ARCH)/tlistutils.do \
	$(UTIL)/trace.do \
	$(ARCH)/trial.do \
	$(ARCH)/trialevent.do \
	$(ARCH)/trialtcl.do \
	$(ARCH)/value.do \
	$(ARCH)/voidptrlist.do \
	$(ARCH)/widget.do \
	$(ARCH)/writer.do \
	$(ARCH)/writeutils.do \

TCLWORKS_OBJS = \
	$(ARCH)/misctcl.do \
	$(ARCH)/stringifycmd.do \
	$(ARCH)/tclcmd.do \
	$(ARCH)/tcldlist.do \
	$(ARCH)/tclerror.do \
	$(ARCH)/tclitempkg.do \
	$(ARCH)/tclpkg.do \
	$(ARCH)/tclvalue.do \
	$(ARCH)/tclveccmds.do \


#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS = $(ARCH_FLAGS) $(INCLUDE_DIRS)

ALL_PROD_OPTIONS = $(COMMON_OPTIONS) $(PROD_OPTIM) $(PROD_OPTIONS) 
ALL_DEBUG_OPTIONS = $(COMMON_OPTIONS) $(DEBUG_OPTIM) $(DEBUG_OPTIONS) $(DEBUG_FLAGS)

$(ARCH)/%.o : %.cc
	$(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)

$(ARCH)/%.do : %.cc;
	$(CC) -c $< -o $@ $(ALL_DEBUG_OPTIONS)

#-------------------------------------------------------------------------
#
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

DEBUG_TARGET = testsh

DEBUG_LIBVISX = $(ARCH)/libvisx.d.$(SHLIB_EXT)
DEBUG_LIBTCLWORKS = $(ARCH)/libtclworks.d.$(SHLIB_EXT)

DEBUG_FLAGS = -DPROF -DASSERT -DINVARIANT -DTEST

DEBUG_GRSH_STATIC_OBJS = $(GRSH_STATIC_OBJS)
DEBUG_GRSH_DYNAMIC_OBJS = $(GRSH_DYNAMIC_OBJS)
DEBUG_TCLWORKS_OBJS = $(TCLWORKS_OBJS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET = grsh0.7a2

PROD_LIBVISX = $(ARCH)/libvisx.$(SHLIB_EXT)
PROD_LIBTCLWORKS = $(ARCH)/libtclworks.$(SHLIB_EXT)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.
ifeq ($(ARCH),irix6)
	PROD_LIBVISX = $(ARCH)/libvisx.$(STATLIB_EXT)
	PROD_LIBTCLWORKS = $(ARCH)/libtclworks.$(STATLIB_EXT)
endif

PROD_GRSH_STATIC_OBJS = $(DEBUG_GRSH_STATIC_OBJS:.do=.o)
PROD_GRSH_DYNAMIC_OBJS = $(DEBUG_GRSH_DYNAMIC_OBJS:.do=.o)
PROD_TCLWORKS_OBJS = $(DEBUG_TCLWORKS_OBJS:.do=.o)

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

testsh: TAGS $(HOME)/bin/$(ARCH)/$(DEBUG_TARGET)
	$(DEBUG_TARGET) ./testing/grshtest.tcl

ALL_DEBUG_DEPENDS = \
	$(DEBUG_GRSH_STATIC_OBJS) \
	$(DEBUG_LIBVISX) \
	$(DEBUG_LIBTCLWORKS)

$(HOME)/bin/$(ARCH)/$(DEBUG_TARGET): $(ALL_DEBUG_DEPENDS)
	$(CC) $(DEBUG_LINK_OPTIONS) -o $@ $(DEBUG_GRSH_STATIC_OBJS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) -lvisx.d -ltclworks.d $(LIBRARIES) 

grsh: TAGS $(HOME)/bin/$(ARCH)/$(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

ALL_PROD_DEPENDS = \
	$(PROD_GRSH_STATIC_OBJS) \
	$(PROD_LIBVISX) \
	$(PROD_LIBTCLWORKS)

$(HOME)/bin/$(ARCH)/$(PROD_TARGET): $(ALL_PROD_DEPENDS)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_GRSH_STATIC_OBJS) \
	$(LIB_DIRS) -lvisx -ltclworks $(LIBRARIES)

#-------------------------------------------------------------------------
#
# Build rules for production and debug shared libraries
#
#-------------------------------------------------------------------------

%.$(SHLIB_EXT):
	$(SHLIB_CMD) $(HOME)/ftp/$@ $^
	mv $(HOME)/ftp/$@ ./$@

%.$(STATLIB_EXT):
	$(STATLIB_CMD) $(HOME)/ftp/$@ $^
	mv $(HOME)/ftp/$@ ./$@

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
BITMAPREP_H = bitmaprep.h
BMAPRENDERER_H = bmaprenderer.h
CANVAS_H = canvas.h
DEBUG_H = util/debug.h
DEMANGLE_H = demangle.h
ERRMSG_H = errmsg.h
ERROR_H = util/error.h
EXPERIMENT_H = experiment.h
IOUTILS_H = ioutils.h
OBJLISTTCL_H = objlisttcl.h
OBJTOGL_H = objtogl.h
OBSERVABLE_H = observable.h
OBSERVER_H = observer.h
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
WIDGET_H = widget.h

#
# level 1 headers
#
APPLICATION_H = $(ERROR_H) application.h
FACTORY_H = $(ERROR_H) $(DEMANGLE_H) factory.h
GLBMAPRENDERER_H = $(BMAPRENDERER_H) glbmaprenderer.h
GLCANVAS_H = $(CANVAS_H) glcanvas.h
IO_H = $(ERROR_H) io.h
PBM_H = $(ERROR_H) pbm.h
READER_H = $(ERROR_H) reader.h
RECT_H = $(POINT_H) rect.h
TCLERROR_H = $(ERROR_H) tclerror.h
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
GROBJ_H = $(IO_H) $(OBSERVABLE_H) $(OBSERVER_H) grobj.h
GRSHAPP_H = $(APPLICATION_H) $(ERROR_H) grshapp.h
IOFACTORY_H = $(FACTORY_H) $(IO_H) iofactory.h
IOPTRLIST_H = $(VOIDPTRLIST_H) $(IO_H) ioptrlist.h
IOSTL_H = $(IO_H) iostl.h
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
FISH_H = $(GROBJ_H) $(PROPERTY_H) fish.h
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
PTRLIST_H = $(IOPTRLIST_H) ptrlist.h
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
PTRLIST_CC = $(PTRLIST_H) ptrlist.cc
RHLIST_H = $(PTRLIST_H) rhlist.h
SOUNDLIST_H = $(PTRLIST_H) soundlist.h
STRINGIFYCMD_H = $(TCLCMD_H) stringifycmd.h
THLIST_H = $(PTRLIST_H) thlist.h
TLIST_H = $(PTRLIST_H) $(IO_H) tlist.h
TCLITEMPKG_H = $(TCLPKG_H) $(TCLCMD_H) $(PROPERTY_H) tclitempkg.h
TCLVECCMDS_H = $(TCLCMD_H) tclveccmds.h
XBITMAP_H = $(BITMAP_H) xbitmap.h

#
# level 5 headers
#
LISTITEMPKG_H = $(TCLITEMPKG_H) $(DEMANGLE_H) listitempkg.h
LISTPKG_H = $(TCLITEMPKG_H) listpkg.h

#
# level 6 headers
#
PROPITEMPKG_H = $(LISTITEMPKG_H) $(IOMGR_H) propitempkg.h

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

BITMAP_CC = $(BITMAP_H) $(BITMAPREP_H) $(TRACE_H) $(DEBUG_H) bitmap.cc

BITMAPREP_CC = $(BITMAPREP_H) $(BMAPRENDERER_H) $(GLCANVAS_H) \
	$(ERROR_H) $(IO_H) $(PBM_H) \
	$(READER_H) $(RECT_H) $(WRITER_H) $(TRACE_H) $(DEBUG_H) bitmaprep.cc

BITMAPTCL_CC = $(BITMAP_H) $(GLBITMAP_H) $(XBITMAP_H) $(IOMGR_H) $(OBJLIST_H) \
	$(OBJTOGL_H) $(LISTITEMPKG_H) $(SYSTEM_H) $(TCLCMD_H) \
	$(TCLOBJLOCK_H) $(TRACE_H) $(DEBUG_H) bitmaptcl.cc

BLOCK_CC = $(BLOCK_H) $(RAND_H) $(IOSTL_H) $(READER_H) $(READUTILS_H) \
	$(TLIST_H) $(TRIAL_H) \
	$(WRITER_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) block.cc

BLOCKLIST_CC = $(BLOCKLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(BLOCK_H) $(PTRLIST_CC) blocklist.cc

BLOCKTCL_CC = $(IOMGR_H) $(BLOCKLIST_H) $(BLOCK_H) $(TCLCMD_H) \
	$(LISTITEMPKG_H) $(LISTPKG_H) $(TRACE_H) $(DEBUG_H) blocktcl.cc

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

FISHTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(LISTITEMPKG_H) $(FISH_H) fishtcl.cc

FIXPT_CC = $(FIXPT_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) fixpt.cc

FIXPTTCL_CC = $(OBJLIST_H) $(FIXPT_H) $(PROPITEMPKG_H) \
	$(IOMGR_H) $(TRACE_H) fixpttcl.cc

GABOR_CC = $(GABOR_H) $(RANDUTILS_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) gabor.cc

GABORTCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(GABOR_H) $(TRACE_H) gabortcl.cc

GLBITMAP_CC = $(GLBITMAP_H) $(GLBMAPRENDERER_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) glbitmap.cc

GLBMAPRENDERER_CC = $(GLBMAPRENDERER_H) $(TRACE_H) glbmaprenderer.cc

GLCANVAS_CC = $(GLCANVAS_H) $(ERROR_H) $(POINT_H) $(RECT_H) \
	$(DEBUG_H) $(TRACE_H) glcanvas.cc

GROBJ_CC = $(GROBJ_H) $(BITMAPREP_H) $(CANVAS_H) $(GLCANVAS_H) \
	$(GLBMAPRENDERER_H) $(ERROR_H) $(RECT_H) $(READER_H) \
	$(WRITER_H) $(XBMAPRENDERER_H) $(TRACE_H) $(DEBUG_H) grobj.cc

GROBJTCL_CC = $(APPLICATION_H) $(EXPERIMENT_H) $(GROBJ_H) \
	$(OBJLIST_H) $(LISTITEMPKG_H) $(RECT_H) $(TCLVECCMDS_H) grobjtcl.cc

GRSHAPP_CC = $(GRSHAPP_H) $(TRACE_H) grshapp.cc

GRSHAPPINIT_CC = $(TRACE_H) $(GRSHAPP_H) grshAppInit.cc

GTEXT_CC = $(GTEXT_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) gtext.cc

GTEXTTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(GTEXT_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) gtexttcl.cc

HOUSE_CC = $(HOUSE_H) $(READER_H) $(RECT_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) house.cc

HOUSETCL_CC = $(HOUSE_H) $(OBJLIST_H) $(PROPITEMPKG_H) $(TRACE_H) housetcl.cc

IO_CC = $(IO_H) $(DEMANGLE_H) $(TRACE_H) $(DEBUG_H) io.cc

IOFACTORY_CC = $(IOFACTORY_H) iofactory.cc

IOMGR_CC = $(IOMGR_H) $(TRACE_H) $(DEBUG_H) iomgr.cc

IOPTRLIST_CC = $(IOPTRLIST_H) $(DEMANGLE_H) $(IOMGR_H) \
	$(READUTILS_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) ioptrlist.cc

IOSTL_CC = $(IOSTL_H) iostl.cc

IOUTILS_CC = $(IOUTILS_H) $(IO_H) ioutils.cc

JITTER_CC = $(JITTER_H) $(RANDUTILS_H) $(READER_H) $(WRITER_H) \
	$(TRACE_H) $(DEBUG_H) jitter.cc

JITTERTCL_CC = $(IOMGR_H) $(JITTER_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TRACE_H) $(DEBUG_H) jittertcl.cc

KBDRESPONSEHDLR_CC = $(KBDRESPONSEHDLR_H) \
	$(TRACE_H) $(DEBUG_H) kbdresponsehdlr.cc

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

POSITIONTCL_CC = $(DEMANGLE_H) $(IOMGR_H) $(POSITION_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) positiontcl.cc

POSLIST_CC = $(POSLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(POSITION_H) $(PTRLIST_CC) poslist.cc

POSLISTTCL_CC = $(POSLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) poslisttcl.cc

PROPERTY_CC = $(PROPERTY_H) property.cc

READER_CC = $(READER_H) reader.cc

READUTILS_CC = $(READUTILS_H) readutils.cc

RESPONSEHANDLER_CC = $(RESPONSEHANDLER_H) $(TRACE_H) responsehandler.cc

RHLIST_CC = $(RHLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(RESPONSEHANDLER_H) $(PTRLIST_CC) rhlist.cc

RHTCL_CC = $(EVENTRESPONSEHDLR_H) $(IOMGR_H) $(RHLIST_H) \
	$(RESPONSEHANDLER_H) $(TCLCMD_H) $(KBDRESPONSEHDLR_H) \
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

TCLVECCMDS_CC = $(TCLVECCMDS_H) $(TCLITEMPKG_H) $(DEBUG_H) $(TRACE_H) \
	tclveccmds.cc

THLIST_CC = $(THLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(TIMINGHDLR_H) $(PTRLIST_CC) thlist.cc

THTCL_CC = $(IOMGR_H) $(THLIST_H) $(TCLCMD_H) $(TIMINGHDLR_H) \
	$(TIMINGHANDLER_H) $(TRIALEVENT_H) $(LISTITEMPKG_H) $(LISTPKG_H)\
	 $(TRACE_H) $(DEBUG_H) thtcl.cc

TIMINGHANDLER_CC = $(TIMINGHANDLER_H) $(TRIALEVENT_H) \
	$(TRACE_H) $(DEBUG_H) timinghandler.cc

TIMINGHDLR_CC = $(TIMINGHDLR_H) $(IOMGR_H) $(TRIALEVENT_H) \
	$(READUTILS_H) $(WRITEUTILS_H) $(TRACE_H) $(DEBUG_H) timinghdlr.cc

TLIST_CC = $(TLIST_H) $(TRIAL_H) $(TRACE_H) $(DEBUG_H) \
	$(PTRLIST_CC) tlist.cc

TLISTTCL_CC = $(TLIST_H) $(TCLCMD_H) $(LISTPKG_H) \
	$(TLISTUTILS_H) $(TRACE_H) $(DEBUG_H) tlisttcl.cc

TLISTUTILS_CC = $(TLISTUTILS_H) $(ERROR_H) $(GLCANVAS_H) \
	$(GTEXT_H) $(OBJLIST_H) \
	$(POSITION_H) $(POSLIST_H) $(RECT_H) $(TRIAL_H) $(TLIST_H) \
	$(TRACE_H) $(DEBUG_H) tlistutils.cc

TLISTWIDGET_CC = $(TLISTWIDGET_H) $(CANVAS_H) $(TLIST_H) $(TRIAL_H) \
	$(TRACE_H) $(DEBUG_H) tlistwidget.cc

TOGLCONFIG_CC = $(TOGLCONFIG_H) $(ERROR_H) $(GLCANVAS_H) \
	$(TCLEVALCMD_H) $(TRACE_H) $(DEBUG_H) toglconfig.cc

TRACE_CC = $(TRACE_H) util/trace.cc

TRIAL_CC = $(TRIAL_H) $(CANVAS_H) \
	$(OBJLIST_H) $(POSLIST_H) $(GROBJ_H) $(POSITION_H) \
	$(READER_H) $(READUTILS_H) $(WRITER_H) $(WRITEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) trial.cc

TRIALEVENT_CC = $(TRIALEVENT_H) $(CANVAS_H) $(DEMANGLE_H) $(ERROR_H) \
	$(EXPERIMENT_H) $(READER_H) \
	$(WRITER_H) $(TRACE_H) $(DEBUG_H) trialevent.cc

TRIALTCL_CC = $(IOMGR_H) $(TRIAL_H) $(TLIST_H) $(LISTITEMPKG_H) \
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

$(ARCH)/application.*[ol]:       $(APPLICATION_CC)
$(ARCH)/asciistreamreader.*[ol]: $(ASCIISTREAMREADER_CC)
$(ARCH)/asciistreamwriter.*[ol]: $(ASCIISTREAMWRITER_CC)
$(ARCH)/bitmap.*[ol]:            $(BITMAP_CC)
$(ARCH)/bitmaprep.*[ol]:         $(BITMAPREP_CC)
$(ARCH)/bitmaptcl.*[ol]:         $(BITMAPTCL_CC)
$(ARCH)/block.*[ol]:             $(BLOCK_CC)
$(ARCH)/blocklist.*[ol]:         $(BLOCKLIST_CC)
$(ARCH)/blocktcl.*[ol]:          $(BLOCKTCL_CC)
$(ARCH)/bmaprenderer.*[ol]:      $(BMAPRENDERER_CC)
$(ARCH)/canvas.*[ol]:            $(CANVAS_CC)
$(ARCH)/cloneface.*[ol]:         $(CLONEFACE_CC)
$(ARCH)/demangle.*[ol]:          $(DEMANGLE_CC)
$(UTIL)/error.*[ol]:             $(ERROR_CC)
$(ARCH)/eventresponsehdlr.*[ol]: $(EVENTRESPONSEHDLR_CC)
$(ARCH)/experiment.*[ol]:        $(EXPERIMENT_CC)
$(ARCH)/exptdriver.*[ol]:        $(EXPTDRIVER_CC)
$(ARCH)/expttcl.*[ol]:           $(EXPTTCL_CC)
$(ARCH)/expttesttcl.*[ol]:       $(EXPTTESTTCL_CC)
$(ARCH)/face.*[ol]:              $(FACE_CC)
$(ARCH)/facetcl.*[ol]:           $(FACETCL_CC)
$(ARCH)/factory.*[ol]:           $(FACTORY_CC)
$(ARCH)/fish.*[ol]:              $(FISH_CC)
$(ARCH)/fishtcl.*[ol]:           $(FISHTCL_CC)
$(ARCH)/fixpt.*[ol]:             $(FIXPT_CC)
$(ARCH)/fixpttcl.*[ol]:          $(FIXPTTCL_CC)
$(ARCH)/gabor.*[ol]:             $(GABOR_CC)
$(ARCH)/gabortcl.*[ol]:          $(GABORTCL_CC)
$(ARCH)/glbitmap.*[ol]:          $(GLBITMAP_CC)
$(ARCH)/glbmaprenderer.*[ol]:    $(GLBMAPRENDERER_CC)
$(ARCH)/glcanvas.*[ol]:          $(GLCANVAS_CC)
$(ARCH)/grobj.*[ol]:             $(GROBJ_CC)
$(ARCH)/grobjtcl.*[ol]:          $(GROBJTCL_CC)
$(ARCH)/grshapp.*[ol]:           $(GRSHAPP_CC)
$(ARCH)/grshAppInit.*[ol]:       $(GRSHAPPINIT_CC)
$(ARCH)/gtext.*[ol]:             $(GTEXT_CC)
$(ARCH)/gtexttcl.*[ol]:          $(GTEXTTCL_CC)
$(ARCH)/house.*[ol]:             $(HOUSE_CC)
$(ARCH)/housetcl.*[ol]:          $(HOUSETCL_CC)
$(ARCH)/io.*[ol]:                $(IO_CC)
$(ARCH)/iofactory.*[ol]:         $(IOFACTORY_CC)
$(ARCH)/iomgr.*[ol]:             $(IOMGR_CC)
$(ARCH)/ioptrlist.*[ol]:         $(IOPTRLIST_CC)
$(ARCH)/iostl.*[ol]:             $(IOSTL_CC)
$(ARCH)/ioutils.*[ol]:           $(IOUTILS_CC)
$(ARCH)/jitter.*[ol]:            $(JITTER_CC)
$(ARCH)/jittertcl.*[ol]:         $(JITTERTCL_CC)
$(ARCH)/kbdresponsehdlr.*[ol]:   $(KBDRESPONSEHDLR_CC)
$(ARCH)/maskhatch.*[ol]:         $(MASKHATCH_CC)
$(ARCH)/masktcl.*[ol]:           $(MASKTCL_CC)
$(ARCH)/misctcl.*[ol]:           $(MISCTCL_CC)
$(ARCH)/morphyface.*[ol]:        $(MORPHYFACE_CC)
$(ARCH)/morphyfacetcl.*[ol]:     $(MORPHYFACETCL_CC)
$(ARCH)/nullresponsehdlr.*[ol]:  $(NULLRESPONSEHDLR_CC)
$(ARCH)/objlist.*[ol]:           $(OBJLIST_CC)
$(ARCH)/objlisttcl.*[ol]:        $(OBJLISTTCL_CC)
$(ARCH)/objtogl.*[ol]:           $(OBJTOGL_CC)
$(ARCH)/observable.*[ol]:        $(OBSERVABLE_CC)
$(ARCH)/observer.*[ol]:          $(OBSERVER_CC)
$(ARCH)/pbm.*[ol]:               $(PBM_CC)
$(ARCH)/position.*[ol]:          $(POSITION_CC)
$(ARCH)/positiontcl.*[ol]:       $(POSITIONTCL_CC)
$(ARCH)/poslist.*[ol]:           $(POSLIST_CC)
$(ARCH)/poslisttcl.*[ol]:        $(POSLISTTCL_CC)
$(ARCH)/property.*[ol]:          $(PROPERTY_CC)
$(ARCH)/reader.*[ol]:            $(READER_CC)
$(ARCH)/readutils.*[ol]:         $(READUTILS_CC)
$(ARCH)/responsehandler.*[ol]:   $(RESPONSEHANDLER_CC)
$(ARCH)/rhlist.*[ol]:            $(RHLIST_CC)
$(ARCH)/rhtcl.*[ol]:             $(RHTCL_CC)
$(ARCH)/sound.*[ol]:             $(SOUND_CC)
$(ARCH)/soundlist.*[ol]:         $(SOUNDLIST_CC)
$(ARCH)/soundtcl.*[ol]:          $(SOUNDTCL_CC)
$(ARCH)/stringifycmd.*[ol]:      $(STRINGIFYCMD_CC)
$(ARCH)/subject.*[ol]:           $(SUBJECT_CC)
$(ARCH)/subjecttcl.*[ol]:        $(SUBJECTTCL_CC)
$(ARCH)/system.*[ol]:            $(SYSTEM_CC)
$(ARCH)/tcldlist.*[ol]:          $(TCLDLIST_CC)
$(ARCH)/tclcmd.*[ol]:            $(TCLCMD_CC)
$(ARCH)/tclerror.*[ol]:          $(TCLERROR_CC)
$(ARCH)/tclgl.*[ol]:             $(TCLGL_CC)
$(ARCH)/tclitempkg.*[ol]:        $(TCLITEMPKG_CC)
$(ARCH)/tclpkg.*[ol]:            $(TCLPKG_CC)
$(ARCH)/tclvalue.*[ol]:          $(TCLVALUE_CC)
$(ARCH)/tclveccmds.*[ol]:        $(TCLVECCMDS_CC)
$(ARCH)/thlist.*[ol]:            $(THLIST_CC)
$(ARCH)/thtcl.*[ol]:             $(THTCL_CC)
$(ARCH)/timinghdlr.*[ol]:        $(TIMINGHDLR_CC)
$(ARCH)/timinghandler.*[ol]:     $(TIMINGHANDLER_CC)
$(ARCH)/tlist.*[ol]:             $(TLIST_CC)
$(ARCH)/tlisttcl.*[ol]:          $(TLISTTCL_CC)
$(ARCH)/tlistutils.*[ol]:        $(TLISTUTILS_CC)
$(ARCH)/tlistwidget.*[ol]:       $(TLISTWIDGET_CC)
$(ARCH)/toglconfig.*[ol]:        $(TOGLCONFIG_CC)
$(UTIL)/trace.*[ol]:             $(TRACE_CC)
$(ARCH)/trial.*[ol]:             $(TRIAL_CC)
$(ARCH)/trialevent.*[ol]:        $(TRIALEVENT_CC)
$(ARCH)/trialtcl.*[ol]:          $(TRIALTCL_CC)
$(ARCH)/value.*[ol]:             $(VALUE_CC)
$(ARCH)/voidptrlist.*[ol]:       $(VOIDPTRLIST_CC)
$(ARCH)/widget.*[ol]:            $(WIDGET_CC)
$(ARCH)/writer.*[ol]:            $(WRITER_CC)
$(ARCH)/writeutils.*[ol]:        $(WRITEUTILS_CC)
$(ARCH)/xbitmap.*[ol]:           $(XBITMAP_CC)
$(ARCH)/xbmaprenderer.*[ol]:     $(XBMAPRENDERER_CC)

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Compile whatever is in the test.cc file and link it
test: test.cc
	$(CC) $(DEBUG_LINK_OPTIONS) test.cc $(DEBUG_OBJECTS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) $(LIBRARIES)

# Make html doc files for all *.h files
html:
	cxx2html -dir ~/www/grsh *.h


# (This target is broken) 
share:
	$(CC) -c +Z $(DEBUG_OPTIM) $(DEBUG_OPTIONS) $(INCLUDE_DIRS) -c face.cc

# Remove all object files and build a new production executable from scratch
new: cleaner $(PROD_TARGET)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f *~ \#* core

# Make clean, and also remove all object files
cleaner: clean
	rm -f *.o

# Generate TAGS file based on all source files
TAGS_FILES = *\.[ch]* util/*\.[ch]*
TAGS: $(TAGS_FILES)
ifeq ($(ARCH),hp9000s700)
	echo 'etags $(TAGS_FILES)'
else
	etags $(TAGS_FILES)
endif

# Start emacs and load all source files and Makefile
edit: clean
	emacs *\.[ch]* Makefile testing/grshtest.tcl Log.txt

# Count the lines in all source files
count:
	wc -l *\.[ch]*

# Count the number of non-commented source lines
ncsl:
	NCSL *\.[ch]*

docs: DoxygenConfig *.h
	doxygen DoxygenConfig
#	/cit/rjpeters/packages/docxx/bin/doc++ -H -B /cit/rjpeters/grsh/Doc/footer.html -d /cit/rjpeters/www/grsh Docs.hh

AdepAliases: FileList *.h *.cc
	adep -s -fFileList > AdepAliases

cdeps: *.h *.cc
	cdep -iCdepSearchpath *.h *.cc > CdepDeps

ldeps: cdeps
	ldep -dCdepDeps -L > Ldeps

backup:
	tclsh Backup.tcl

