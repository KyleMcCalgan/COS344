files = shader.cpp Geometry.cpp Mesh.cpp

main: main.cpp
	g++ -std=c++11 -g $(files) main.cpp -lglfw -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run: main
	./main

all: clean main run
