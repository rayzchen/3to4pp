#
# Created by gmakemake (Ubuntu May 28 2024) on Thu May 30 11:26:27 2024
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

CPPFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -Iinclude -pedantic
CXXFLAGS = --std=c++11
ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -Llib -lfreetype -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -Llib -lfreetype -lglfw -lGL
	CPPFLAGS += -I/usr/include/freetype2/
endif

ifeq ($(MAKECMDGOALS),build)
	CXXFLAGS += -s -Ofast -DNDEBUG
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
		CXXFLAGS += -mwindows
	endif
endif

ifeq ($(MAKECMDGOALS),shared)
	CXXFLAGS += -s -Ofast -DNDEBUG
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -lglfw3dll
		CXXFLAGS += -mwindows -DGLFW_DLL -DDLL_IMPORT
		CCLIBFLAGS += lib/*.dll
	endif
	CCLIBFLAGS := -Wl,-Bdynamic $(CCLIBFLAGS)
endif

all:	3to4++
build:	clean all
shared: build
	rm -rf dist
	mkdir dist
	cp 3to4++ dist
	cp LICENSE dist
ifeq ($(OS),Windows_NT)
	ldd 3to4++ | grep -v "WINDOWS" | sed -e 's/\t.*\.dll => \| \(.*\)\|not found//g' | xargs -I {} cp {} dist
	cp lib/*.dll dist
endif

########## End of flags from header.mak


CPP_FILES =	3to4++.cpp camera.cpp control.cpp gui.cpp pieces.cpp puzzle.cpp render.cpp shaders.cpp window.cpp
C_FILES =	gl.c
PS_FILES =	
S_FILES =	
H_FILES =	camera.h constants.h control.h gui.h pieces.h puzzle.h render.h shaders.h window.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	camera.o control.o gui.o pieces.o puzzle.o render.o shaders.o window.o gl.o 

#
# Main targets
#

all:	3to4++ 

3to4++:	3to4++.o $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o 3to4++ 3to4++.o $(OBJFILES) $(CCLIBFLAGS)

#
# Dependencies
#

3to4++.o:	camera.h control.h gui.h pieces.h puzzle.h render.h window.h
camera.o:	camera.h constants.h
control.o:	control.h pieces.h puzzle.h render.h
gui.o:	gui.h pieces.h puzzle.h render.h
pieces.o:	pieces.h
puzzle.o:	puzzle.h
render.o:	constants.h pieces.h puzzle.h render.h
shaders.o:	shaders.h
window.o:	camera.h constants.h control.h gui.h pieces.h puzzle.h render.h shaders.h window.h
gl.o:	

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) 3to4++.o core

realclean:        clean
	-/bin/rm -f 3to4++ 
