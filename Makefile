CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -g

TARGET = app
SRC = src/main.cpp src/render.c src/window.c src/glad/glad.c src/mesh.c  # Add more source files here.

INCLUDE = -IC:/msys64/ucrt64/include
LIBDIR  = -LC:/msys64/ucrt64/lib

LIBS = -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

all: run

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(INCLUDE) $(LIBDIR) $(LIBS)
	chmod 777 app.exe

run: $(TARGET)
	.\$(TARGET).exe

clean:
	rm -f $(TARGET).exe