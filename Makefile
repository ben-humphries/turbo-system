make:
	g++ -g src/* imgui/*.cpp -Iinclude/ -Iimgui/ \
		-o bin/turbo-system \
		-lSDL2 -lGL -lGLEW
