# Assignment 3

Here is the introduction for fuzzingwins.cpp.

I choose a file compression library [lz4](https://github.com/lz4/lz4) to take a round-trip test, which is compression + decompression.

First, in `/home/user` clone the repo.

Then, go to the path  where the header file is.

Last, create static library.

```bash
cd /home/user
git clone https://github.com/lz4/lz4.git
cd lz4/lib
ar -rc lz4.a lz4.o
```

The test harness and Makefile will allow you to test [lz4](https://github.com/lz4/lz4) using

* DeepState's built-in force fuzzer
* AFL
* libFuzzer
* Eclipser
* Swarm Testing

Reference: https://github.com/lz4/lz4

