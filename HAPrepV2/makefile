CXX      = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra

SRCS = main.cpp \
       shader.cpp \
       prac1/Matrix.cpp \
       prac1/Vector.cpp \
       src/Mesh.cpp \
       src/Face.cpp \
       src/Tessellator.cpp \
       src/Editor.cpp \
       src/Renderer.cpp

LIBS = -lglfw -lGLEW -lGL -ldl -lpthread

TARGET = editor

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

all: clean $(TARGET)
