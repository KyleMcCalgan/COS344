#add all the files that you need to the files makefile variable below as a space seperated list
files =

main: main.cpp shader.cpp
	g++ -std=c++11 -g shader.cpp $(files) main.cpp -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

# Export: run the program normally — press F5 in-game to write layout.json
export: main
	./main

# Import: load a layout file on startup
# Usage: make import FILE=mylayout.json
import: main
	./main --import $(FILE)

all:
	make clean
	make
	make run