DeepState 用法：https://github.com/trailofbits/deepstate/blob/master/docs/test_harness.md



## AFL

`deepstate-afl -i afl_in/ -o afl_out/ ./testlz4_AFL`

## Symbolic execution

`deepstate-angr ./symexwins`



## 显示crashes

`./symexwins_AFL --input_test_file afl_out/crashes/xxxxxxxxxxxx `

使用flag`--input_test_file`

clang++ -o timing timing.cpp -ldeepstate_AFL -lpthread -fsanitize=fuzzer 

# 代码覆盖率

## AFL

 deepstate-afl ./timing -i afl_in -o afl_out/ -t 60

> -t 表示运行时间