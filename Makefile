TARGET = grsh.a0.1
CC = aCC
FLAGS = +O3
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
	expttcl.o \
	face.o facetcl.o \
	gfxattribs.o \
	glist.o glisttcl.o \
	grobj.o \
	grobjmgr.o \
	io.o \
	ioutils.o \
	misctcl.o \
	objlist.o objlisttcl.o \
	objtogl.o \
	realexpt.o \
	scaleable.o scaleabletcl.o \
	tcldlist.o \
	tclgl.o \
	toglconfig.o \
	trace.o

ALL_OBJECTS = $(OBJECTS) timertcl.o timer.o \
	subjecttcl.o subject.o \
	fixpt.o fixpttcl.o \
	grobjtcl.o 

.SUFFIXES:
.SUFFIXES: .cc .o

.cc.o:; $(CC) $(FLAGS) $(OPTIM) $(INCLUDEFLAGS) -c $*.cc

$(TARGET): tags $(ALL_OBJECTS) grshAppInit.o
	$(CC) $(FLAGS) $(OPTIM) -o $(HOME)/bin/$(ARCH)/$@ \
	$(ALL_OBJECTS) grshAppInit.o $(LIBRARIES)

################
# object files #
################

expttcl.o: io.h objlisttcl.h glisttcl.h glist.h expt.h realexpt.h \
	expttcl.h expttcl.cc
face.o: io.h ioutils.h gfxattribs.h grobj.h scaleable.h face.h face.cc
facetcl.o: objlist.h objlisttcl.h face.h facetcl.cc
fixpt.o: io.h ioutils.h grobj.h scaleable.h fixpt.h fixpt.cc
fixpttcl.o: fixpt.h objlist.h objlisttcl.h fixpttcl.cc
gfxattribs.o: gfxattribs.h gfxattribs.cc
glist.o: io.h ioutils.h objlist.h glist.h glist.cc
glisttcl.o: glist.h objlisttcl.h glisttcl.h glisttcl.cc
grobj.o: io.h ioutils.h grobj.h grobj.cc
grobjmgr.o: grobjmgr.h grobjmgr.cc face.h fixpt.h scaleable.h grobj.h io.h
grobjtcl.o: grobjtcl.h grobjtcl.cc objlist.h objlisttcl.h
grshAppInit.o: grshAppInit.cc
io.o: io.h io.cc
ioutils.o: io.h ioutils.h ioutils.cc
misctcl.o: misctcl.h misctcl.cc
objlist.o: io.h grobj.h objlist.h scaleable.h objlist.cc grobjmgr.h \
	ioutils.h randutils.h
objlisttcl.o: io.h objlist.h grobj.h objlisttcl.h objlisttcl.cc
objtogl.o: glist.h glisttcl.h toglconfig.h objtogl.h objtogl.cc io.h rect.h \
	gfxattribs.h
realexpt.o: expt.h glist.h realexpt.h realexpt.cc face.h io.h ioutils.h \
	face.h scaleable.h grobj.h objlist.h randutils.h
scaleable.o: io.h ioutils.h scaleable.h scaleable.cc
scaleabletcl.o: io.h grobj.h objlisttcl.h scaleable.h \
	scaleabletcl.h scaleabletcl.cc
subject.o: io.h ioutils.h subject.h subject.cc
subjecttcl.o: subject.h subjecttcl.cc
tclgl.o: tclgl.h tclgl.cc
tcldlist.o: tcldlist.h tcldlist.cc 
timer.o: io.h grobj.h timer.h timer.cc
timertcl.o: timertcl.h timertcl.cc grobjtcl.h grobjtcl.cc timer.h objlist.h \
	objlisttcl.h
toglconfig.o: rect.h toglconfig.h toglconfig.cc
trace.o: trace.cc


# old files #

#stimdg.o: stimdg.h stimdg.cc tcldlist.h
#stimdgtcl.o: face.h glist.h glisttcl.h objlist.h objlisttcl.h \
#	stimdg.h stimdgtcl.h stimdgtcl.cc


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
