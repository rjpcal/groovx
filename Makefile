#$Id$
TARGET = grsh.a1.1
CC = aCC
FLAGS = -DTRACE
#+O3 +Onolimit 
OPTIM = 

INCLUDEFLAGS = -I/cit/rjpeters/include/ \
	-I/opt/graphics/OpenGL/include/
LIBRARIES = -L/opt/graphics/OpenGL/lib \
	-L/cit/rjpeters/lib/ \
	-ltogl -lGLU -lGL \
	-ltk -ltcl -lXmu \
	-lX11 -lXext \
	-lm

OBJECTS = 	\
	face.o facetcl.o \
	gfxattribs.o \
	grobj.o \
	grobjmgr.o \
	io.o \
	iostl.o \
	ioutils.o \
	misctcl.o \
	objlist.o objlisttcl.o \
	objtogl.o \
	position.o positiontcl.o \
	poslist.o poslisttcl.o \
	tcldlist.o \
	tclgl.o \
	tlist.o tlisttcl.o \
	toglconfig.o \
	trace.o \
	trial.o \
	trialexpt.o

ALL_OBJECTS = $(OBJECTS) \
	subjecttcl.o subject.o \
	fixpt.o fixpttcl.o \

.SUFFIXES:
.SUFFIXES: .cc .o

.cc.o:; $(CC) $(FLAGS) $(OPTIM) $(INCLUDEFLAGS) -c $*.cc

$(TARGET): tags $(ALL_OBJECTS) grshAppInit.o
	$(CC) $(FLAGS) $(OPTIM) -o $(HOME)/bin/$(ARCH)/$@ \
	$(ALL_OBJECTS) grshAppInit.o $(LIBRARIES)

#################
# object files #
################

face.o: io.h ioutils.h gfxattribs.h grobj.h face.h face.cc
facetcl.o: objlist.h objlisttcl.h face.h facetcl.cc
fixpt.o: io.h ioutils.h grobj.h fixpt.h fixpt.cc
fixpttcl.o: fixpt.h objlist.h objlisttcl.h fixpttcl.cc
gfxattribs.o: gfxattribs.h gfxattribs.cc
grobj.o: io.h ioutils.h grobj.h grobj.cc
grobjmgr.o: grobjmgr.h grobjmgr.cc face.h fixpt.h grobj.h io.h
grshAppInit.o: grshAppInit.cc
io.o: io.h io.cc
iostl.o: io.h iostl.h iostl.cc
ioutils.o: io.h ioutils.h ioutils.cc
misctcl.o: misctcl.h misctcl.cc
objlist.o: io.h grobj.h objlist.h scaleable.h objlist.cc grobjmgr.h \
	ioutils.h randutils.h
objlisttcl.o: io.h objlist.h grobj.h objlisttcl.h objlisttcl.cc
objtogl.o: tlist.h tlisttcl.h toglconfig.h objtogl.h objtogl.cc io.h rect.h \
	gfxattribs.h
position.o: io.h position.h position.cc
positiontcl.o: position.h poslist.h poslisttcl.h positiontcl.h positiontcl.cc
poslist.o: io.h position.h poslist.h poslist.cc
poslisttcl.o: position.h poslist.h poslisttcl.h poslisttcl.cc
tlist.o: trial.h tlist.h tlist.cc objlist.h poslist.h
tlisttcl.o: tlist.h objlisttcl.h poslisttcl.h tlisttcl.h tlisttcl.cc
toglconfig.o: rect.h toglconfig.h toglconfig.cc
trace.o: trace.cc
trial.o: trial.h trial.cc objlist.h poslist.h grobj.h position.h
trialexpt.o: trialexpt.h trialexpt.cc trial.h tlist.h

# old files #

#expttcl.o: io.h objlisttcl.h glisttcl.h glist.h expt.h realexpt.h \
#	expttcl.h expttcl.cc
#glist.o: io.h ioutils.h objlist.h glist.h glist.cc
#glisttcl.o: glist.h objlisttcl.h glisttcl.h glisttcl.cc
#grobjtcl.o: grobjtcl.h grobjtcl.cc objlist.h objlisttcl.h
#realexpt.o: expt.h glist.h realexpt.h realexpt.cc face.h io.h ioutils.h \
#	face.h scaleable.h grobj.h objlist.h randutils.h
#stimdg.o: stimdg.h stimdg.cc tcldlist.h
#stimdgtcl.o: face.h glist.h glisttcl.h objlist.h objlisttcl.h \
#	stimdg.h stimdgtcl.h stimdgtcl.cc
#scaleable.o: io.h ioutils.h scaleable.h scaleable.cc
#scaleabletcl.o: io.h grobj.h objlisttcl.h scaleable.h \
#	scaleabletcl.h scaleabletcl.cc
#subject.o: io.h ioutils.h subject.h subject.cc
#subjecttcl.o: subject.h subjecttcl.cc
#tclgl.o: tclgl.h tclgl.cc
#tcldlist.o: tcldlist.h tcldlist.cc 
#timer.o: io.h grobj.h timer.h timer.cc
#timertcl.o: timertcl.h timertcl.cc grobjtcl.h grobjtcl.cc timer.h objlist.h \
#	objlisttcl.h


test:	test.cc
	$(CC) $(FLAGS) $(INCLUDEFLAGS) test.cc \
	$(ALL_OBJECTS) $(LIBRARIES)


share:
	$(CC) -v +Z $(FLAGS) $(OPTIM) $(INCLUDEFLAGS) -c face.cc
	$(CC) -v -b tclgl.o -o face.sl $(LIBRARIES)

new: cleaner devsh

clean:
	rm -f *~ \#* core
cleaner: clean
	rm -f *.o
tags:
	etags *\.[ch]*
edit: clean
	emacs *\.[ch]* Makefile
count:
	wc -l *\.[ch]*
