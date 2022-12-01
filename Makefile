all: tests
	./fn_tests.out

tests: fn_tests.cpp
	g++ -std=c++17 -Wall -Werror -Wextra fn_tests.cpp -o fn_tests.out


