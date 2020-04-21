LZ4=/home/user/lz4

FILES=testlz4 testlz4_AFL testlz4_LF testlz4_swarm testlz4_symex symexwins symexwins_AFL

all: $(FILES)

clean:
	rm -rf $(FILES)
	
testlz4: fuzzingwins.cpp
	$(CXX) -o testlz4 fuzzingwins.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a  -ldeepstate -lpthread


testlz4_AFL: fuzzingwins.cpp
	afl-clang++ -o testlz4_AFL fuzzingwins.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread

testlz4_LF: fuzzingwins.cpp
	clang++ -o testlz4_LF fuzzingwins.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate_AFL -lpthread -fsanitize=fuzzer

testlz4_swarm: fuzzingwins.cpp
	$(CXX) -o testlz4_swarm fuzzingwins.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate -lpthread -DDEEPSTATE_PURE_SWARM

testlz4_symex: fuzzingwins.cpp
	clang++ -o testlz4_symex fuzzingwins.cpp -I$(LZ4)/lib $(LZ4)/lib/lz4.a -ldeepstate

symexwins: symexwins.cpp
	clang++ -o symexwins symexwins.cpp -ldeepstate

symexwins_AFL: symexwins.cpp
	afl-clang++ -o symexwins_AFL symexwins.cpp -ldeepstate_AFL -lpthread	