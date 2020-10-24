---
Title: Fuzz testing

---

# Fuzzing (or Fuzz Testing)

* Generate a huge number of test inputs
  *  increase code coverage and make the code misbehave

什么是模糊测试呢？简单来说，就是给程序很多随机的数据作为输入，最后看看有哪些bug 或者crash造成程序崩溃。

Fuzzing is one of the most powerful and proven strategies for identifying security issues in real-world software; it is responsible for the vast majority of remote code execution and privilege escalation bugs found to date in security-critical software.
模糊化是识别现实世界软件中安全问题的最强大、最成熟的策略之一；它是迄今为止在安全关键软件中发现的绝大多数远程代码执行和权限提升错误的原因。

到目前为止的几种方法:

* Corpus distillation 语料库蒸馏
  * 该方法依靠覆盖信号从大量高质量的候选文件集中选择感兴趣的种子子集，然后用传统方法对其进行模糊化。这种方法工作得非常好，但需要这样一个<u>语料库随时可用</u>。此外，块覆盖度量只提供了对程序状态的非常简单的理解，对于指导长期的模糊化工作不太有用。
* Program flow analysis (Concolic execution)程序流分析
* Symbolic execution 符号执行
* Static analysis 静态分析
* All these methods are extremely promising in experimental settings, but tend to suffer from reliability and performance problems in practical uses 但在实际应用中往往会遇到可靠性和性能问题- and currently do not offer a viable alternative to "dumb" fuzzing techniques.

## AFL (American fuzzy lop)

AFL则是fuzzing的一个很好用的工具，全称是American Fuzzy Lop，由Google安全工程师Michał Zalewski开发的一款开源fuzzing测试工具，可以高效地对**二进制程序**进行fuzzing，挖掘可能存在的内存安全漏洞，如栈溢出、堆溢出、UAF、double free等。由于需要在相关代码处插桩，因此AFL主要用于对开源软件进行测试。当然配合QEMU等工具，也可对闭源二进制代码进行fuzzing，但执行效率会受到影响。

工作原理：

通过对源码进行重新编译时进行**插桩**（简称编译时插桩）的方式自动产生测试用例来探索二进制程序内部新的执行路径。AFL也支持直接对没有源码的二进制程序进行测试，但需要QEMU的支持。

整个算法可以总结为：

1. 将用户提供的初始测试用例加载到队列中，
2. 从队列中取出下一个输入文件，
3. 尝试将测试用例修剪到最小的尺寸，但不改变程序的测量行为，
4. 使用各种平衡的、经过充分研究的传统模糊化策略反复改变文件，
5. 如果任何生成的**突变**导致了由仪器记录的新状态转换，则将突变输出作为<u>新条目添加到队列</u>中。 
6. 继续进行2步骤



根据构建过程的具体情况，重新编译目标程序的正确方法可能会有所不同，但一种近乎通用的方法是:

```shell
$ CC=/path/to/afl/afl-gcc ./configure
$ make clean all
```

For C++ programs, you'd would also want to set 

```
CXX=/path/to/afl/afl-g++.
```

The clang wrappers (`afl-clang` and `afl-clang++`) can be used in the same way; clang users may also opt to leverage a higher-performance instrumentation mode, as described in` llvm_mode/README.llvm`.

当测试库时，您需要找到或编写<u>一个简单的程序</u>，从stdin或文件中读取数据，并将其传递给被测试的库。在这种情况下，必须将这个可执行文件与检测库的静态版本相链接，或者确保正确的。因此文件是在运行时加载的(通常是通过设置`LD_LIBRARY_PATH`)。最简单的选择是静态构建，通常可能通过:

```shell
$ CC=/path/to/afl/afl-gcc ./configure --disable-shared
```

### 安装

在Linux环境下

先下载安装包： http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz

然后解压 `$ tar -zxvf afl-latest.tgz`

进入该文件夹`$ cd afl-latest`

安装:`$ sudo make install`

测试是否成功：`$ afl-fuzz` 

### 若运行时有报错

When we do this afl-fuzz will usually complain that you should change your CPUFREQ settings to performance because the automatic frequency scaling by the Linux kernel doesn't work well with afl. You can do this by running this command as root:

