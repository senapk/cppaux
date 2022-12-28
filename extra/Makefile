all: tests exec clear

tests:
	g++ -std=c++17 -Wall -Werror -Wextra fn_tests.cpp -o fn_tests.out
	g++ mod1.cpp mod2.cpp -o mod.out

exec:
	./fn_tests.out
	./mod.out

clear:
	rm -f *.out