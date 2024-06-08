#
# Created by gmakemake (Ubuntu Jun  8 2024) on Sat Jun 08 18:13:17 2024
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

CPPFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -pedantic -Iinclude -Iimgui/ -Iimgui/backends/
CXXFLAGS = --std=c++11
ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -lshell32
else
	CCLIBFLAGS = -Llib -lglfw -lGL
endif

ifeq ($(MAKECMDGOALS),build)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	ifeq ($(CXX),cccl)
		CCLIBFLAGS += /O2 /link /subsystem:windows /ENTRY:mainCRTStartup /nologo
		unexport CXXFLAGS
	else
		ifeq ($(OS),Windows_NT)
			CCLIBFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
			CXXFLAGS += -mwindows
		endif
	endif
endif

ifeq ($(MAKECMDGOALS),shared)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW -DGLFW_DLL -DDLL_IMPORT
	CCLIBFLAGS := $(filter-out -lglfw3,$(CCLIBFLAGS))
	ifeq ($(CXX),cccl)
		CCLIBFLAGS += -luser32 -lglfw3dll /O2 /link /subsystem:windows /ENTRY:mainCRTStartup /nologo
		unexport CXXFLAGS
	else
		ifeq ($(OS),Windows_NT)
			CCLIBFLAGS += -lglfw3dll lib/*.dll
			CXXFLAGS += -mwindows
		endif
		CCLIBFLAGS := -Wl,-Bdynamic $(CCLIBFLAGS)
	endif
endif

ifeq ($(MAKECMDGOALS),emscripten)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	CPPFLAGS += -Wno-dollar-in-identifier-extension -x c++ -lglfw3
endif

########## End of flags from header.mak


CPP_FILES =	3to4++.cpp camera.cpp control.cpp font.cpp gui.cpp pieces.cpp puzzle.cpp render.cpp shaders.cpp window.cpp
C_FILES =	gl.c
PS_FILES =	
S_FILES =	
H_FILES =	camera.h constants.h control.h font.h gui.h pieces.h puzzle.h render.h shaders.h window.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	camera.o control.o font.o gui.o pieces.o puzzle.o render.o shaders.o window.o gl.o 

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
font.o:	
gui.o:	control.h font.h gui.h pieces.h puzzle.h render.h
pieces.o:	pieces.h
puzzle.o:	puzzle.h
render.o:	constants.h control.h pieces.h puzzle.h render.h
shaders.o:	shaders.h
window.o:	camera.h constants.h control.h gui.h pieces.h puzzle.h render.h shaders.h window.h
gl.o:	

########## Targets from targets.mak

.PHONY: all run addicon build shared clean realclean

IMGUI_SOURCEFILES = imgui/imgui.cpp \
					imgui/imgui_draw.cpp \
					imgui/imgui_demo.cpp \
					imgui/imgui_tables.cpp \
					imgui/imgui_widgets.cpp \
					imgui/backends/imgui_impl_glfw.cpp \
					imgui/backends/imgui_impl_opengl3.cpp
IMGUI_OBJFILES = $(IMGUI_SOURCEFILES:.cpp=.o)

ifneq ($(CXX),cccl)
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
	$(LINK.cc) -shared -o $@ $^ lib/glfw3.dll
endif
clean: OBJFILES += $(IMGUI_OBJFILES)

CCLIBFLAGS += -Wl,-rpath=\$$ORIGIN -limgui
else
OBJFILES += $(IMGUI_OBJFILES)
endif
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

emscripten:
	rm -rf web/3to4++*
	em++ $(CPPFLAGS) $(CPP_FILES) $(C_FILES) $(IMGUI_SOURCEFILES) \
		-o web/3to4++.js -sMAX_WEBGL_VERSION=3 -sFILESYSTEM=0 \
		-flto --closure 1 -sENVIRONMENT=web

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
