#
# Created by gmakemake (Ubuntu May 17 2024) on Fri May 17 16:22:15 2024
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

ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -lglfw -lGL
endif
CPPFLAGS = -DGLEW_NO_GLU -Iinclude

########## End of flags from header.mak


CPP_FILES =	3to4.cpp camera.cpp pieces.cpp render.cpp shaders.cpp window.cpp
C_FILES =	glad.c
PS_FILES =	
S_FILES =	
H_FILES =	camera.h pieces.h render.h shaders.h window.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	camera.o pieces.o render.o shaders.o window.o glad.o 

#
# Main targets
#

all:	3to4 

3to4:	3to4.o $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o 3to4 3to4.o $(OBJFILES) $(CCLIBFLAGS)

#
# Dependencies
#

3to4.o:	camera.h render.h window.h
camera.o:	camera.h
pieces.o:	pieces.h
render.o:	render.h
shaders.o:	shaders.h
window.o:	camera.h pieces.h render.h shaders.h window.h
glad.o:	

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) 3to4.o core

realclean:        clean
	-/bin/rm -f 3to4 
