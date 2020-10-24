安装成功后，让我们小试牛刀吧～

# 使用AFL来插桩(instrumentation)

## 选择C程序

用个简单的C程序作为例子

```c
// 文件名暂设为afl_test.c
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h> 

int vuln(char *str)
{
    int len = strlen(str);
    if(str[0] == 'A' && len == 66)
    {
        raise(SIGSEGV);
        //如果输入的字符串的首字符为A并且长度为66，则异常退出
    }
    else if(str[0] == 'F' && len == 6)
    {
        raise(SIGSEGV);
        //如果输入的字符串的首字符为F并且长度为6，则异常退出
    }
    else
    {
        printf("it is good!\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    char buf[100]={0};
    gets(buf);//存在栈溢出漏洞
    printf(buf);//存在格式化字符串漏洞
    vuln(buf);

    return 0;
}
```

首先用afl-gcc进行编译,接着选择小于1k的文件(暂时命名为testcase) 作为输入，然后启动afl-fuzz程序，将刚才的输入文件(testcase) 作为程序的执行输入文件。AFL将在testcase的基础上进行自动变异，使程序产生crash，然后保存到某个指定文件夹下。

## AFL编译

将上面的afl_test.c进行编译：

```shell
afl-gcc -g afl-test.c -o afl-test // ‘-o afl-test‘ 表示指定编译后的程序名为afl-test
```

同样的是，若编译C++源码，则用`afl-g++`

接着建立两个文件夹，分别是`fuzz_in`，`fuzz_out`,用来存放程序的输入和输出

在`fuzz_in`中还需要创建一个testcase的文件，随便输入一点东西即可，比如我们输入`aaaa`即可

**注意**

在编译项目时，通常有Makefile，这时就需要在Makefile中添加内容

gcc/g++重新编译目标程序的方法是：
CC=/path/to/afl/afl-gcc ./configure
make clean all
对于一个C++程序，要设置:
CXX=/path/to/afl/afl-g++.

afl-clang和afl-clang++的使用方法类似。

## 开始Fuzz测试

For target binaries that accept input directly **from stdin**, the usual syntax is:
对那些可以直接从**stdin**读取输入的目标程序来说，语法如下：

> 从stdin读取输入也就是说从键盘输入到缓冲区的东西

```shell
$ ./afl-fuzz -i testcase_dir -o findings_dir /path/to/program [...params...]
```

For programs that **take input from a file**, use '**@@**' to mark the location in the target's command line where the input file name should be placed. The fuzzer will substitute this for you:
对从文件读取输入的目标程序来说，要用“@@”，语法如下

```shell
$ ./afl-fuzz -i testcase_dir -o findings_dir /path/to/program @@
```

You can also use the `-f` option to have the mutated data written to <u>a specific file</u>. This is useful if the program expects a particular file extension or so.

在此例中，输入命令：

```bash
afl-fuzz -i fuzz_in -o fuzz_out ./afl_test
```

但一般都会报错

![image-20200208110130660](AFL%20(American%20fuzzy%20lop).assets/image-20200208110130660.png)

根据提示，需要输入`    echo core >/proc/sys/kernel/core_pattern`，但是我们要在管理者权限下进行设置。

多以就进行：

```shell
sudo su
echo core >/proc/sys/kernel/core_pattern
```

再次运行试试看呢，如果还是有报错，根据它的提示进行修改

![image-20200208110450127](AFL%20(American%20fuzzy%20lop).assets/image-20200208110450127.png)

此处它要求我们修改性能

```shell
cd /sys/devices/system/cpu
echo performance | tee cpu*/cpufreq/scaling_governor
```

这样只是暂时修改了CPU运行模式为高性能，想要修改默认运行模式的话，请使用下方命令

 **1、安装sysfsutils**

```bash
sudo apt-get install sysfsutils
```

 **2、编辑/etc/sysfs.conf**

```shell
sudo gedit /etc/sysfs.conf
```

**3. 在文件下方增加如下语句:**

```
devices/system/cpu/cpu0/cpufreq/scaling_governor = performance
```



