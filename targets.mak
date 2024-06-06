.PHONY: all run addicon build shared clean realclean

IMGUI_SOURCEFILES = imgui/imgui.cpp \
					imgui/imgui_draw.cpp \
					imgui/imgui_demo.cpp \
					imgui/imgui_tables.cpp \
					imgui/imgui_widgets.cpp \
					imgui/backends/imgui_impl_glfw.cpp \
					imgui/backends/imgui_impl_opengl3.cpp
IMGUI_OBJFILES = $(IMGUI_SOURCEFILES:.cpp=.o)

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

emscripten:
	rm -rf web/3to4++*
	em++ $(CPPFLAGS) $(CPP_FILES) $(C_FILES) $(IMGUI_SOURCEFILES) \
		-o web/3to4++.js -sFULL_ES3 -sMAX_WEBGL_VERSION=3
