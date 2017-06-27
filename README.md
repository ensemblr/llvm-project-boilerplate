# llvm-project-boilerplate
Boilerplate for LLVM based Tools

### Compiling LLVM
The LLVM version provided by Ubuntu 16.04 (v3.8) has some problems that lead to [errors](http://stackoverflow.com/questions/42732341/error-using-cmake-with-llvm?noredirect=1#comment72594294_42732341).
<br>We will be building LLVM v`3.9.1` which is the latest as of this writing.
<br>We assume that you have a working compiler toolchain (GCC or LLVM) and that CMake is installed (minimum version 3.4.3).

1.  Download LLVM [source](http://llvm.org/releases/)
and unpack it in a directory of your choice which will refer to as `$LLVM_SRC`

2. Create a separate build directory
    ```bash
    $ mkdir llvm-build
    $ cd llvm-build
    ```
3. Instruct CMake to detect and configure your build environment:

    ```bash
    $ cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_TARGETS_TO_BUILD=X86 $LLVM_SRC
    ```
    Note that we instructed cmake to only build `X86` backend.
    You can choose different backends if needed. Without specifying `LLVM_TARGETS_TO_BUILD`
    all supported backends will be built by default which requires more time.

4. Now start the actual compilation within your build directory

    ```bash
    $ cmake --build .
    ```
    The `--build` option is a portable why to tell cmake to invoke the underlying
    build tool (make, ninja, xcodebuild, msbuild, etc.)

5. Building takes some time to finish. After that you can install LLVM in its default directory which is `/usr/local`
    ```bash
    $ cmake --build . --target install
    ```
    Alternatively, it's possible to set a different install directory (`$LLVM_HOME`)
    ```bash
    $ cmake -DCMAKE_INSTALL_PREFIX=$LLVM_HOME -P cmake_install.cmake
    ```
    Note that `$LLVM_HOME` must __not__ contain `~` (tilde) to refer to your home directory as it won't be expanded. Use absolute paths instead.


### Build
    $ cd llvm-project-boilerplate
    $ mkdir build
    $ cd build
    $ LLVM_DIR="$LLVM_HOME/share/llvm/cmake" cmake ..
    $ make
    $ cd ..

cmake needs to find its LLVM configurations which we provide using `$LLVM_DIR`.

### Run
    $ clang-3.9 -Xclang -load -Xclang build/skeleton/libSkeletonPass.* Skeleton.cpp

Note that Clang needs to be installed separately.

#### Building both LLVM and Clang
1. Run the script download.sh provided in this repo where you want to place the llvm source.

2. Create a separate build directory
    ```bash
    $ mkdir llvm-build
    $ cd llvm-build
    ```

3. Install the Ninja build system
    ```bash
    $ sudo apt install ninja-build
    ```
    Ninja is a preferred build system for llvm toolchain.
    Other options are :
    Unix Makefiles — for generating make-compatible parallel makefiles.
    Visual Studio — for generating Visual Studio projects and solutions.
    Xcode — for generating Xcode projects.
    To use these options, replace "Ninja" in next step by these strings.

4. Instruct CMake to detect and configure your build environment:

    ```bash
    $ cmake -G "Ninja" -DLLVM_TARGETS_TO_BUILD=X86 $LLVM_SRC
    ```
    Note that we instructed cmake to only build `X86` backend.
    You can choose different backends if needed. Without specifying `LLVM_TARGETS_TO_BUILD`
    all supported backends will be built by default which requires more time.

5. Now start the actual compilation within your build directory
    ```bash
    $ ninja
    ```

6. Building takes some time to finish. After that you can install LLVM in its default directory which is `/usr/local`
    ```bash
    $ ninja install
    ```

### Further resources
This tutorial is based on the following resources

- Adrian Sampson's blog entry "LLVM for Grad Students" ([link](http://adriansampson.net/blog/llvm.html))
- LLVM documentation: Writing an LLVM pass ([link](http://llvm.org/docs/WritingAnLLVMPass.html))
- LLVM documentation: Building LLVM with CMake ([link](http://llvm.org/docs/CMake.html#cmake-out-of-source-pass))
