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

CC = aCC

#-------------------------------------------------------------------------
#
# Directories to search for include files and code libraries
#
#-------------------------------------------------------------------------

HOME_DIR = /cit/rjpeters

INCLUDE_DIRS = -I$(HOME)/include

LIB_DIRS =  -L/opt/graphics/OpenGL/lib \
	-L$(HOME)/lib \
	-L/opt/audio/lib

LIBRARIES = \
	-ltogl -lGLU -lGL \
	-ltk -ltcl -lXmu \
	-lX11 -lXext \
	-lm -lAlib

#-------------------------------------------------------------------------
#
# Info for testing/debugging executable
#
#-------------------------------------------------------------------------

TEST_TARGET = testsh
TEST_OPTIONS = +p +w +W818,655,392,495,469,361,749,416
TEST_OPTIM = -g
TEST_LINK_OPTIONS = -Wl,-B,immediate -Wl,+vallcompatwarnings

TEST_FLAGS = -DPROF -DASSERT -DINVARIANT -DTEST

STATIC_OBJECTS = 

DYNAMIC_OBJECTS = 

TEST_OBJECTS =	\
	bitmap.do \
	bitmaptcl.sl \
	block.sl \
	blocklist.sl \
	blocktcl.sl \
	cloneface.sl \
	error.sl \
	exptdriver.sl \
	expttcl.sl \
	expttesttcl.sl \
	face.do \
	facetcl.sl \
	fish.do \
	fishtcl.sl \
	fixpt.do \
	fixpttcl.sl \
	gabor.do \
	gabortcl.sl \
	gfxattribs.sl \
	glbitmap.do \
	grobj.do \
	grobjtcl.sl \
	grshAppInit.do \
	gtext.do \
	gtexttcl.sl \
	house.do \
	housetcl.sl \
	id.sl \
	io.sl \
	iofactory.sl \
	iomgr.sl \
	iostl.sl \
	ioutils.sl \
	jitter.do \
	jittertcl.sl \
	kbdresponsehdlr.sl \
	maskhatch.do \
	masktcl.sl \
	misctcl.sl \
	morphyface.do \
	morphyfacetcl.sl \
	nullresponsehdlr.sl \
	objlist.sl \
	objlisttcl.sl \
	objtogl.do \
	observable.sl \
	observer.sl \
	pbm.sl \
	position.do \
	positiontcl.sl \
	poslist.sl \
	poslisttcl.sl \
	property.sl \
	responsehandler.sl \
	rhlist.sl \
	rhtcl.sl \
	sound.sl \
	soundlist.sl \
	soundtcl.sl \
	stringifycmd.sl \
	subject.sl \
	subjecttcl.sl \
	tclgl.do \
	tclcmd.sl \
	tcldlist.sl \
	tclerror.sl \
	tclitempkg.sl \
	tclpkg.sl \
	tclvalue.sl \
	thlist.sl \
	thtcl.sl \
	timinghandler.sl \
	timinghdlr.sl \
	tlist.do \
	tlisttcl.sl \
	toglconfig.do \
	trace.sl \
	trial.do \
	trialtcl.sl \
	trialevent.do \
	value.sl \
	xbitmap.do

TEST_DEPENDS = $(TEST_OBJECTS)

#-------------------------------------------------------------------------
#
# Info for production executable
#
#-------------------------------------------------------------------------

PROD_TARGET = grsh0.6a2
PROD_OPTIONS = +p
PROD_OPTIM = +O3
PROD_LINK_OPTIONS = -Wl,+vallcompatwarnings

TEMP_1 = $(TEST_OBJECTS:.do=.o)
PROD_OBJECTS = $(TEMP_1:.sl=.o)

#-------------------------------------------------------------------------
#
# Suffix rules to build *.o (production) and *.do (test/debug) object files
#
#-------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .cc .o .do .sl

.cc.o:
	$(CC) $(PROD_OPTIM) $(PROD_OPTIONS) -o $*.o $(INCLUDE_DIRS) -c $*.cc

