main: src/main.cpp src/json.h
	g++ -Wall -std=c++17 -o main src/main.cpp src/json.h

clean:
	rm -rf main