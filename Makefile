.PHONY: imgui

make:
	g++ src/* imgui.a -Iinclude/ -Iimgui/ \
		-o bin/turbo-system \
		-lSDL2 -lGL -lGLEW

imgui:
	g++ -c -DIMGUI_IMPL_OPENGL_LOADER_GLEW imgui/*.cpp
	ar rvs imgui.a *.o
	rm *.o
