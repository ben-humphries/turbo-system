make:
	g++ -g src/* -Iinclude/ \
		-o bin/turbo-system \
		-lSDL2 -lGL -lGLEW
