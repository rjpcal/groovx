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
	ARCH_FLAGS = -DGCC_COMPILER -DIRIX6
	SHLIB_FLAG = 
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

INCLUDE_DIRS = -I$(HOME)/include $(STL_INCLUDE_DIR)

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
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

TEST_TARGET = testsh

ifeq ($(ARCH),hp9000s700)
	TEST_OPTIONS = +Z +p +w +W818,655,392,495,469,361,749,416
	TEST_OPTIM = -g
	TEST_LINK_OPTIONS = -Wl,-B,immediate -Wl,+vallcompatwarnings
endif
ifeq ($(ARCH),irix6)
	TEST_OPTIONS =
	TEST_OPTIM =
	TEST_LINK_OPTIONS =
endif

TEST_FLAGS = -DPROF -DASSERT -DINVARIANT -DTEST

STATIC_OBJECTS = \
	$(ARCH)/bitmap.do \
	$(ARCH)/face.do \
	$(ARCH)/fish.do \
	$(ARCH)/fixpt.do \
	$(ARCH)/gabor.do \
	$(ARCH)/glbitmap.do \
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
	$(ARCH)/tlist.do \
	$(ARCH)/toglconfig.do \
	$(ARCH)/trial.do \
	$(ARCH)/trialevent.do \
	$(ARCH)/xbitmap.do

DYNAMIC_OBJECTS = \
	$(ARCH)/bitmaptcl.do \
	$(ARCH)/block.do \
	$(ARCH)/blocklist.do \
	$(ARCH)/blocktcl.do \
	$(ARCH)/cloneface.do \
	$(ARCH)/demangle.do \
	$(ARCH)/error.do \
	$(ARCH)/exptdriver.do \
	$(ARCH)/expttcl.do \
	$(ARCH)/expttesttcl.do \
	$(ARCH)/facetcl.do \
	$(ARCH)/fishtcl.do \
	$(ARCH)/fixpttcl.do \
	$(ARCH)/gabortcl.do \
	$(ARCH)/gfxattribs.do \
	$(ARCH)/grobjtcl.do \
	$(ARCH)/gtexttcl.do \
	$(ARCH)/housetcl.do \
	$(ARCH)/id.do \
	$(ARCH)/io.do \
	$(ARCH)/iofactory.do \
	$(ARCH)/iomgr.do \
	$(ARCH)/iostl.do \
	$(ARCH)/ioutils.do \
	$(ARCH)/jittertcl.do \
	$(ARCH)/kbdresponsehdlr.do \
	$(ARCH)/masktcl.do \
	$(ARCH)/misctcl.do \
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
	$(ARCH)/responsehandler.do \
	$(ARCH)/rhlist.do \
	$(ARCH)/rhtcl.do \
	$(ARCH)/sound.do \
	$(ARCH)/soundlist.do \
	$(ARCH)/soundtcl.do \
	$(ARCH)/stringifycmd.do \
	$(ARCH)/subject.do \
	$(ARCH)/subjecttcl.do \
	$(ARCH)/tclcmd.do \
	$(ARCH)/tcldlist.do \
	$(ARCH)/tclerror.do \
	$(ARCH)/tclitempkg.do \
	$(ARCH)/tclpkg.do \
	$(ARCH)/tclvalue.do \
	$(ARCH)/thlist.do \
	$(ARCH)/thtcl.do \
	$(ARCH)/timinghandler.do \
	$(ARCH)/timinghdlr.do \
	$(ARCH)/tlisttcl.do \
	$(ARCH)/trace.do \
	$(ARCH)/trialtcl.do \
	$(ARCH)/value.do

