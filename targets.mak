.PHONY: all run addicon build shared clean realclean

IMGUI_SOURCEFILES = imgui/imgui.cpp \
					imgui/imgui_draw.cpp \
					imgui/imgui_demo.cpp \
					imgui/imgui_tables.cpp \
					imgui/imgui_widgets.cpp \
					imgui/backends/imgui_impl_glfw.cpp \
					imgui/backends/imgui_impl_opengl3.cpp
IMGUI_OBJFILES = $(IMGUI_SOURCEFILES:.cpp=.o)

NFD_SOURCEFILES = nfd/src/nfd_common.c
NFD_OBJFILES = nfd/src/nfd_common.o
ifeq ($(OS),Windows_NT)
NFD_SOURCEFILES += nfd/src/nfd_win.cpp
NFD_OBJFILES += nfd/src/nfd_win.o
else
NFD_SOURCEFILES += nfd/src/nfd_zenity.c
NFD_OBJFILES += nfd/src/nfd_zenity.o
endif

ifneq ($(CXX),cccl)
ifeq ($(MAKECMDGOALS),shared)
ifeq ($(OS),Windows_NT)
LIBIMGUI = lib/imgui.dll
LIBNFD = lib/nfd.dll
else
LIBIMGUI = lib/libimgui.so
LIBNFD = lib/libnfd.so
endif

$(IMGUI_OBJFILES): CXXFLAGS += -fPIC
$(LIBIMGUI): $(IMGUI_OBJFILES)
	rm -f $@
ifeq ($(OS),Windows_NT)
	$(LINK.cc) -shared -o $@ $^ lib/glfw3.dll
else
	$(LINK.cc) -shared -o $@ $^ -lglfw
endif

$(NFD_OBJFILES): CXXFLAGS += -fPIC
$(LIBNFD): $(NFD_OBJFILES)
	rm -f $@
	$(LINK.cc) -shared -o $@ $^

clean: OBJFILES += $(IMGUI_OBJFILES) $(NFD_OBJFILES)

CCLIBFLAGS += -Wl,-rpath=\$$ORIGIN -limgui -lnfd
else
OBJFILES += $(IMGUI_OBJFILES) $(NFD_OBJFILES)
endif
else
OBJFILES += $(IMGUI_OBJFILES) $(NFD_OBJFILES)
endif

ifeq ($(OS),Windows_NT)
resources.o: resources.rc icons/icons.ico
	windres resources.rc -o resources.o
OBJFILES += resources.o
endif

3to4++:	3to4++.o $(LIBIMGUI) $(LIBNFD) $(OBJFILES)

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
