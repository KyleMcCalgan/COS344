#add all the files that you need to the files makefile variable below as a space seperated list
files = src/Input.cpp src/Mesh.cpp src/Scene.cpp

main: main.cpp
	g++ -g shader.cpp $(files) main.cpp -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run