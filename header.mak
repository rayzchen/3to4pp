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