返回到刚才的路径下～

再次运行我们就成功啦，wohoo～

![image-20200208110649545](AFL%20(American%20fuzzy%20lop).assets/image-20200208110649545.png)

接下来，让我们试试看这里面这些东西都是些何方神圣。



##  **使用screen**

一次Fuzzing过程通常会持续很长时间，如果这期间运行afl-fuzz实例的终端终端被**意外关闭**了，那么Fuzzing也会被中断。而通过在`screen session`中启动每个实例，可以方便的连接和断开。关于screen的用法这里就不再多讲，大家可以自行查询。

```
$ screen afl-fuzz -i testcase_dir -o findings_dir /path/to/program @@
```



# Interpreting output

See the [status_screen.txt](http://lcamtuf.coredump.cx/afl/status_screen.txt) file for information on how to **interpret** the displayed stats and monitor the health of the process. 

The fuzzing process will continue until you press `Ctrl-C`. At minimum, you want to allow the fuzzer to complete one queue cycle, which may take anywhere from <u>a couple of hours to a week or so</u>.

## 1) Process timing

```shell
  +----------------------------------------------------+
  |        run time : 0 days, 8 hrs, 32 min, 43 sec    |
  |   last new path : 0 days, 0 hrs, 6 min, 40 sec     |
  | last uniq crash : none seen yet                    |
  |  last uniq hang : 0 days, 1 hrs, 24 min, 32 sec    |
  +----------------------------------------------------+
```

这部分很容易理解，告诉我们fuzzer已经运行了多久

## 2) Overall results

```shell
  +-----------------------+
  |  cycles done : 0      |
  |  total paths : 2095   |
  | uniq crashes : 0      |
  |   uniq hangs : 19     |
  +-----------------------+
```

> The first field in this section gives you the count of queue passes done so far - that is, the number of times the fuzzer went over all the interesting test cases discovered so far, fuzzed them, and **looped back to the very beginning**. Every fuzzing session should be allowed to complete at least one cycle; and ideally, should run much longer than that.
> 本节中的第一个字段给出了到目前为止队列传递的次数——也就是说，模糊化器检查到目前为止发现的所有有趣的测试用例、对它们进行模糊化并返回到最开始的次数。应允许每个模糊化会话至少完成一个周期；理想情况下，运行时间应该比这长得多

这里包含了运行的总周期数、总路径数、崩溃数以及超时次数。

其中，总周期数可以用来作为何时停止fuzzing的参考。随着不断地fuzzing，周期数会不断增大，其颜色也会由<u>洋红色，逐步变为黄色、蓝色、绿色。</u>一般来说，当其变为**绿色**时，代表可执行的内容已经很少了，继续fuzzing下去也不会有什么新的发现了。此时，我们便可以通过Ctrl-C，<u>结束</u>当前的fuzzing

![6.jpg](AFL%20(American%20fuzzy%20lop).assets/1552021969_5c81f9d1d0676.jpg!small)

## * 何时停止Fuzz

> To help make the call on when to hit Ctrl-C, the cycle counter is color-coded. It is shown in magenta during the first pass, progresses to yellow if new finds are still being made in subsequent rounds, then blue when that ends - and finally, turns green after the fuzzer hasn't been seeing any action for a longer while.
>
> 当状态窗口中”cycles done”字段颜色变为**<u>绿色</u>**该字段的颜色可以作为何时停止测试的参考，随着周期数不断增大，其颜色也会由洋红色，逐步变为黄色、蓝色、绿色。当其变为绿色时，继续Fuzzing下去也很难有新的发现了，这时便可以通过Ctrl-C停止afl-fuzz。

## 3) Cycle progress

```shell
  +-------------------------------------+
  |  now processing : 1296 (61.86%)     |
  | paths timed out : 0 (0.00%)         |
  +-------------------------------------+
```

This box tells you **how far** along the fuzzer is with the current queue cycle: it shows the **ID of the test case** it is currently working on, plus the number of inputs it decided to **ditch** because they were persistently timing out.

## 4) Map coverage

```shell
  +--------------------------------------+
  |    map density : 10.15% / 29.07%     |
  | count coverage : 4.03 bits/tuple     |
  +--------------------------------------+
```

The section provides some trivia about the coverage observed by the instrumentation embedded in the target binary. 本节提供了一些关于嵌入到目标二进制文件中的工具所观察到的覆盖率的细节。

框中的第一行告诉您我们已经命中了多少个分支元组，与位图可以容纳的数量成比例。左边的数字描述<u>当前输入</u>；右边的是<u>整个输入语料库的值</u>。

## 5) Stage progress

```
  +-------------------------------------+
  |  now trying : interest 32/8         |
  | stage execs : 3996/34.4k (11.62%)   |
  | total execs : 27.4M                 |
  |  exec speed : 891.7/sec             |
  +-------------------------------------+
```

这里包含正在测试的fuzzing策略、进度、目标的执行总次数、目标的执行速度

This part gives you an in-depth peek at what the fuzzer is actually doing right now. It tells you about the current stage, which can be any of:

  - calibration - a pre-fuzzing stage where the execution path is examined to detect anomalies, establish baseline execution speed, and so on. Executed very briefly whenever a new find is being made.
    
  - trim L/S - another pre-fuzzing stage where the test case is trimmed to the shortest form that still produces the same execution path. The length (L) and stepover (S) are chosen in general relationship to file size.
    
  - bitflip L/S - deterministic bit flips. There are L bits toggled at any given time, walking the input file with S-bit increments. The current L/S variants are: 1/1, 2/1, 4/1, 8/8, 16/8, 32/8.
    
  - arith L/8 - deterministic arithmetics. The fuzzer tries to subtract or add small integers to 8-, 16-, and 32-bit values. The stepover is always 8 bits.
    
  - interest L/8 - deterministic value overwrite. The fuzzer has a list of known "interesting" 8-, 16-, and 32-bit values to try. The stepover is 8 bits.
    
  - extras - deterministic injection of dictionary terms. This can be shown as "user" or "auto", depending on whether the fuzzer is using a user-supplied dictionary (-x) or an auto-created one. You will also see "over" or "insert", depending on whether the dictionary words overwrite existing data or are inserted by offsetting the remaining data to accommodate their length.
    
  - havoc - a sort-of-fixed-length cycle with stacked random tweaks. The operations attempted during this stage include bit flips, overwrites with random and "interesting" integers, block deletion, block duplication, plus assorted dictionary-related operations (if a dictionary is supplied in the first place).
    
  - splice 连接 - a last-resort strategy that kicks in after the first full queue cycle with no new paths. It is equivalent to 'havoc', except that it first splices together two random inputs from the queue at some arbitrarily selected midpoint.
    
  - sync - a stage used only when -M or -S is set (see parallel_fuzzing.txt). No real fuzzing is involved, but the tool scans the output from other fuzzers and imports test cases as necessary. The first time this is done, it may take several minutes or so.

The remaining fields should be fairly self-evident: there's the exec count progress indicator for the current stage, a global exec counter, and a benchmark for the current program execution speed. This may fluctuate from one test case to another, but the benchmark should be ideally over 500 execs/sec most of the time - and if it stays below 100, the job will probably take very long.

The fuzzer will explicitly warn you about slow targets, too. If this happens, see the perf_tips.txt file included with the fuzzer for ideas on how to speed things up.



## 6) Findings in depth

```
  +--------------------------------------+
  | favored paths : 879 (41.96%)         |
  |  new edges on : 423 (20.19%)         |
  | total crashes : 0 (0 unique)         |
  |  total tmouts : 24 (19 unique)       |
  +--------------------------------------+
```

This gives you several metrics that are of interest mostly to complete nerds. 

The section includes the number of paths that the fuzzer likes the most based on a minimization algorithm baked into the code (these will get considerably more air time), and the number of test cases that actually resulted in better edge coverage (versus just pushing the branch hit counters up). There are also additional, more detailed counters for crashes and timeouts.
这一部分包括模糊化器最喜欢的路径的数量，这些路径是基于烘焙到代码中的最小化算法(这些将获得相当多的播放时间)，以及实际上导致更好的边缘覆盖的测试用例的数量(相对于仅仅向上推动分支命中计数器)。还有更多更详细的崩溃和超时计数器。

## 7) Fuzzing strategy yields

```shell
  +-----------------------------------------------------+
  |   bit flips : 57/289k, 18/289k, 18/288k             |
  |  byte flips : 0/36.2k, 4/35.7k, 7/34.6k             |
  | arithmetics : 53/2.54M, 0/537k, 0/55.2k             |
  |  known ints : 8/322k, 12/1.32M, 10/1.70M            |
  |  dictionary : 9/52k, 1/53k, 1/24k                   |
  |       havoc : 1903/20.0M, 0/0                       |
  |        trim : 20.31%/9201, 17.05%                   |
  +-----------------------------------------------------+
```

This is just another nerd-targeted section keeping track of how many paths we have netted, in proportion to the number of execs attempted, for each of the fuzzing strategies discussed earlier on. This serves to convincingly validate assumptions about the usefulness of the various approaches taken by afl-fuzz.
记录我们已经为前面讨论的每一个模糊化策略获得了多少条路径，与尝试的高管人数成比例。这有助于令人信服地验证afl-fuzz所采取的各种方法的有效性的假设。

The trim strategy stats in this section are a bit different than the rest. The first number in this line shows the ratio of bytes removed from the input files; the second one corresponds to the number of execs needed to achieve this goal. Finally, the third number shows the proportion of bytes that, although not possible to remove, were deemed to have no effect and were excluded from some of the more expensive deterministic fuzzing steps.



## 8) Path geometry

```shell
  +---------------------+
  |    levels : 5       |
  |   pending : 1570    |
  |  pend fav : 583     |
  | own finds : 0       |
  |  imported : 0       |
  | stability : 100.00% |
  +---------------------+
```

The first field in this section tracks the **path depth** reached through the guided fuzzing process.

The next field shows you the number of inputs that have not gone through any fuzzing yet.

# Option

`-d`: Note that afl-fuzz starts by performing an array of deterministic fuzzing steps, which can take several days, but tend to produce neat test cases. If you want <u>**quick & dirty results right away**</u> - akin to zzuf and other traditional fuzzers - add the -d option to the command line.

```
afl-fuzz [ options ] -- /path/to/fuzzed_app [ ... ]

Required parameters:

  -i dir        - input directory with test cases
  -o dir        - output directory for fuzzer findings

Execution control settings:

  -f file       - location read by the fuzzed program (stdin)
  -t msec       - timeout for each run (auto-scaled, 50-1000 ms)
  -m megs       - memory limit for child process (25 MB)
  -Q            - use binary-only instrumentation (QEMU mode)

Fuzzing behavior settings:

  -d            - quick & dirty mode (skips deterministic steps) 快速模式（跳过确认测试）
  -n            - fuzz without instrumentation (dumb mode)
  -x dir        - optional fuzzer dictionary (see README)

Other stuff:

  -T text       - text banner -to show on the screen
  -M / -S id    - distributed mode (see parallel_fuzzing.txt)
  -C            - crash exploration mode (the peruvian rabbit thing)

For additional tips, please consult /usr/local/share/doc/afl/README.
```

# 分析输出

通过上图，我们看出一共跑出了6个unique的crash

进入对应的`fuzz_out`文件夹下看看有些什么

![image-20200208110908820](AFL%20(American%20fuzzy%20lop).assets/image-20200208110908820.png)

# 三个文件夹 (queue/, crashes/, hangs/)

> created within the output directory and updated in real time

- `queue/`: **test cases** for every distinctive execution path, plus all the starting files given by the user. This is the synthesized **corpus**. 简单来说，就是<u>每个不同执行路径的测试用例</u>
  - Before using this corpus for any other purposes, you can shrink it to a smaller size using the `afl-cmin` tool. The tool will find a **smaller subset** of files offering equivalent edge coverage.

- `crashes/`: unique test cases that cause the tested program to receive **a fatal signal** (e.g., SIGSEGV, SIGILL, SIGABRT). The entries are grouped by the received signal. 产生的crash的样例
- `hangs/`: unique test cases that cause the tested program to **time out**导致程序超时. The default time limit before something is classified as a hang is the larger of <u>1 second and the value of the -t parameter</u>. The value can be fine-tuned by setting `AFL_HANG_TMOUT`, but this is rarely necessary.

Crashes and hangs are considered "**unique**" if the associated execution paths involve any state transitions <u>not seen in previously-recorded faults</u>.



---

### Tip1: 

​	When you can't reproduce a crash found by afl-fuzz, the most likely cause is that you are not setting **the same memory limit** as used by the tool.

​	Try:

```Shell
$ LIMIT_MB=50
$ ( ulimit -Sv $[LIMIT_MB << 10]; /path/to/tested_binary ... )
```

### Tip2:

​	Any <u>existing output</u> directory can be also used to **resume aborted jobs**; try:

```shell
$ ./afl-fuzz -i -o existing_output_dir [...etc...]
```

### Tip3:

​	If you have gnuplot installed, you can also generate some pretty graphs for any active fuzzing task using 	afl-plot. For an example of how this looks like, see http://lcamtuf.coredump.cx/afl/plot/.也可以使用afl-plot	为任何活动的模糊化任务生成一些漂亮的图形

---

## 处理测试结果,分析crash

### xxd 以十六进制的形式显示

进入之前设置的输出文件夹(fuzz_out)，使用xxd命令来查看crash的内容

对照着我们的源码来看

```c
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h> 

int vuln(char *str)
{
    int len = strlen(str);
    if(str[0] == 'A' && len == 66)
    {
        raise(SIGSEGV); //如果输入的字符串的首字符为A并且长度为66，则异常退出
    }
    else if(str[0] == 'F' && len == 6)
    {
        raise(SIGSEGV); //如果输入的字符串的首字符为F并且长度为6，则异常退出
    }
    else
    {
        printf("it is good!\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    char buf[100]={0};
    gets(buf); //存在栈溢出漏洞
    printf(buf); //存在格式化字符串漏洞
    vuln(buf);

    return 0;
}
```

关于[栈溢出](https://www.cnblogs.com/bonelee/p/6149543.html)和[格式化字符串](https://blog.csdn.net/sealyao/article/details/5961330)漏洞，请点击相应链接。

第一个样例，发现符合**栈溢出**漏洞的crash情况

![image-20200208120058632](AFL%20(American%20fuzzy%20lop).assets/image-20200208120058632.png)



第二个样例，发现符合首字符为‘F’且字符串长度为6的异常退出情况

![image-20200208120129245](AFL%20(American%20fuzzy%20lop).assets/image-20200208120129245.png)



第三个样例，发现符合格式化字符串的%n任意地址写的漏洞情况

![image-20200208120227211](file:///media/dongyang/Office/_NAU%20course/1-2020%20Spring/CS-499%20EMBEDDED%20TESTING/Embeded-Notes/AFL%20(American%20fuzzy%20lop).assets/image-20200208120227211.png?lastModify=1581188615)



第四个样例，发现符合栈溢出漏洞的crash情况

![image-20200208120142338](AFL%20(American%20fuzzy%20lop).assets/image-20200208120142338.png)



第五个样例，发现符合栈溢出漏洞的crash情况

![image-20200208120151587](AFL%20(American%20fuzzy%20lop).assets/image-20200208120151587.png)



第六个样例，发现符合首字符为A且字符串长度为66的异常退出情况

![image-20200208120207846](AFL%20(American%20fuzzy%20lop).assets/image-20200208120207846.png)

### hexdump 

非纯文本文件内容查阅，例如二进制文件内容查阅

### 另一个命令，运行程序

`./pbuffer < out/crashes/id...`



Reference:

https://xz.aliyun.com/t/4314

http://lcamtuf.coredump.cx/afl/README.txt