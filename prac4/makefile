files = shader.cpp Geometry.cpp Mesh.cpp src/App.cpp src/Input.cpp src/Scene.cpp src/HUD.cpp src/Textures.cpp
headers = shader.hpp Geometry.h Mesh.h math3d.hpp src/App.h src/Input.hpp src/Scene.hpp src/HUD.hpp src/linefont.h src/Textures.hpp src/Palette.hpp
main: main.cpp $(files) $(headers) vertex.glsl fragment.glsl HUDVertex.glsl HUDFragment.glsl
	g++ -std=c++11 -g -I. -Isrc $(files) main.cpp -lglfw -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run: main
	./main

all: clean main run
