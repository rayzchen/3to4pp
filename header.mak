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

# Set `all` to be the default target
.PHONY: all run addicon build shared clean realclean
all: 3to4++ addicon

run:	3to4++
	./3to4++
ifeq ($(OS),Windows_NT)
resources.o: resources.rc icons/icons.ico
	windres resources.rc -o resources.o
addicon: resources.o
	$(CXX) $(CXXFLAGS) -o 3to4++ 3to4++.o resources.o $(OBJFILES) $(CCLIBFLAGS)
endif

ifeq ($(OS),Windows_NT)
build:	clean all addicon
else
build:	clean all
endif
	rm -rf 3to4pp
	mkdir 3to4pp
	cp 3to4++ 3to4pp
	cp LICENSE 3to4pp

shared: build
ifeq ($(OS),Windows_NT)
	ldd 3to4++ | grep -v "WINDOWS" | sed -e 's/\t.*\.dll => \| \(.*\)\|not found//g' | xargs -I {} cp {} 3to4pp
	cp lib/*.dll 3to4pp
endif

release:
	rm -rf dist
	make build
	7z a dist/3to4++.zip 3to4pp/
	make shared
	7z a dist/3to4++dll.zip 3to4pp/
