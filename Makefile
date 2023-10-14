windows:
	g++ -std=c++14 -I src/include -L src/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2
linux:
	g++ -o main main.cpp -lSDL2