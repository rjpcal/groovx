#$Id$
TEST_TARGET = testsh
TARGET = grsh.a1.2
CC = aCC
OPTIM = +O3
#+O3 +Onolimit 
LINK_OPTIM = +O1

INCLUDEFLAGS = -I/cit/rjpeters/include/
LIBRARIES = -L/opt/graphics/OpenGL/lib \
	-L/cit/rjpeters/lib \
	-L/opt/audio/lib \
	-ltogl -lGLU -lGL \
	-ltk -ltcl -lXmu \
	-lX11 -lXext \
	-lm -lAlib

OBJECTS =	\
	expt.o \
	expttcl.o \
	face.o facetcl.o \
	gfxattribs.o \
	grobj.o \
	io.o \
	iomgr.o \
	iostl.o \
	ioutils.o \
	jitter.o jittertcl.o \
	misctcl.o \
	objlisttcl.o \
	objtogl.o \
	position.o positiontcl.o \
	poslisttcl.o \
	ptrlist.o \
	soundtcl.o \
	tlist.o tlisttcl.o \
	toglconfig.o \
	trace.o \
	trial.o

ALL_OBJECTS = $(OBJECTS) \
	tcldlist.o \
	tclgl.o \
	subjecttcl.o subject.o \
	fixpt.o fixpttcl.o \

TEST_OBJECTS = $(ALL_OBJECTS)

.SUFFIXES:
.SUFFIXES: .cc .o

.cc.o:; $(CC) $(OPTIM) $(INCLUDEFLAGS) -c $*.cc

testsh: $(HOME)/bin/$(ARCH)/$(TEST_TARGET)
	$(TEST_TARGET) ./testing/grshtest.tcl

grsh: $(HOME)/bin/$(ARCH)/$(TARGET)
	$(TARGET) ./testing/grshtest.tcl

$(HOME)/bin/$(ARCH)/$(TARGET): tags $(ALL_OBJECTS) grshAppInit.o
	$(CC) $(LINK_OPTIM) -o $@ \
	$(ALL_OBJECTS) grshAppInit.o $(LIBRARIES)

$(HOME)/bin/$(ARCH)/$(TEST_TARGET): tags $(TEST_OBJECTS) grshAppInit.o
	$(CC) $(LINK_OPTIM) -o $@ \
	$(TEST_OBJECTS) grshAppInit.o $(LIBRARIES)

################
# object files #
################

expt.o: expt.h io.h randutils.h iostl.h tlist.h trial.h \
	trace.h debug.h expt.cc
expttcl.o: expttcl.h objlisttcl.h tlisttcl.h io.h expt.h errmsg.h \
	trace.h debug.h expttcl.cc
face.o: face.h grobj.h io.h gfxattribs.h trace.h debug.h face.cc
facetcl.o: facetcl.h errmsg.h objlist.h ptrlist.h io.h grobj.h \
	objlisttcl.h face.h trace.h debug.h facetcl.cc
fixpt.o: fixpt.h grobj.h io.h fixpt.cc
fixpttcl.o: errmsg.h objlist.h ptrlist.h io.h grobj.h objlisttcl.h \
	fixpt.h trace.h debug.h fixpttcl.cc
gfxattribs.o: gfxattribs.h gfxattribs.cc
grobj.o: grobj.h io.h trace.h debug.h grobj.cc
grshAppInit.o: trace.h grshAppInit.cc
io.o: io.h trace.h io.cc
iomgr.o: iomgr.h io.h face.h grobj.h fixpt.h position.h jitter.h \
	trace.h debug.h iomgr.cc
iostl.o: io.h iostl.h iostl.cc
ioutils.o: io.h ioutils.h ioutils.cc
jitter.o: jitter.h position.h io.h randutils.h trace.h debug.h jitter.cc
jittertcl.o: jittertcl.h errmsg.h jitter.h position.h io.h ptrlist.h poslist.h \
	poslisttcl.h trace.h debug.h jittertcl.cc
misctcl.o: misctcl.h misctcl.cc randutils.h
objlisttcl.o: io.h objlist.h ptrlist.h grobj.h objlisttcl.h objlisttcl.cc
objtogl.o: objtogl.h errmsg.h gfxattribs.h tlist.h io.h tlisttcl.h \
	toglconfig.h rect.h trace.h debug.h objtogl.cc
position.o: position.h io.h trace.h debug.h position.cc
positiontcl.o: positiontcl.h position.h io.h ptrlist.h poslist.h \
	poslisttcl.h trace.h debug.h errmsg.h positiontcl.cc
poslisttcl.o: poslisttcl.h errmsg.h ptrlist.h poslist.h io.h position.h \
	trace.h debug.h poslisttcl.cc
ptrlist.o: ptrlist.h io.h iomgr.h grobj.h position.h trace.h debug.h ptrlist.cc
soundtcl.o: soundtcl.h errmsg.h trace.h debug.h soundtcl.cc
	$(CC) $(OPTIM) $(INCLUDEFLAGS) -I/opt/audio/include -c $*.cc
subject.o: subject.h io.h ioutils.h trace.h debug.h subject.cc
subjecttcl.o: subjecttcl.h errmsg.h subject.h io.h trace.h debug.h \
	subjecttcl.cc
tcldlist.o: tcldlist.h errmsg.h trace.h debug.h tcldlist.cc
tclgl.o: tclgl.h errmsg.h trace.h debug.h tclgl.cc
tlist.o: tlist.h io.h trial.h objlist.h ptrlist.h grobj.h poslist.h \
	position.h trace.h debug.h tlist.cc
tlisttcl.o: tlisttcl.h tlist.h io.h trial.h objlist.h ptrlist.h grobj.h \
	poslist.h position.h objlisttcl.h poslisttcl.h errmsg.h \
	trace.h debug.h tlisttcl.cc
toglconfig.o: toglconfig.h rect.h gfxattribs.h toglconfig.cc
trace.o: trace.cc
trial.o: trial.h io.h iostl.h objlist.h ptrlist.h grobj.h poslist.h \
	position.h trace.h debug.h trial.cc


test: test.cc
	$(CC) $(OPTIM) $(INCLUDEFLAGS) test.cc \
	$(ALL_OBJECTS) $(LIBRARIES)


share:
	$(CC) -v +Z $(OPTIM) $(INCLUDEFLAGS) -c face.cc
	$(CC) -v -b tclgl.o -o face.sl $(LIBRARIES)

new: cleaner $(TARGET)

clean:
	rm -f *~ \#* core
cleaner: clean
	rm -f *.o
tags:
	etags *\.[ch]*
edit: clean
	emacs *\.[ch]* Makefile testing/grshtest.tcl
count:
	wc -l *\.[ch]*
