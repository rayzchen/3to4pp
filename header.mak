ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -lglfw -lGL
endif
CPPFLAGS = -DGLEW_NO_GLU -Iinclude
