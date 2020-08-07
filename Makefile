CC = g++
CFLAGS = -I./include -I./glm-0.9.7.1
LFLAGS = -L./lib/mac -lfreeimage
DEPS = geometry.hpp

pathtracer: main.o geometry.o material.o
	$(CC) -o pathtracer main.o geometry.o material.o $(CFLAGS) $(LFLAGS)

main.o: main.cpp geometry.hpp material.hpp
	$(CC) -c -o main.o main.cpp $(CFLAGS)

geometry.o: geometry.cpp geometry.hpp material.hpp
	$(CC) -c -o geometry.o geometry.cpp $(CFLAGS)

material.o: material.cpp material.hpp
	$(CC) -c -o material.o material.cpp $(CFLAGS)