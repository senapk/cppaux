all: tests
	./tests.out

tests: tests.cpp
	g++ -std=c++17 -Wall -Werror -Wextra tests.cpp -o tests.out


