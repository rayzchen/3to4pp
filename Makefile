#
# Created by gmakemake (Ubuntu Jun  6 2024) on Thu Jun 06 12:43:47 2024
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) -o $@ $<
.C.o:
		$(COMPILE.cc) -o $@ $<
.cpp.o:
		$(COMPILE.cc) -o $@ $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) -o $@ $<
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
CXXFLAGS = --std=c++11 -Iimgui/ -Iimgui/backends/
ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -Llib -lglfw -lGL
endif

ifeq ($(MAKECMDGOALS),build)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
		CXXFLAGS += -mwindows
	endif
endif

ifeq ($(MAKECMDGOALS),shared)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -lglfw3dll
		CXXFLAGS += -mwindows -DGLFW_DLL -DDLL_IMPORT
		CCLIBFLAGS += lib/*.dll
	endif
	CCLIBFLAGS := -Wl,-Bdynamic $(CCLIBFLAGS)
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
control.o:	constants.h control.h pieces.h puzzle.h render.h
gui.o:	control.h gui.h pieces.h puzzle.h render.h
pieces.o:	pieces.h
puzzle.o:	puzzle.h
render.o:	constants.h control.h pieces.h puzzle.h render.h
shaders.o:	shaders.h
window.o:	camera.h constants.h control.h gui.h pieces.h puzzle.h render.h shaders.h window.h
gl.o:	

########## Targets from targets.mak

.PHONY: all run addicon build shared clean realclean

IMGUI_OBJFILES = imgui/imgui.o \
				imgui/imgui_draw.o \
				imgui/imgui_demo.o \
				imgui/imgui_tables.o \
				imgui/imgui_widgets.o \
				imgui/backends/imgui_impl_glfw.o \
				imgui/backends/imgui_impl_opengl3.o

ifeq ($(MAKECMDGOALS),shared)
ifeq ($(OS),Windows_NT)
LIBIMGUI = lib/imgui.dll
else
LIBIMGUI = lib/libimgui.so
endif

$(IMGUI_OBJFILES): CXXFLAGS += -fPIC
$(LIBIMGUI): $(IMGUI_OBJFILES)
	rm -f $@
ifeq ($(OS),Windows_NT)
	$(LINK.cc) -shared -o $@ $^ lib/*.dll
endif
clean: OBJFILES += $(IMGUI_OBJFILES)

CCLIBFLAGS += -Wl,-rpath=\$$ORIGIN -limgui
else
OBJFILES += $(IMGUI_OBJFILES)
endif

ifeq ($(OS),Windows_NT)
resources.o: resources.rc icons/icons.ico
	windres resources.rc -o resources.o
OBJFILES += resources.o
endif

3to4++:	3to4++.o $(LIBIMGUI) $(OBJFILES)

run:	3to4++
	./3to4++

build:	clean all
	rm -rf 3to4pp
	mkdir 3to4pp
	cp 3to4++ 3to4pp
	cp LICENSE 3to4pp

shared: build
ifeq ($(OS),Windows_NT)
	ldd 3to4++ | grep -v "WINDOWS" | sed -e 's/\t.*\.dll => \| \(.*\)\|not found//g' | xargs -I {} cp {} 3to4pp
	cp lib/*.dll 3to4pp
else
	cp lib/*.so 3to4pp
endif

release:
	rm -rf dist
	make build
	7z a dist/3to4++.zip 3to4pp/
	make shared
	7z a dist/3to4++dll.zip 3to4pp/

########## End of targets from targets.mak

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