.cc.do:
	$(CC) -c +Z $(TEST_OPTIM) $(TEST_OPTIONS) -o $*.do \
	$(INCLUDE_DIRS) $(TEST_FLAGS) $*.cc

.do.sl:; $(CC) -b -o $*.sl $*.do

#-------------------------------------------------------------------------
#
# Build rules for production and test/debug executables
#
#-------------------------------------------------------------------------

testsh: TAGS $(HOME)/bin/$(ARCH)/$(TEST_TARGET)
	$(TEST_TARGET) ./testing/grshtest.tcl

$(HOME)/bin/$(ARCH)/$(TEST_TARGET): $(TEST_DEPENDS)
	$(CC) $(TEST_LINK_OPTIONS) -o $@ $(TEST_OBJECTS) /opt/langtools/lib/end.o \
	$(LIB_DIRS) $(LIBRARIES)

.PHONY: Prof
Prof: testing/grshtest.tcl
	$(TEST_TARGET) ./testing/grshtest.tcl 2> ./Prof
	sort +3 Prof > Prof.sort

grsh: $(HOME)/bin/$(ARCH)/$(PROD_TARGET)
	$(PROD_TARGET) ./testing/grshtest.tcl

$(HOME)/bin/$(ARCH)/$(PROD_TARGET): TAGS $(PROD_OBJECTS)
	$(CC) $(PROD_LINK_OPTIONS) -o $@ $(PROD_OBJECTS) $(LIB_DIRS) $(LIBRARIES)

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
ERRMSG_H = errmsg.h
ERROR_H = error.h
GFXATTRIBS_H = gfxattribs.h
ID_H = id.h
IOUTILS_H = ioutils.h
OBJTOGL_H = objtogl.h
OBSERVABLE_H = observable.h
OBSERVER_H = observer.h
RAND_H = rand.h
RANDUTILS_H = randutils.h
RECT_H = rect.h
STLUTILS_H = stlutils.h
TCLLINK_H = tcllink.h
TCLOBJLOCK_H = tclobjlock.h
TCLPKG_H = tclpkg.h
TIMEUTILS_H = timeutils.h
TLISTTCL_H = tlisttcl.h
TRACE_H = trace.h

#
# level 1 headers
#
FACTORY_H = $(ERROR_H) factory.h
IO_H = $(ERROR_H) io.h
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
PTRLIST_CC = $(PTRLIST_H) $(IOMGR_H) ptrlist.cc
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
	$(EXPT_H) $(PTRLIST_CC) blocklist.cc

BLOCKTCL_CC = $(IOMGR_H) $(BLOCKLIST_H) $(EXPT_H) $(TCLCMD_H) $(LISTITEMPKG_H) \
	$(LISTPKG_H) $(TRACE_H) $(DEBUG_H) blocktcl.cc

CLONEFACE_CC = $(CLONEFACE_H) $(TRACE_H) $(DEBUG_H) cloneface.cc

ERROR_CC = $(ERROR_H) $(TRACE_H) $(DEBUG_H) error.cc

EXPTDRIVER_CC = $(EXPTDRIVER_H) $(TCLERROR_H) $(EXPT_H) $(OBJTOGL_H) \
	$(TOGLCONFIG_H) \
	$(RESPONSEHANDLER_H) $(TCLEVALCMD_H) $(TIMINGHDLR_H) $(TLISTTCL_H) \
	$(TRIAL_H) $(OBJLIST_H) $(POSLIST_H) $(BLOCKLIST_H) $(RHLIST_H) \
	$(THLIST_H) $(TLIST_H) $(TIMEUTILS_H) $(TRACE_H) $(DEBUG_H) exptdriver.cc

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

GROBJ_CC = $(GROBJ_H) $(GLBITMAP_H) $(XBITMAP_H) \
	$(ERROR_H) $(RECT_H) $(TRACE_H) $(DEBUG_H) grobj.cc

