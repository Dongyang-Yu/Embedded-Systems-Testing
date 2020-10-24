# Class 2:

- Embedded software
- Talks to hardware not people
- Low level 
- Ex: managing virtual tables
- Sensors and actuators
- Always in demand jobs
- Ex: robots, automotive, etc.

# Class 3:

* Why is embedded difficult
* Hard to simulate hardware faults
* Machine locks up/doesn’t work 

# Class 4:

* Bart Miller
* AFL 
  * -d
    * quick & dirty mode (skips deterministic steps) 快速模式（跳过确认测试）
    * If you have a fixed amount of time to run the program
* Libfuzzer
  * use_value_profile=1
    * When it finds a new location in code
    * Pays attention to bit patterns (different results)
    * Creates more overhead (slows fuzzing down)
    * Can create boring coverage
    * Not always useful
* Abstract def of fuzzers
  * Afl and libfuzzer take <u>input</u> and <u>mutate</u> it k times
    * Then they take the interesting ones to use again/mutate further
    * Improvements usually try to change the <u>“whats interesting</u>” factor
  * No such things as “the best” fuzzer
    * Run a lot of them

# Class 7

* Sanitizers
  * Read article
  * Compile your code so that things that wouldn't be crashes are now crashes
  * Address sanitizer
    * read/wrote memory that isn't valid
  * Undefined sanitizer
    * Compiler, the language does not specify what happens (compiler is free to do what it wants with that behavior, if that will speed up ur code, it will do it)
    * do the Null checks
  * Radamsa
    * Takes a file, produces an new, interesting file (knows about patterns)
  * Memory sanitizer
    * Access to uninitialized values
    * Huge **slow down**
  * Speed is important with fuzzers
  * Should run fuzzer on VM
* Sometimes fuzzers and sanitizers dont work well together
  * Sanitizers are S L O W
    * Usually will make program run twice as slow

Spirit Rover

* Nearly lost because file system needed memory for deleted files
* The rest of the system needed memory but couldnt get it
* Able to control from earth to reformat HDD, control, etc
* Need to test real behavior

Mars Polar Lander

* They had a test and it failed for physical reasons
  * Fixed hardware problem
  * Did not rerun the test after “fixing”
* Testing job is to find out when requirements are broken

# Class14

* Look at deepstate stack example
* Swarm testing:
  * Probability of turning on the function
  * Good at pumping out resource overflow bugs
  * Ex:
    * 10 features and 20 steps
      * 2 of the 10 choices are bad
      * Whats the chance that good stuff is there? 1
      * Whats the chance we aren't including bad things
        * ⅘ ^20 = 1%
        * More bad things, gets worse
  * Suppressors/Triggers
    * Example of initing doing weird stuff:
      * Crypto
        * Ex: computing hash function, encoding
        * important for there to be no nice relationships
  * Deepstate knows how to apply top level tricks

# Class 15

* Binary search test
* Broken comparison function found

# Class 16

* Generalized unit tests
* Know about the domain your testing
* Round Trip-> transforming from f(x) -> x -> f(x) again
* Real bug in cblosc2

# Class 17

* Simulate real hardware
  * Real hardware is slow
  * Hard to inject faults into simulated hw
* Testfs
  * Simulated resets
* Injected simulator faults are a good way to check even good embedded code

# Class 18

* Resets corrupt data

# Class ??

* Triage
  * Crash symptom will tell what bug is (if lucky)
* Reduce test cases (reducer tool)
  * Takes big test into similar test but still causes same failure
* Testing is like science on a program
* Printfs should be experiments (do not just pepper them throughout ur code)
* Try to find whats wrong with stuff
* Assigment 1 prob
  * Using rand in fuzzer harness, its just going to generate value (not rly fuzzing) so dont do that
  * Deepstate works with rand better (but still no good)
* Cblosc
  * Weird multithreading issue
  * Decompressed wrong only when get 1 item
* Difference between libfuzzer and afl
  * Libfuzzer
    * Fuzzes functions
    * Making a function call is fast
    * When something bad happens in that function,
      * It gonna have trouble
      * Could corrupt libfuzzer’s state
      * Run out of memory
    * Raw speed
    * Closer to compiler (comparison ops)
    * Coverage
      * Less 
      * use_value_profile=1 (bit patterns)
  * Afl
    * Fuzzes exec/programs
    * Slower to run exec
    * *Wont* run out of memory
    * Robust, stable
    * Coverage
      * Considers paths
  * Deepstate Cstring manages memory, but Deepstate Malloc does not rn
    * In the future will fix
  * Deepstate tricks
    * Keep tests short



# Difference btwn AFL and libFuzzer

libFuzzer tests a function

AFL tests a  executable program



https://docs.google.com/document/d/1JROR2O2FKHy_NUG2x5g_exPZAuoJi40B_t0bGLSL_uU/edit?usp=sharing