```
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

Alternatively you can also just tell afl to ignore the CPUFREQ settings:

```
AFL_SKIP_CPUFREQ=1 afl-fuzz -i in -o out [path_to_tool] @@
```



### Fuzzing binary

程序执行前对程序源码进行**插桩**（instrumentation），以便在程序执行过程中实时获取程序的执行情况。AFL用遗传算法对程序的输入进行变异，用边覆盖（edge converage）来测试fuzzing的性能。

The fuzzing process itself is carried out by the afl-fuzz utility. This program requires 

1. a <u>read-only</u> directory with initial test cases, 包含初始测试用例的只读目录
2. a separate place to <u>store</u> its findings, 单独的存储其发现的地方
3. plus a path to the binary to <u>test</u>. 要测试的二进制文件的路径



For target binaries that accept input directly **from stdin**, the usual syntax is:

```shell
$ ./afl-fuzz -i testcase_dir -o findings_dir /path/to/program [...params...]
```

For programs that **take input from a file**, use '**@@**' to mark the location in the target's command line where the input file name should be placed. The fuzzer will substitute this for you:

```shell
$ ./afl-fuzz -i testcase_dir -o findings_dir /path/to/program @@
```

You can also use the `-f` option to have the mutated data written to <u>a specific file</u>. This is useful if the program expects a particular file extension or so.













#### Crash triage

For test case minimization, give `afl-tmin` a try. The tool can be operated in a very simple way:

```shell
$ ./afl-tmin -i test_case -o minimized_result -- /path/to/program [...]
```



---

We need one or several input samples. Preferably they should be small. We place them in a directory we will call *in*. Now we start *afl-fuzz*:

```
afl-fuzz -i in -o out [path_to_tool] @@
```

The `@@` is replaced by the fuzzed input files. If you skip that it will pass the fuzzed file on the standard input. When we do this afl-fuzz will usually complain that you should change your CPUFREQ settings to performance because the automatic frequency scaling by the Linux kernel doesn't work well with afl. You can do this by running this command as root:

```
echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
```

Alternatively you can also just tell afl to ignore the CPUFREQ settings:

```
AFL_SKIP_CPUFREQ=1 afl-fuzz -i in -o out [path_to_tool] @@
```

![afl-fuzz](Fuzz%20testing.assets/afl-screenshot.png)

The most interesting value is the `uniq crashes`. There you will see if you found any segfaults, most of them will likely be memory access errors.

The samples that create crashes will be collected in `out/crashes`. You will also find potential hangs in `out/hangs`, however you should check if they really hang your tool. The default timeout of afl is quite low so you'll see a lot of false positives here.



To enable the use of Address Sanitizer you need to set the environment variable AFL_USE_ASAN to 1 during compilation:

```shell
AFL_USE_ASAN=1 ./configure CC=afl-gcc CXX=afl-g++ LD=afl-gcc--disable-shared
AFL_USE_ASAN=1 make
```





















## libFuzzer

> LibFuzzer is linked with the library under test, and feeds fuzzed inputs to the library via a specific fuzzing entrypoint (aka “**target function**”); the fuzzer then tracks which areas of the code are reached, and generates **mutations** on the **corpus** of input data in order to maximize the code coverage. Start with some test corpus (may be empty) 

* Provide your own target function: 
  *  (const uint8_t *Data, size_t Size) 
* Build it with special compiler instrumentation (LLVM) 
  * Add one of the sanitizers for better results
* Run on many CPUs
  * The test corpus (语料库) will grow
  * Bugs will be reported, reproducers will be recorded

### 安装

https://github.com/Dor1s/libfuzzer-workshop

首先

```
git clone https://github.com/Dor1s/libfuzzer-workshop.git
sudo ln -s /usr/include/asm-generic /usr/include/asm
apt-get install gcc-multilib
```

然后进入 `libfuzzer-workshop/` ， 执行 `sudo sh checkout_build_install_llvm.sh` 安装好 `llvm`.

然后进入 `libfuzzer-workshop/libFuzzer/Fuzzer/` ，执行 `sudo shbuild.sh` 编译好 `libFuzzer`。

如果编译成功，会生成 `libfuzzer-workshop/libFuzzer/Fuzzer/libFuzzer.a`



### Fuzz Target

The first step in using libFuzzer on a library is to implement **a *fuzz target*** – a function that accepts an array of bytes and does something interesting with these bytes using the API under test. Like this:

```c
// fuzz_target.cc
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  DoSomethingInterestingWithMyAPI(Data, Size);
  return 0;  // Non-zero return values are reserved for future use.
}
```

注意事项：

* Fuzzing engine 将在同一过程中用<u>不同的输入</u>，多次执行模糊目标。

* 它必须允许任何类型的输入(empty, huge, malformed, etc).

* It must not exit() on any input.

* It may use threads but ideally all threads should be joined at the end of the function. 举个栗子：pthread_join() 

  pthread_join() 作用：

  * 用于等待其他线程结束：当调用 pthread_join() 时，当前线程会处于阻塞状态，直到被调用的线程结束后，当前线程才会重新开始执行。
  * 对线程的资源进行回收：如果一个线程是非分离的（默认情况下创建的线程都是非分离）并且没有对该线程使用 pthread_join() 的话，该线程结束后并<u>不会释放其内存空间</u>

* 必须尽可能具有<u>确定性</u>。不确定性(例如，不基于input byte (输入字节) 的随机决策)将使模糊化效率低下。

* It must be fast. 尽量避免立方或更大的复杂性、日志记录或过度的内存消耗.

* 通常，目标越窄越好。例如，如果您的目标可以解析多种数据格式，请将其拆分为多个目标，每种格式一个。

### Fuzzer Usage

In order to build your fuzzer binary, use the `fsanitize=fuzzer` flag during the compilation and linking. In most cases you may want to combine libFuzzer with [AddressSanitizer](http://clang.llvm.org/docs/AddressSanitizer.html) (ASAN), [UndefinedBehaviorSanitizer](http://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (UBSAN), or both. You can also build with [MemorySanitizer](http://clang.llvm.org/docs/MemorySanitizer.html) (MSAN)

举例：

```shell
clang -g -O1 -fsanitize=fuzzer                         mytarget.c # Builds the fuzz target w/o sanitizers
clang -g -O1 -fsanitize=fuzzer,address                 mytarget.c # Builds the fuzz target with ASAN
clang -g -O1 -fsanitize=fuzzer,signed-integer-overflow mytarget.c # Builds the fuzz target with a part of UBSAN
clang -g -O1 -fsanitize=fuzzer,memory                  mytarget.c # Builds the fuzz target with MSAN
```

### Features

#### [Leaks](https://llvm.org/docs/LibFuzzer.html#id39) /sanitizer

Binaries built with [AddressSanitizer](http://clang.llvm.org/docs/AddressSanitizer.html) or [LeakSanitizer](http://clang.llvm.org/docs/LeakSanitizer.html) will try to detect **memory leaks** at the process shutdown. For in-process fuzzing this is inconvenient since the fuzzer needs to report a leak with a reproducer as soon as the leaky mutation is found. However, running full leak detection after every mutation is expensive.

By default (`-detect_leaks=1`) libFuzzer will count the number of `malloc` and `free` calls when executing every mutation. If the numbers don’t match (which by itself doesn’t mean there is a leak) libFuzzer will invoke the more expensive [LeakSanitizer](http://clang.llvm.org/docs/LeakSanitizer.html) pass and if the actual leak is found, it will be reported with the reproducer and the process will exit.



### Corpus (语料库)

Coverage-guided fuzzers like libFuzzer rely on a ***corpus*** of sample inputs for the code under test. This corpus should ideally be **seeded** with a varied collection of valid and invalid inputs for the code under test; 

For example, for a graphics library the initial corpus might hold a variety of different small PNG/JPG/GIF files. The fuzzer generates random mutations (mutation) based around the sample inputs in the current corpus. If a mutation triggers execution of a previously-uncovered path in the code under test, then that mutation is saved to the corpus for future variations.



### Running

To run the fuzzer, first create a <u>Corpus directory</u> that holds the initial “**seed**” sample inputs:

```shell
mkdir CORPUS_DIR
cp /some/input/samples/* CORPUS_DIR
```

Then run the fuzzer on the corpus directory:

```shell
./my_fuzzer CORPUS_DIR  # -max_len=1000 -jobs=20 ...
```

As the fuzzer discovers <u>new</u> interesting test cases (i.e. test cases that trigger coverage of new paths through the code under test), those test cases will be <u>added</u> to the corpus directory.

By default, the fuzzing process will continue indefinitely – at least until a bug is found. Any crashes or sanitizer failures will be reported as usual, stopping the fuzzing process, and the particular input that triggered the bug will be written to disk (typically as `crash-`, `leak-`, or `timeout-`).

### Options

To run the fuzzer, pass <u>zero or more corpus directories as command line arguments.</u> The fuzzer will read test inputs from each of these corpus directories, and any new test inputs that are generated will be written back to the first corpus directory:

```shell
./fuzzer [-flag1=val1 [-flag2=val2 ...] ] [dir1 [dir2 ...] ]
```

#### The most important command line options are:

- `-help`

  Print help message (`-help=1`).

- `-seed`

  Random seed. If 0 (the default), the seed is generated.

- `-runs`

  Number of individual test runs, -1 (the default) to run indefinitely.

- `-max_len`

  <u>Maximum length of a test input</u>. If 0 (the default), libFuzzer tries to guess a good value based on the corpus (and reports it).

- `-len_control`

  Try generating small inputs first, then try larger inputs over time. Specifies the rate at which the length limit is increased (smaller == faster). Default is 100. If 0, immediately try inputs with size up to max_len.

- `-timeout`

  Timeout in <u>seconds</u>, default 1200. If an input takes longer than this timeout, the process is treated as a failure case.

- `-rss_limit_mb`

  <u>Memory usage limit</u> in Mb, default 2048. Use 0 to disable the limit. If an input requires more than this amount of RSS memory to execute, the process is treated as a failure case. The limit is checked in a separate thread every second. If running w/o ASAN/MSAN, you may use ‘ulimit -v’ instead.

- `-malloc_limit_mb`

  If non-zero, the fuzzer will exit if the target tries to allocate this number of Mb with one malloc call. If zero (default) same limit as rss_limit_mb is applied.

- `-timeout_exitcode`

  Exit code (default 77) used if libFuzzer reports a timeout.

- `-error_exitcode`

  Exit code (default 77) used if libFuzzer itself (not a sanitizer) reports a bug (leak, OOM, etc).

- `-max_total_time`

  If positive, indicates the maximum total time in seconds to run the fuzzer. If 0 (the default), run indefinitely.

- `-merge`

  If set to 1, any corpus inputs from the 2nd, 3rd etc. corpus directories that trigger new code coverage will be merged into the first corpus directory. Defaults to 0. This flag can be used to minimize a corpus.

- `-merge_control_file`

  Specify a control file used for the merge process. If a merge process gets killed it tries to leave this file in a state suitable for resuming the merge. By default a temporary file will be used.

- `-minimize_crash`

  If 1, minimizes the provided crash input. Use with -runs=N or -max_total_time=N to limit the number of attempts.

- `-reload`

  If set to 1 (the default), the corpus directory is re-read periodically to check for new inputs; this allows detection of new inputs that were discovered by other fuzzing processes.

- `-jobs`

  Number of fuzzing jobs to run to completion. Default value is 0, which runs a single fuzzing process until completion. If the value is >= 1, then this number of jobs performing fuzzing are run, in a collection of parallel separate worker processes; each such worker process has its `stdout`/`stderr` redirected to `fuzz-.log`.

- `-workers`

  Number of simultaneous worker processes to run the fuzzing jobs to completion in. If 0 (the default), `min(jobs, NumberOfCpuCores()/2)` is used.

- `-dict`

  Provide a dictionary of input keywords; see [Dictionaries](http://llvm.org/docs/LibFuzzer.html#dictionaries).

- `-use_counters`

  Use [coverage counters](http://clang.llvm.org/docs/SanitizerCoverage.html#coverage-counters) to generate approximate counts of how often code blocks are hit; defaults to 1.

- `-reduce_inputs`

  Try to reduce <u>the size of inputs</u> while preserving their full feature sets; defaults to 1.

- `-use_value_profile`

  Use [value profile](http://llvm.org/docs/LibFuzzer.html#value-profile) to guide corpus expansion; defaults to 0.

- `-only_ascii`

  If 1, generate only ASCII (`isprint``+``isspace`) inputs. Defaults to 0.

- `-artifact_prefix`

  Provide a prefix to use when saving fuzzing artifacts (crash, timeout, or slow inputs) as `$(artifact_prefix)file`. Defaults to empty.

- `-exact_artifact_path`

  Ignored if empty (the default). If non-empty, write the single artifact on failure (crash, timeout) as `$(exact_artifact_path)`. This overrides `-artifact_prefix` and will not use checksum in the file name. Do not use the same path for several parallel processes.

- `-print_pcs`

  If 1, print out newly covered PCs. Defaults to 0.

- `-print_final_stats`

  If 1, print statistics at exit. Defaults to 0.

- `-detect_leaks`

  If 1 (default) and if LeakSanitizer is enabled try to detect memory leaks during fuzzing (i.e. not only at shut down).

- `-close_fd_mask`

  Indicate output streams to close at startup. Be careful, this will remove diagnostic output from target code (e.g. messages on assert failure).0 (default): close neither `stdout` nor `stderr`1 : close `stdout`2 : close `stderr`3 : close both `stdout` and `stderr`.

For the full list of flags run the fuzzer binary with `-help=1`.

### Output

During operation the fuzzer prints information to `stderr`, for example:

```shell
INFO: Seed: 1523017872
INFO: Loaded 1 modules (16 guards): [0x744e60, 0x744ea0),
INFO: -max_len is not provided, using 64
INFO: A corpus is not provided, starting from an empty corpus
#0    READ units: 1
#1    INITED cov: 3 ft: 2 corp: 1/1b exec/s: 0 rss: 24Mb
#3811 NEW    cov: 4 ft: 3 corp: 2/2b exec/s: 0 rss: 25Mb L: 1 MS: 5 ChangeBit-ChangeByte-ChangeBit-ShuffleBytes-ChangeByte-
#3827 NEW    cov: 5 ft: 4 corp: 3/4b exec/s: 0 rss: 25Mb L: 2 MS: 1 CopyPart-
#3963 NEW    cov: 6 ft: 5 corp: 4/6b exec/s: 0 rss: 25Mb L: 2 MS: 2 ShuffleBytes-ChangeBit-
#4167 NEW    cov: 7 ft: 6 corp: 5/9b exec/s: 0 rss: 25Mb L: 3 MS: 1 InsertByte-
...
```

The early parts of the output include information about the fuzzer options and configuration, including the current random seed (in the `Seed:` line; this can be overridden with the `-seed=N` flag).

---

Further output lines have the form of an event code and statistics. The possible event codes are:

- `READ`

  The fuzzer has read in all of the provided input samples from the corpus directories. 模糊化器已经从语料库目录中读入了所有提供的输入样本。 

- `INITED`

  The fuzzer has <u>completed</u> initialization, which includes running each of the initial input samples through the code under test.

- `NEW`

  The fuzzer has **created a test input** that covers new areas of the code under test. This input will be saved to the primary corpus directory. 覆盖了测试代码的新区域。该输入将被保存到主语料库目录中。

- `REDUCE`

  The fuzzer has found a **better (smaller) input** that triggers previously discovered features (set `-reduce_inputs=0` to disable).

- `pulse`

  The fuzzer has generated 2nd inputs (generated periodically to reassure the user that the fuzzer is still working).

- `DONE`

  The fuzzer has completed operation because it has reached the specified iteration limit (`-runs`) or time limit (`-max_total_time`).

- `RELOAD`

  The fuzzer is performing a **periodic** reload of inputs from the corpus directory; this allows it to discover any inputs discovered by other fuzzer processes (see [Parallel Fuzzing](http://llvm.org/docs/LibFuzzer.html#parallel-fuzzing)).

---

#### Each output line also reports the following statistics (when non-zero):

```bash
#0    READ units: 1
#1    INITED cov: 3 ft: 2 corp: 1/1b exec/s: 0 rss: 24Mb
#3811 NEW    cov: 4 ft: 3 corp: 2/2b exec/s: 0 rss: 25Mb L: 1 MS: 5 ChangeBit-ChangeByte-ChangeBit-ShuffleBytes-ChangeByte-
#3827 NEW    cov: 5 ft: 4 corp: 3/4b exec/s: 0 rss: 25Mb L: 2 MS: 1 CopyPart-
#3963 NEW    cov: 6 ft: 5 corp: 4/6b exec/s: 0 rss: 25Mb L: 2 MS: 2 ShuffleBytes-ChangeBit-
#4167 NEW    cov: 7 ft: 6 corp: 5/9b exec/s: 0 rss: 25Mb L: 3 MS: 1 InsertByte-
...
```

```shell
INFO: Seed: 1523017872
INFO: Loaded 1 modules (16 guards): [0x744e60, 0x744ea0),
INFO: -max_len is not provided, using 64
INFO: A corpus is not provided, starting from an empty corpus
#0    READ units: 1
#1    INITED cov: 3 ft: 2 corp: 1/1b exec/s: 0 rss: 24Mb
#3811 NEW    cov: 4 ft: 3 corp: 2/2b exec/s: 0 rss: 25Mb L: 1 MS: 5 ChangeBit-ChangeByte-ChangeBit-ShuffleBytes-ChangeByte-
#3827 NEW    cov: 5 ft: 4 corp: 3/4b exec/s: 0 rss: 25Mb L: 2 MS: 1 CopyPart-
#3963 NEW    cov: 6 ft: 5 corp: 4/6b exec/s: 0 rss: 25Mb L: 2 MS: 2 ShuffleBytes-ChangeBit-
#4167 NEW    cov: 7 ft: 6 corp: 5/9b exec/s: 0 rss: 25Mb L: 3 MS: 1 InsertByte-
...
```

Each output line also reports the following statistics (when non-zero):

- `cov:`

  Total **number of code blocks or edges** covered by executing the current corpus.

- `ft:`

  libFuzzer uses different signals to evaluate the code coverage: edge coverage, edge counters, value profiles, indirect caller/callee pairs, etc. These signals combined are called ***features*** (ft:).

- `corp:`

  **Number of entries** in the current in-memory test corpus and **its size in bytes.**

- `lim:`

  Current limit on the length of new entries in the corpus. Increases over time until the max length (`-max_len`) is reached.

- `exec/s:`

  Number of fuzzer **iterations per second**.

- `rss:`

  Current **memory consumption**.

For `NEW` and `REDUCE` events, the output line also includes information about the mutation operation that produced the new input:

- `L:`

  Size of the new input in bytes.

- `MS: `

  Count and list of the **mutation operations** used to generate the input.

### Examples

#### Toy example

> A simple function that does something interesting if it receives the input “HI!”:

```shell
cat << EOF > test_fuzzer.cc
#include <stdint.h>
#include <stddef.h>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size > 0 && data[0] == 'H')
    if (size > 1 && data[1] == 'I')
       if (size > 2 && data[2] == '!')
       __builtin_trap();
  return 0;
}
EOF
# Build test_fuzzer.cc with asan and link against libFuzzer.
clang++ -fsanitize=address,fuzzer test_fuzzer.cc
# Run the fuzzer with no corpus.
./a.out
```

You should get an error pretty quickly:

```shell
INFO: Seed: 1523017872
INFO: Loaded 1 modules (16 guards): [0x744e60, 0x744ea0),
INFO: -max_len is not provided, using 64
INFO: A corpus is not provided, starting from an empty corpus
#0    READ units: 1
#1    INITED cov: 3 ft: 2 corp: 1/1b exec/s: 0 rss: 24Mb
#3811 NEW    cov: 4 ft: 3 corp: 2/2b exec/s: 0 rss: 25Mb L: 1 MS: 5 ChangeBit-ChangeByte-ChangeBit-ShuffleBytes-ChangeByte-
#3827 NEW    cov: 5 ft: 4 corp: 3/4b exec/s: 0 rss: 25Mb L: 2 MS: 1 CopyPart-
#3963 NEW    cov: 6 ft: 5 corp: 4/6b exec/s: 0 rss: 25Mb L: 2 MS: 2 ShuffleBytes-ChangeBit-
#4167 NEW    cov: 7 ft: 6 corp: 5/9b exec/s: 0 rss: 25Mb L: 3 MS: 1 InsertByte-
==31511== ERROR: libFuzzer: deadly signal
...
artifact_prefix='./'; Test unit written to ./crash-b13e8756b13a00cf168300179061fb4b91fefbed

```

















Reference:

AFL: http://lcamtuf.coredump.cx/afl/

LibFuzzer: http://llvm.org/docs/LibFuzzer.html

PPT [introduction](https://www.usenix.org/sites/default/files/conference/protected-files/enigma_slides_serebryany.pdf)https://www.usenix.org/sites/default/files/conference/protected-files/enigma_slides_serebryany.pdf)