TEST_STATIC_OBJECTS = $(STATIC_OBJECTS)
TEST_DYNAMIC_OBJECTS = $(DYNAMIC_OBJECTS) $(SOUND_OBJECTS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET = grsh0.6a2

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

PROD_STATIC_OBJECTS = $(TEST_STATIC_OBJECTS:.do=.o)
PROD_DYNAMIC_OBJECTS = $(TEST_DYNAMIC_OBJECTS:.do=.o)

#-------------------------------------------------------------------------
#
# Pattern rules to build %.o (production) and %.do (test/debug) object files
#
#-------------------------------------------------------------------------

COMMON_OPTIONS = $(ARCH_FLAGS) $(INCLUDE_DIRS)

ALL_PROD_OPTIONS = $(COMMON_OPTIONS) $(PROD_OPTIM) $(PROD_OPTIONS) 
ALL_TEST_OPTIONS = $(COMMON_OPTIONS) $(TEST_OPTIM) $(TEST_OPTIONS) $(TEST_FLAGS)

$(ARCH)/%.o : %.cc
	$(CC) -c $< -o $@ $(ALL_PROD_OPTIONS)

$(ARCH)/%.do : %.cc;
	$(CC) -c $< -o $@ $(ALL_TEST_OPTIONS)

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

SHLIB_LD =
ifeq ($(ARCH),hp9000s700)
	SHLIB_LD = $(CC) $(SHLIB_FLAG)
	SHLIB_EXT = sl
endif
ifeq ($(ARCH),irix6)
	SHLIB_LD = ld -n32 -shared -check_registry /usr/lib32/so_locations
	SHLIB_EXT = so
endif

LIBVISX = $(ARCH)/libvisx.$(SHLIB_EXT)
LIBVISX_D = $(ARCH)/libvisx.d.$(SHLIB_EXT)
ifeq ($(ARCH),irix6)
	LIBVISX = $(ARCH)/libvisx.a
endif

testsh: TAGS $(HOME)/bin/$(ARCH)/$(TEST_TARGET)
	$(TEST_TARGET) ./testing/grshtest.tcl

$(HOME)/bin/$(ARCH)/$(TEST_TARGET): $(TEST_STATIC_OBJECTS) $(LIBVISX_D)
	$(CC) $(TEST_LINK_OPTIONS) -o $@ $(TEST_STATIC_OBJECTS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) -lvisx.d $(LIBRARIES)

$(ARCH)/libvisx.d.sl: $(TEST_DYNAMIC_OBJECTS)
	$(CC) $(SHLIB_FLAG) -o $(ARCH)/libvisx.d.sl $(TEST_DYNAMIC_OBJECTS)

.PHONY: Prof
Prof: testing/grshtest.tcl
	$(TEST_TARGET) ./testing/grshtest.tcl 2> ./Prof
	sort +3 Prof > Prof.sort

grsh: TAGS $(HOME)/bin/$(ARCH)/$(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

$(HOME)/bin/$(ARCH)/$(PROD_TARGET): $(PROD_STATIC_OBJECTS) $(LIBVISX)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_STATIC_OBJECTS) \
	$(LIB_DIRS) -lvisx $(LIBRARIES)

# Note: exception handling does not work with shared libraries in the
# current version of g++ (2.95.1), so on irix6 we must make the
# libvisx library as an archive library.

$(LIBVISX): $(PROD_DYNAMIC_OBJECTS)
ifeq ($(ARCH),irix6)
#	rm -rf $(LIBVISX)
	ar rus $(LIBVISX) $(PROD_DYNAMIC_OBJECTS)
endif
ifeq ($(ARCH),hp9000s700)
	$(SHLIB_LD) -o $(LIBVISX) $(PROD_DYNAMIC_OBJECTS)
endif

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
DEBUG_H = debug.h
DEMANGLE_H = demangle.h
ERRMSG_H = errmsg.h
ERROR_H = error.h
GFXATTRIBS_H = gfxattribs.h
ID_H = id.h
IOUTILS_H = ioutils.h
OBJLISTTCL_H = objlisttcl.h
OBJTOGL_H = objtogl.h
OBSERVABLE_H = observable.h
OBSERVER_H = observer.h
POSLISTTCL_H = poslisttcl.h
RAND_H = rand.h
RANDUTILS_H = randutils.h
RECT_H = rect.h
STLUTILS_H = stlutils.h
TCLDLIST_H = tcldlist.h
TCLLINK_H = tcllink.h
TCLOBJLOCK_H = tclobjlock.h
TCLPKG_H = tclpkg.h
TIMEUTILS_H = timeutils.h
TLISTTCL_H = tlisttcl.h
TRACE_H = trace.h

#
# level 1 headers
#
FACTORY_H = $(ERROR_H) $(DEMANGLE_H) factory.h
GCCDEMANGLE_CC = $(TRACE_H) $(DEBUG_H) gccdemangle.cc
HPSOUND_CC = $(TRACE_H) $(DEBUG_H) hpsound.cc
IO_H = $(ERROR_H) io.h
IRIXSOUND_CC = $(TRACE_H) $(DEBUG_H) irixsound.cc
PBM_H = $(ERROR_H) pbm.h
TCLERROR_H = $(ERROR_H) tclerror.h
TCLEVALCMD_H = $(TCLOBJLOCK_H) tclevalcmd.h
TOGLCONFIG_H = $(RECT_H) toglconfig.h
VALUE_H = $(ERROR_H) value.h

#
# level 2 headers
#
BLOCK_H = $(IO_H) block.h
EXPTDRIVER_H = $(IO_H) $(ERROR_H) exptdriver.h
GROBJ_H = $(IO_H) $(OBSERVABLE_H) $(OBSERVER_H) grobj.h
IOFACTORY_H = $(FACTORY_H) $(IO_H) iofactory.h
IOSTL_H = $(IO_H) iostl.h
POSITION_H = $(IO_H) position.h
PROPERTY_H = $(IO_H) $(OBSERVABLE_H) $(VALUE_H) property.h
PTRLIST_H = $(IO_H) $(ERROR_H) ptrlist.h
RESPONSEHANDLER_H = $(IO_H) responsehandler.h 
SOUND_H = $(ERROR_H) $(IO_H) sound.h
SUBJECT_H = $(IO_H) subject.h
TCLVALUE_H = $(VALUE_H) tclvalue.h
TIMINGHDLR_H = $(IO_H) timinghdlr.h
TRIAL_H = $(ID_H) $(IO_H) $(ERROR_H) trial.h
TRIALEVENT_H = $(IO_H) trialevent.h

#
# level 3 headers
#
BITMAP_H = $(GROBJ_H) $(RECT_H) bitmap.h
BLOCKLIST_H = $(PTRLIST_H) blocklist.h
FACE_H = $(GROBJ_H) $(PROPERTY_H) face.h
FISH_H = $(GROBJ_H) $(PROPERTY_H) fish.h
FIXPT_H = $(GROBJ_H) $(PROPERTY_H) fixpt.h
GABOR_H = $(GROBJ_H) $(PROPERTY_H) gabor.h
GTEXT_H = $(GROBJ_H) $(ERROR_H) gtext.h
HOUSE_H = $(GROBJ_H) $(PROPERTY_H) house.h
IOMGR_H = $(IO_H) $(IOFACTORY_H) iomgr.h
JITTER_H = $(POSITION_H) jitter.h
KBDRESPONSEHDLR_H = $(RESPONSEHANDLER_H) kbdresponsehdlr.h
MASKHATCH_H = $(GROBJ_H) $(PROPERTY_H) maskhatch.h
MORPHYFACE_H = $(GROBJ_H) $(PROPERTY_H) morphyface.h
NULLRESPONSEHDLR_H = $(RESPONSEHANDLER_H) nullresponsehdlr.h
OBJLIST_H = $(PTRLIST_H) $(ID_H) objlist.h
POSLIST_H = $(PTRLIST_H) $(ID_H) poslist.h
RHLIST_H = $(PTRLIST_H) rhlist.h
SOUNDLIST_H = $(PTRLIST_H) soundlist.h
TCLCMD_H = $(TCLVALUE_H) $(TCLERROR_H) tclcmd.h
THLIST_H = $(PTRLIST_H) thlist.h
TIMINGHANDLER_H = $(TIMINGHDLR_H) timinghandler.h
TLIST_H = $(PTRLIST_H) $(IO_H) tlist.h

#
# level 4 headers
#
CLONEFACE_H = $(FACE_H) cloneface.h
GLBITMAP_H = $(BITMAP_H) glbitmap.h
STRINGIFYCMD_H = $(TCLCMD_H) stringifycmd.h
PTRLIST_CC = $(PTRLIST_H) $(DEMANGLE_H) $(IOMGR_H) ptrlist.cc
TCLITEMPKG_H = $(TCLPKG_H) $(TCLCMD_H) $(PROPERTY_H) tclitempkg.h
XBITMAP_H = $(BITMAP_H) xbitmap.h

#
# level 5 headers
#
LISTITEMPKG_H = $(TCLITEMPKG_H) listitempkg.h
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

BITMAP_CC = $(BITMAP_H) $(ERROR_H) $(PBM_H) $(TRACE_H) $(DEBUG_H) bitmap.cc

BITMAPTCL_CC = $(BITMAP_H) $(GLBITMAP_H) $(XBITMAP_H) $(IOMGR_H) $(OBJLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) bitmaptcl.cc

BLOCK_CC = $(BLOCK_H) $(RAND_H) $(IOSTL_H) $(TLIST_H) $(TRIAL_H) \
	$(TIMEUTILS_H) $(TRACE_H) $(DEBUG_H) block.cc

BLOCKLIST_CC = $(BLOCKLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(BLOCK_H) $(PTRLIST_CC) blocklist.cc

BLOCKTCL_CC = $(IOMGR_H) $(BLOCKLIST_H) $(BLOCK_H) $(TCLCMD_H) \
	$(LISTITEMPKG_H) $(LISTPKG_H) $(TRACE_H) $(DEBUG_H) blocktcl.cc

CLONEFACE_CC = $(CLONEFACE_H) $(TRACE_H) $(DEBUG_H) cloneface.cc

DEMANGLE_CC = $(DEMANGLE_H) $(GCCDEMANGLE_CC) demangle.cc

ERROR_CC = $(ERROR_H) $(TRACE_H) $(DEBUG_H) error.cc

EXPTDRIVER_CC = $(EXPTDRIVER_H) $(TCLERROR_H) $(BLOCK_H) \
	$(RESPONSEHANDLER_H) $(TCLEVALCMD_H) $(TIMINGHDLR_H) $(TLISTTCL_H) \
	$(TRIAL_H) $(OBJLIST_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(POSLIST_H) \
	$(BLOCKLIST_H) $(RHLIST_H) $(THLIST_H) $(TLIST_H) $(TIMEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) exptdriver.cc

EXPTTCL_CC = $(TCLEVALCMD_H) $(EXPTDRIVER_H) \
	$(TCLITEMPKG_H) $(TCLCMD_H) $(OBJTOGL_H) $(TOGLCONFIG_H) \
	$(TRACE_H) $(DEBUG_H) expttcl.cc

EXPTTESTTCL_CC = $(TCLLINK_H) expttesttcl.cc

FACE_CC = $(FACE_H) $(GFXATTRIBS_H) $(TRACE_H) $(DEBUG_H) face.cc

FACETCL_CC = $(CLONEFACE_H) $(IOMGR_H) $(OBJLIST_H) $(FACE_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) facetcl.cc

FISH_CC = $(FISH_H) $(ERROR_H) $(TRACE_H) $(DEBUG_H) fish.cc

FISHTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(LISTITEMPKG_H) $(FISH_H) fishtcl.cc

FIXPT_CC = $(FIXPT_H) fixpt.cc

FIXPTTCL_CC = $(OBJLIST_H) $(FIXPT_H) $(PROPITEMPKG_H) $(TRACE_H) fixpttcl.cc

GABOR_CC = $(GABOR_H) $(RANDUTILS_H) $(TRACE_H) $(DEBUG_H) gabor.cc

GABORTCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(GABOR_H) $(TRACE_H) gabortcl.cc

GFXATTRIBS_CC = $(GFXATTRIBS_H) gfxattribs.cc

GLBITMAP_CC = $(GLBITMAP_H) $(TRACE_H) $(DEBUG_H) glbitmap.cc

GROBJ_CC = $(GROBJ_H) $(GFXATTRIBS_H) $(GLBITMAP_H) $(XBITMAP_H) \
	$(ERROR_H) $(RECT_H) $(TRACE_H) $(DEBUG_H) grobj.cc

GROBJTCL_CC = $(DEMANGLE_H) $(GROBJ_H) $(OBJLIST_H) $(LISTITEMPKG_H) grobjtcl.cc

GRSHAPPINIT_CC = $(TRACE_H) grshAppInit.cc

GTEXT_CC = $(GTEXT_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(TRACE_H) $(DEBUG_H) gtext.cc

GTEXTTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(GTEXT_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) gtexttcl.cc

HOUSE_CC = $(HOUSE_H) $(TRACE_H) $(DEBUG_H) house.cc

HOUSETCL_CC = $(HOUSE_H) $(OBJLIST_H) $(PROPITEMPKG_H) $(TRACE_H) housetcl.cc

ID_CC = $(ID_H) $(OBJLIST_H) $(POSLIST_H) id.cc

IO_CC = $(IO_H) $(DEMANGLE_H) $(TRACE_H) $(DEBUG_H) io.cc

IOFACTORY_CC = $(IOFACTORY_H) iofactory.cc

IOMGR_CC = $(IOMGR_H) $(TRACE_H) $(DEBUG_H) iomgr.cc

IOSTL_CC = $(IOSTL_H) iostl.cc

IOUTILS_CC = $(IOUTILS_H) $(IO_H) ioutils.cc

JITTER_CC = $(JITTER_H) $(RANDUTILS_H) $(TRACE_H) $(DEBUG_H) jitter.cc

JITTERTCL_CC = $(IOMGR_H) $(JITTER_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TRACE_H) $(DEBUG_H) jittertcl.cc

KBDRESPONSEHDLR_CC = $(KBDRESPONSEHDLR_H) $(EXPTDRIVER_H) \
	$(TCLEVALCMD_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(SOUND_H) $(SOUNDLIST_H) \
	$(TRACE_H) $(DEBUG_H) kbdresponsehdlr.cc

MASKHATCH_CC = $(MASKHATCH_H) $(TRACE_H) $(DEBUG_H) maskhatch.cc

MASKTCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(MASKHATCH_H) \
	$(TRACE_H) masktcl.cc

MISCTCL_CC = $(RANDUTILS_H) misctcl.cc

MORPHYFACE_CC = $(MORPHYFACE_H) $(BEZIER_H) $(GFXATTRIBS_H) \
	$(TRACE_H) $(DEBUG_H) morphyface.cc

MORPHYFACETCL_CC = $(OBJLIST_H) $(PROPITEMPKG_H) $(MORPHYFACE_H) \
	$(TRACE_H) morphyfacetcl.cc

NULLRESPONSEHDLR_CC = $(NULLRESPONSEHDLR_H) $(EXPTDRIVER_H) \
	$(TRACE_H) nullresponsehdlr.cc

OBJLIST_CC = $(OBJLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(GROBJ_H) $(PTRLIST_CC) objlist.cc

OBJLISTTCL_CC = $(GROBJ_H) $(IOMGR_H) $(OBJLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) objlisttcl.cc

OBJTOGL_CC = $(OBJTOGL_H) $(GFXATTRIBS_H) $(TLIST_H) $(TCLCMD_H) \
	$(TCLEVALCMD_H) $(TCLITEMPKG_H) $(TOGLCONFIG_H) \
	$(TRACE_H) $(DEBUG_H) objtogl.cc

OBSERVABLE_CC = $(OBSERVABLE_H) $(OBSERVER_H) \
	$(TRACE_H) $(DEBUG_H) observable.cc

OBSERVER_CC = $(OBSERVER_H) $(TRACE_H) observer.cc

PBM_CC = $(PBM_H) $(TRACE_H) $(DEBUG_H) pbm.cc

POSITION_CC = $(POSITION_H) $(TRACE_H) $(DEBUG_H) position.cc

POSITIONTCL_CC = $(DEMANGLE_H) $(IOMGR_H) $(POSITION_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) positiontcl.cc

POSLIST_CC = $(POSLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(POSITION_H) $(PTRLIST_CC) poslist.cc

POSLISTTCL_CC = $(POSLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) poslisttcl.cc

PROPERTY_CC = $(PROPERTY_H) property.cc

RESPONSEHANDLER_CC = $(RESPONSEHANDLER_H) $(TRACE_H) responsehandler.cc

RHLIST_CC = $(RHLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(RESPONSEHANDLER_H) $(PTRLIST_CC) rhlist.cc

RHTCL_CC = $(IOMGR_H) $(RHLIST_H) $(RESPONSEHANDLER_H) $(TCLCMD_H) \
	$(KBDRESPONSEHDLR_H) $(NULLRESPONSEHDLR_H) $(LISTITEMPKG_H) $(LISTPKG_H) \
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
	$(TRACE_H) $(DEBUG_H) stringifycmd.cc

SUBJECT_CC = $(SUBJECT_H) $(IOUTILS_H) $(TRACE_H) $(DEBUG_H) subject.cc

SUBJECTTCL_CC = $(ERRMSG_H) $(SUBJECT_H) $(TRACE_H) $(DEBUG_H) subjecttcl.cc

TCLCMD_CC = $(TCLCMD_H) $(DEMANGLE_H) $(ERRMSG_H) $(TCLVALUE_H) \
	$(TRACE_H) $(DEBUG_H) tclcmd.cc

TCLDLIST_CC = $(TCLDLIST_H) $(ERRMSG_H) $(TRACE_H) $(DEBUG_H) tcldlist.cc

TCLERROR_CC = $(TCLERROR_H) $(TRACE_H) tclerror.cc

TCLGL_CC = $(TCLPKG_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclgl.cc

TCLITEMPKG_CC = $(TCLITEMPKG_H) $(TCLCMD_H) $(STRINGIFYCMD_H) \
	$(TRACE_H) $(DEBUG_H) tclitempkg.cc

TCLPKG_CC = $(TCLPKG_H) $(TCLLINK_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclpkg.cc

TCLVALUE_CC = $(TCLVALUE_H) $(TRACE_H) $(DEBUG_H) tclvalue.cc

THLIST_CC = $(THLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(TIMINGHDLR_H) $(PTRLIST_CC) thlist.cc

THTCL_CC = $(IOMGR_H) $(THLIST_H) $(TCLCMD_H) $(TIMINGHDLR_H) \
	$(TIMINGHANDLER_H) $(TRIALEVENT_H) $(LISTITEMPKG_H) $(LISTPKG_H)\
	 $(TRACE_H) $(DEBUG_H) thtcl.cc

TIMINGHANDLER_CC = $(TIMINGHANDLER_H) $(TRIALEVENT_H) \
	$(TRACE_H) $(DEBUG_H) timinghandler.cc

TIMINGHDLR_CC = $(TIMINGHDLR_H) $(IOMGR_H) $(TRIALEVENT_H) \
	$(TIMEUTILS_H) $(TRACE_H) $(DEBUG_H) timinghdlr.cc

TLIST_CC = $(TLIST_H) $(TRIAL_H) $(TRACE_H) $(DEBUG_H) \
	$(PTRLIST_CC) tlist.cc

TLISTTCL_CC = $(TLISTTCL_H) $(TLIST_H) $(TRIAL_H) $(GROBJ_H) $(GTEXT_H) \
	$(POSITION_H) \
	$(OBJLIST_H) $(POSLIST_H) $(OBJLISTTCL_H) $(POSLISTTCL_H) \
	$(TCLCMD_H) $(LISTPKG_H) $(RECT_H) $(TRACE_H) $(DEBUG_H) tlisttcl.cc

TOGLCONFIG_CC = $(TOGLCONFIG_H) $(ERROR_H) $(GFXATTRIBS_H) $(TCLEVALCMD_H) \
	$(TRACE_H) $(DEBUG_H) toglconfig.cc

TRACE_CC = $(TRACE_H) trace.cc

TRIAL_CC = $(TRIAL_H) $(OBJLIST_H) $(POSLIST_H) $(GROBJ_H) $(POSITION_H) \
	$(TRACE_H) $(DEBUG_H) trial.cc

TRIALEVENT_CC = $(TRIALEVENT_H) $(DEMANGLE_H) $(EXPTDRIVER_H) $(TIMEUTILS_H) \
	$(TRACE_H) $(DEBUG_H) trialevent.cc

TRIALTCL_CC = $(IOMGR_H) $(TRIAL_H) $(TLIST_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) trialtcl.cc

VALUE_CC = $(VALUE_H) value.cc

XBITMAP_CC = $(XBITMAP_H) $(TOGLCONFIG_H) $(OBJTOGL_H) \
	$(TRACE_H) $(DEBUG_H) xbitmap.cc

#-------------------------------------------------------------------------
#
# Dependency lists for object files
#
#-------------------------------------------------------------------------

$(ARCH)/bitmap.*[ol]:            $(BITMAP_CC)
$(ARCH)/bitmaptcl.*[ol]:         $(BITMAPTCL_CC)
$(ARCH)/block.*[ol]:             $(BLOCK_CC)
$(ARCH)/blocklist.*[ol]:         $(BLOCKLIST_CC)
$(ARCH)/blocktcl.*[ol]:          $(BLOCKTCL_CC)
$(ARCH)/cloneface.*[ol]:         $(CLONEFACE_CC)
$(ARCH)/demangle.*[ol]:          $(DEMANGLE_CC)
$(ARCH)/error.*[ol]:             $(ERROR_CC)
$(ARCH)/exptdriver.*[ol]:        $(EXPTDRIVER_CC)
$(ARCH)/expttcl.*[ol]:           $(EXPTTCL_CC)
$(ARCH)/expttesttcl.*[ol]:       $(EXPTTESTTCL_CC)
$(ARCH)/face.*[ol]:              $(FACE_CC)
$(ARCH)/facetcl.*[ol]:           $(FACETCL_CC)
$(ARCH)/fish.*[ol]:              $(FISH_CC)
$(ARCH)/fishtcl.*[ol]:           $(FISHTCL_CC)
$(ARCH)/fixpt.*[ol]:             $(FIXPT_CC)
$(ARCH)/fixpttcl.*[ol]:          $(FIXPTTCL_CC)
$(ARCH)/gabor.*[ol]:             $(GABOR_CC)
$(ARCH)/gabortcl.*[ol]:          $(GABORTCL_CC)
$(ARCH)/gfxattribs.*[ol]:        $(GFXATTRIBS_CC)
$(ARCH)/glbitmap.*[ol]:          $(GLBITMAP_CC)
$(ARCH)/grobj.*[ol]:             $(GROBJ_CC)
$(ARCH)/grobjtcl.*[ol]:          $(GROBJTCL_CC)
$(ARCH)/grshAppInit.*[ol]:       $(GRSHAPPINIT_CC)
$(ARCH)/gtext.*[ol]:             $(GTEXT_CC)
$(ARCH)/gtexttcl.*[ol]:          $(GTEXTTCL_CC)
$(ARCH)/house.*[ol]:             $(HOUSE_CC)
$(ARCH)/housetcl.*[ol]:          $(HOUSETCL_CC)
$(ARCH)/id.*[ol]:                $(ID_CC)
$(ARCH)/io.*[ol]:                $(IO_CC)
$(ARCH)/iofactory.*[ol]:         $(IOFACTORY_CC)
$(ARCH)/iomgr.*[ol]:             $(IOMGR_CC)
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
$(ARCH)/responsehandler.*[ol]:   $(RESPONSEHANDLER_CC)
$(ARCH)/rhlist.*[ol]:            $(RHLIST_CC)
$(ARCH)/rhtcl.*[ol]:             $(RHTCL_CC)
$(ARCH)/sound.*[ol]:             $(SOUND_CC)
$(ARCH)/soundlist.*[ol]:         $(SOUNDLIST_CC)
$(ARCH)/soundtcl.*[ol]:          $(SOUNDTCL_CC)
$(ARCH)/stringifycmd.*[ol]:      $(STRINGIFYCMD_CC)
$(ARCH)/subject.*[ol]:           $(SUBJECT_CC)
$(ARCH)/subjecttcl.*[ol]:        $(SUBJECTTCL_CC)
$(ARCH)/tcldlist.*[ol]:          $(TCLDLIST_CC)
$(ARCH)/tclcmd.*[ol]:            $(TCLCMD_CC)
$(ARCH)/tclerror.*[ol]:          $(TCLERROR_CC)
$(ARCH)/tclgl.*[ol]:             $(TCLGL_CC)
$(ARCH)/tclitempkg.*[ol]:        $(TCLITEMPKG_CC)
$(ARCH)/tclpkg.*[ol]:            $(TCLPKG_CC)
$(ARCH)/tclvalue.*[ol]:          $(TCLVALUE_CC)
$(ARCH)/thlist.*[ol]:            $(THLIST_CC)
$(ARCH)/thtcl.*[ol]:             $(THTCL_CC)
$(ARCH)/timinghdlr.*[ol]:        $(TIMINGHDLR_CC)
$(ARCH)/timinghandler.*[ol]:     $(TIMINGHANDLER_CC)
$(ARCH)/tlist.*[ol]:             $(TLIST_CC)
$(ARCH)/tlisttcl.*[ol]:          $(TLISTTCL_CC)
$(ARCH)/toglconfig.*[ol]:        $(TOGLCONFIG_CC)
$(ARCH)/trace.*[ol]:             $(TRACE_CC)
$(ARCH)/trial.*[ol]:             $(TRIAL_CC)
$(ARCH)/trialevent.*[ol]:        $(TRIALEVENT_CC)
$(ARCH)/trialtcl.*[ol]:          $(TRIALTCL_CC)
$(ARCH)/value.*[ol]:             $(VALUE_CC)
$(ARCH)/xbitmap.*[ol]:           $(XBITMAP_CC)

#-------------------------------------------------------------------------
#
# Miscellaneous targets
#
#-------------------------------------------------------------------------

# Compile whatever is in the test.cc file and link it
test: test.cc
	$(CC) $(TEST_LINK_OPTIONS) test.cc $(TEST_OBJECTS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) $(LIBRARIES)


# (This target is broken) 
share:
	$(CC) -c +Z $(TEST_OPTIM) $(TEST_OPTIONS) $(INCLUDE_DIRS) -c face.cc

# Remove all object files and build a new production executable from scratch
new: cleaner $(PROD_TARGET)

# Remove all backups, temporaries, and coredumps
clean:
	rm -f *~ \#* core

# Make clean, and also remove all object files
cleaner: clean
	rm -f *.o

# Generate TAGS file based on all source files
TAGS: *\.[ch]*
	etags *\.[ch]*

# Start emacs and load all source files and Makefile
edit: clean
	emacs *\.[ch]* Makefile testing/grshtest.tcl Log.txt

# Count the lines in all source files
count:
	wc -l *\.[ch]*

# Count the number of non-commented source lines
ncsl:
	NCSL *\.[ch]*