GROBJTCL_CC = $(LISTITEMPKG_H) $(GROBJ_H) $(OBJLIST_H) grobjtcl.cc

GRSHAPPINIT_CC = $(TRACE_H) grshAppInit.cc

GTEXT_CC = $(GTEXT_H) $(OBJTOGL_H) $(TOGLCONFIG_H) $(TRACE_H) $(DEBUG_H) gtext.cc

GTEXTTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(GTEXT_H) $(LISTITEMPKG_H) \
	$(TRACE_H) $(DEBUG_H) gtexttcl.cc

HOUSE_CC = $(HOUSE_H) $(TRACE_H) $(DEBUG_H) house.cc

HOUSETCL_CC = $(HOUSE_H) $(OBJLIST_H) $(PROPITEMPKG_H) $(TRACE_H) housetcl.cc

ID_CC = $(ID_H) $(OBJLIST_H) $(POSLIST_H) id.cc

IO_CC = $(IO_H) $(TRACE_H) $(DEBUG_H) io.cc

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

OBJLISTTCL_CC = $(IOMGR_H) $(OBJLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) objlisttcl.cc

PBM_CC = $(PBM_H) $(TRACE_H) $(DEBUG_H) pbm.cc

POSITIONTCL_CC = $(IOMGR_H) $(POSITION_H) $(POSLIST_H) \
	$(LISTITEMPKG_H) $(TCLCMD_H) $(TRACE_H) $(DEBUG_H) positiontcl.cc

POSLISTTCL_CC = $(POSLIST_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) poslisttcl.cc

PROPERTY_CC = $(PROPERTY_H) property.cc

RESPONSEHANDLER_CC = $(RESPONSEHANDLER_H) $(TRACE_H) responsehandler.cc

RHTCL_CC = $(IOMGR_H) $(RHLIST_H) $(RESPONSEHANDLER_H) $(TCLCMD_H) \
	$(KBDRESPONSEHDLR_H) $(NULLRESPONSEHDLR_H) $(LISTITEMPKG_H) $(LISTPKG_H) \
	$(TRACE_H) $(DEBUG_H) rhtcl.cc

SOUND_CC = $(SOUND_H) sound.cc

SOUNDLIST_CC = $(SOUNDLIST_H) $(TRACE_H) $(DEBUG_H) \
	$(SOUND_H) $(PTRLIST_CC) soundlist.cc

SOUNDTCL_CC = $(ERRMSG_H) $(SOUNDLIST_H) $(SOUND_H) \
	$(LISTPKG_H) $(LISTITEMPKG_H) $(TCLLINK_H) $(TRACE_H) $(DEBUG_H) soundtcl.cc

STRINGIFYCMD_CC = $(STRINGIFYCMD_H) $(IO_H) \
	$(TRACE_H) $(DEBUG_H) stringifycmd.cc

TCLCMD_CC = $(TCLCMD_H) $(ERRMSG_H) $(TCLVALUE_H) \
	$(TRACE_H) $(DEBUG_H) tclcmd.cc

TCLERROR_CC = $(TCLERROR_H) $(TRACE_H) tclerror.cc

