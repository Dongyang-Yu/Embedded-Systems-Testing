LZ4=/home/user/lz4

FILES=testlz4 testlz4_AFL testlz4_LF testlz4_swarm

all: $(FILES)

clean:
	rm -rf $(FILES)
	
testlz4: TestLz4.cpp
	$(CXX) -o testlz4 TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a  -ldeepstate -lpthread


testlz4_AFL: TestLz4.cpp
	afl-clang++ -o testlz4_AFL TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread

testlz4_LF: TestLz4.cpp
	clang++ -o testlz4_LF TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread -fsanitize=fuzzer

testlz4_swarm: TestLz4.cpp
	$(CXX) -o testlz4_swarm TestLz4.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM

