all: *.c *.h
	clang -o cfunction afl_cfunction.c cfunction.c
	afl-clang -o afl_cfunction afl_cfunction.c cfunction.c
	clang++ -o libfuzzer_cfunction  libfuzzer_cfunction.cpp cfunction.c -fsanitize=address,fuzzer
