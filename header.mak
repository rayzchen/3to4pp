CPPFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -pedantic -Iinclude -Iimgui/ -Iimgui/backends/
CXXFLAGS = --std=c++11
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

ifeq ($(MAKECMDGOALS),emscripten)
	CPPFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	CPPFLAGS += -Wno-dollar-in-identifier-extension -x c++ -lglfw3
endif
