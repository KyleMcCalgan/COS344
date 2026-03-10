#add all the files that you need to the files makefile variable below as a space seperated list
files = 

main: main.cpp shader.cpp
	g++ -std=c++11 -g shader.cpp $(files) main.cpp -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run