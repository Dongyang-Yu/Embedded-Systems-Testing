LZ4=/home/user/lz4

FILES=testlz4 AFL_testlz4 LF_testlz4

all: $(FILES)

clean:
	rm -rf $(FILES)
	
testlz4: TestLz4.cpp
	$(CXX) -o testlz4 TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a  -ldeepstate -lpthread


AFL_testlz4: TestLz4.cpp
	afl-clang++ -o AFL_testlz4 TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread

LF_testlz4: TestLz4.cpp
	clang++ -o LF_testlz4 TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread -fsanitize=fuzzer


