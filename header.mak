CPPFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -Iinclude -pedantic
CXXFLAGS = --std=c++11 -Iimgui/ -Iimgui/backends/
ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -Llib -lglfw -lGL
endif

ifeq ($(MAKECMDGOALS),build)
	CXXFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
		CXXFLAGS += -mwindows
	endif
endif

ifeq ($(MAKECMDGOALS),shared)
	CXXFLAGS += -s -Ofast -DNDEBUG -DNO_DEMO_WINDOW
	ifeq ($(OS),Windows_NT)
		CCLIBFLAGS += -lglfw3dll
		CXXFLAGS += -mwindows -DGLFW_DLL -DDLL_IMPORT
		CCLIBFLAGS += lib/*.dll
	endif
	CCLIBFLAGS := -Wl,-Bdynamic $(CCLIBFLAGS)
endif