TCLGL_CC = $(TCLPKG_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclgl.cc

TCLITEMPKG_CC = $(TCLITEMPKG_H) $(TCLCMD_H) $(STRINGIFYCMD_H) \
	$(TRACE_H) $(DEBUG_H) tclitempkg.cc

TCLPKG_CC = $(TCLPKG_H) $(TCLLINK_H) $(TCLCMD_H) $(TCLERROR_H) \
	$(TRACE_H) $(DEBUG_H) tclpkg.cc

TCLVALUE_CC = $(TCLVALUE_H) $(TRACE_H) $(DEBUG_H) tclvalue.cc

THTCL_CC = $(IOMGR_H) $(THLIST_H) $(TCLCMD_H) $(TIMINGHDLR_H) \
	$(TIMINGHANDLER_H) $(TRIALEVENT_H) $(LISTITEMPKG_H) $(LISTPKG_H)\
	 $(TRACE_H) $(DEBUG_H) thtcl.cc

TIMINGHDLR_CC = $(TIMINGHDLR_H) $(IOMGR_H) $(TRIALEVENT_H) \
	$(TIMEUTILS_H) $(TRACE_H) $(DEBUG_H) timinghdlr.cc

TLIST_CC = $(TLIST_H) $(TRIAL_H) $(TRACE_H) $(DEBUG_H) \
	$(PTRLIST_CC) tlist.cc

TLISTTCL_CC = $(TLISTTCL_H) $(TLIST_H) $(TRIAL_H) $(GROBJ_H) $(POSITION_H) \
	$(OBJLIST_H) $(POSLIST_H) $(OBJLISTTCL_H) $(POSLISTTCL_H) \
	$(TCLCMD_H) $(LISTPKG_H) $(TRACE_H) $(DEBUG_H) tlisttcl.cc

TRIALEVENT_CC = $(TRIALEVENT_H) $(EXPTDRIVER_H) $(TIMEUTILS_H) \
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

bitmap.*[ol]: $(BITMAP_CC)
bitmaptcl.*[ol] : $(BITMAPTCL_CC)
block.*[ol] : $(BLOCK_CC)
blocklist.*[ol]: $(BLOCKLIST_CC)
blocktcl.*[ol]: $(BLOCKTCL_CC)
cloneface.*[ol]: $(CLONEFACE_CC)
error.*[ol]: $(ERROR_CC)
exptdriver.*[ol]: $(EXPTDRIVER_CC)
expttcl.*[ol]: $(EXPTTCL_CC)
expttesttcl.*[ol]: $(EXPTTESTTCL_CC)
face.*[ol]: $(FACE_CC)
facetcl.*[ol]: $(FACETCL_CC)
fish.*[ol]: $(FISH_CC)
fishtcl.*[ol]: $(FISHTCL_CC)
fixpt.*[ol]: $(FIXPT_CC)
fixpttcl.*[ol]: $(FIXPTTCL_CC)
gabor.*[ol]: $(GABOR_CC)
gabortcl.*[ol]: $(GABORTCL_CC)
gfxattribs.*[ol]: $(GFXATTRIBS_CC)
glbitmap.*[ol]: $(GLBITMAP_CC)
grobj.*[ol]: $(GROBJ_CC)
grobjtcl.*[ol]: $(GROBJTCL_CC)
grshAppInit.*[ol]: $(GRSHAPPINIT_CC)
gtext.*[ol]: $(GTEXT_CC)
gtexttcl.*[ol]: $(GTEXTTCL_CC)
house.*[ol]: $(HOUSE_CC)
housetcl.*[ol]: $(HOUSETCL_CC)
id.*[ol]: $(ID_CC)
io.*[ol]: $(IO_CC)
iofactory.*[ol]: $(IOFACTORY_CC)
iomgr.*[ol]: $(IOMGR_CC)
iostl.*[ol]: $(IOSTL_CC)
ioutils.*[ol]: $(IOUTILS_CC)
jitter.*[ol]: $(JITTER_CC)
jittertcl.*[ol]: $(JITTERTCL_CC)
kbdresponsehdlr.*[ol]: $(KBDRESPONSEHDLR_CC)
maskhatch.*[ol]: $(MASKHATCH_CC)
masktcl.*[ol]: $(MASKTCL_CC)
misctcl.*[ol]: $(MISCTCL_CC)
morphyface.*[ol]: $(MORPHYFACE_CC)
morphyfacetcl.*[ol]: $(MORPHYFACETCL_CC)
nullresponsehdlr.*[ol]: $(NULLRESPONSEHDLR_CC)
objlist.*[ol]: objlist.h ptrlist.h io.h iomgr.h grobj.h id.h \
	observable.h observer.h trace.h debug.h ptrlist.cc objlist.cc
objlisttcl.*[ol]: $(OBJLISTTCL_CC)
objtogl.*[ol]: objtogl.h gfxattribs.h tlist.h io.h \
	$(TCLEVALCMD_H) $(TCLCMD_H) $(TCLITEMPKG_H) \
	toglconfig.h rect.h trace.h debug.h objtogl.cc
observable.*[ol]: observable.h observer.h trace.h debug.h observable.cc
observer.*[ol]: observer.h observer.cc
pbm.*[ol]: $(PBM_CC)
position.*[ol]: position.h io.h trace.h debug.h position.cc
positiontcl.*[ol]: $(POSITIONTCL_CC)
poslist.*[ol]: poslist.h ptrlist.h id.h io.h iomgr.h position.h \
	debug.h trace.h ptrlist.cc poslist.cc
poslisttcl.*[ol]: $(POSLISTTCL_CC)
property.*[ol]: $(PROPERTY_CC)
responsehandler.*[ol]: $(RESPONSEHANDLER_CC)
rhlist.*[ol]: debug.h io.h ptrlist.h ptrlist.cc responsehandler.h \
	rhlist.h rhlist.cc trace.h
rhtcl.*[ol]: $(RHTCL_CC)
sound.*[ol]: $(SOUND_CC)
soundlist.*[ol]: $(SOUNDLIST_CC)
soundtcl.o: $(SOUNDTCL_CC)
	$(CC) $(PROD_OPTIM) -o $*.o $(INCLUDE_DIRS) -I/opt/audio/include -c $*.cc
soundtcl.do: $(SOUNDTCL_CC)
	$(CC) -c +Z $(TEST_OPTIM) -o $*.do $(INCLUDE_DIRS) -I/opt/audio/include \
		$(TEST_FLAGS) $*.cc
soundtcl.sl: soundtcl.do
stringifycmd.*[ol]: $(STRINGIFYCMD_CC)
subject.*[ol]: subject.h io.h ioutils.h trace.h debug.h subject.cc
subjecttcl.*[ol]: errmsg.h subject.h io.h trace.h debug.h \
	subjecttcl.cc
tcldlist.*[ol]: tcldlist.h errmsg.h trace.h debug.h tcldlist.cc
tclcmd.*[ol]: $(TCLCMD_CC)
tclerror.*[ol]: $(TCLERROR_CC) 
tclgl.*[ol]: $(TCLGL_CC)
tclitempkg.*[ol]: $(TCLITEMPKG_CC)
tclpkg.*[ol]: $(TCLPKG_CC)
tclvalue.*[ol]: $(TCLVALUE_CC)
thlist.*[ol]: debug.h io.h ptrlist.h ptrlist.cc \
	timinghdlr.h thlist.h thlist.cc trace.h
thtcl.*[ol]: $(THTCL_CC)
timinghdlr.*[ol]: $(TIMINGHDLR_CC)
timinghandler.*[ol]: timinghandler.h io.h exptdriver.h $(TRIALEVENT_H) \
	trace.h debug.h timinghandler.cc
tlist.*[ol]: $(TLIST_CC)
tlisttcl.*[ol]: $(TLISTTCL_CC)
toglconfig.*[ol]: toglconfig.h rect.h gfxattribs.h $(TCLEVALCMD_H) \
	$(ERROR_H) $(TRACE_H) $(DEBUG_H) toglconfig.cc
trace.*[ol]: trace.h trace.cc
trial.*[ol]: trial.h io.h objlist.h ptrlist.h grobj.h id.h \
	observable.h observer.h poslist.h position.h trace.h debug.h trial.cc
trialevent.*[ol]: $(TRIALEVENT_CC)
trialtcl.*[ol]: $(TRIALTCL_CC)
value.*[ol]: $(VALUE_CC)
xbitmap.*[ol]: $(XBITMAP_CC)

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
