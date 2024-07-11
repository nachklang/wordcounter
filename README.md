WordCounter
============
## Brief overview

WordCounter is a simple demo application that reads words from an input, processes them in a multi-threaded mode, and allows the user to see how many times each word was entered.

Project structure:
- main.cxx - application file
- word_counter.hpp - bussiness logic
- unit tests.cxx - Unit tests for WordCounter class 
- fuzz_tests.cxx - Fuzz tests for WordCounter class

A more detailed specification of the application logic is presented below

### Specification
1. It reads a list of ASCII words from STDIN, one per line, terminated by the word
'end'.
2. It works in multi-threaded mode - reading data from the input occurs in one thread, further processing - in another
3. It removes the duplicate words.
4. It prints the words in alphabetical order, one per line.
5. After each word (and one the same line), it also prints how many times it
occurred.
6. Then it repeatedly asks the user to enter a word and tries to look it up in the
word list. It terminates when it encounters EOF.

### Notes

1. The current validator is implemented using std::regex_search - in the main application, we assume manual input - this should not become a bottleneck. In addition, in case of optimization, the validator can be easily replaced without reworking the main class.

2. std::map is chosen instead of std::unordered_map in order to immediately print sorted data, instead of further manual sorting

3. Test libraries (GTests && Google Fuzz Tests) added via download at CMake stage - this may not be the best option, but the documentation now recommends it

## How to build a project
There are two ways to build a project:

#### Build in docker container
An easy way to build a project and run tests using docker:

1. Build docker container:
```
docker build -t ubuntu-cpp-build .
```

2. Build project with desirable compiler and run tests:

GCC build (with running unit tests):
```
docker run -it -v $(pwd):/home/project ubuntu-cpp-build /bin/bash -c "mkdir -p /home/project/build/build_gcc; pushd /home/project/build/build_gcc;cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DBUILD_TESTS=ON ../../ ;cmake --build . -j${nproc}; ctest"
```

Clang build (with running unit & fuzz tests):
```
docker run -it -v $(pwd):/home/project -e CC="/usr/bin/clang" -e CXX="/usr/bin/clang++" ubuntu-cpp-build /bin/bash -c "mkdir -p /home/project/build/build_clang; pushd /home/project/build/build_clang;cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DBUILD_TESTS=ON ../../;cmake --build . -j${nproc}; ctest"
```


After the build is completed, the artifacts will be available in the following directory:
```
<project_path>/build/build_<compiler_name> // gcc or clang
```

#### Build on Linux

GCC Build && Run Tests:
```
mkdir -p build/build_gcc_local && cd build/build_gcc_local

cmake \
  -DCMAKE_BUILD_TYPE=RelWithDebug -DBUILD_TESTS=ON ../../
cmake --build . -j${nproc}
ctest
```

Clang Build && Run Tests:
```
mkdir -p build/build_clang_local && cd build/build_clang_local

CC=clang CXX=clang++ cmake \
  -DCMAKE_BUILD_TYPE=RelWithDebug -DBUILD_TESTS=ON ../../
cmake --build . -j${nproc}
ctest
```

#### CMake build options

1. BUILD_TESTS -> Build unit tests (GCC) or unit and fuzz tests (Clang)
2. USE_TIDY -> Use clang-tidy for analyzing project code base.
3. SANITIZE -> Enable sanitizers (by default will be enabled bundle of: address,undefined,leak). If you want to specify sanitizer use this option and additional option SANITIZERS, for example:

```
cmake -DSANITIZE -DSANITIZERS=thread ../../
```
#### Build artifacts
After the build, the following executable files will be in the artifacts folder:
```
word_counter - application file described in the specification for the assignment
unit_tests - executable file of unit tests (will appear in the case of a project build with the BUILD_TESTS=ON option)
fuzz_tests - executable file of fuzzing tests (will appear in the case of a project build with the BUILD_TESTS=ON option and a project build using clang)
```

