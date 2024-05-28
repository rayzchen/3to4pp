ifeq ($(OS),Windows_NT)
	CCLIBFLAGS = -lglfw3 -lopengl32 -lgdi32
else
	CCLIBFLAGS = -lglfw -lGL
endif
CPPFLAGS = -Wall -Werror -Iinclude -pedantic
CXXFLAGS = --std=c++11
