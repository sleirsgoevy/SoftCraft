main: *.cpp *.h
	clang++ *.cpp --std=c++14 -O3 -o main -lSDL2 -pthread

life3d.o:
	clang++ life3d.cpp --std=c++14 -O3 -c -o life3d.o

debug: *.cpp *.h life3d.o
	bash -c "clang++ *[^life3d].cpp life3d.o --std=c++14 -g -o main -lSDL2 -pthread"
