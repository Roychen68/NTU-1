# Cumulative Sum — C++ solution and user guide

[繁體中文使用說明](README.zh-TW.md)

This project solves the **Cumulative Sum** histogram problem in the supplied
image. The complete program is in [main.cpp](main.cpp). It uses standard C++11
and the standard library, with no third-party C++ dependencies. The automated
tests are also written entirely in C++.

For your current Mac, opening this folder in **CLion** is a convenient starting
point. For the online judge, submit **only `main.cpp`**.

## Find the files

| File | Purpose |
| --- | --- |
| [main.cpp](main.cpp) | Complete, standalone program to run or submit |
| [CMakeLists.txt](CMakeLists.txt) | Project setup for CLion and other CMake-based IDEs |
| [.vscode/tasks.json](.vscode/tasks.json) | Build and run tasks for VS Code |
| [Makefile](Makefile) | Optional terminal build with `make` |
| [examples/](examples/) | Inputs and expected outputs |
| [tests/test_cumulative_sum.cpp](tests/test_cumulative_sum.cpp) | C++ tests of the actual solution functions |
| [README.zh-TW.md](README.zh-TW.md) | Traditional Chinese guide |

## What the problem asks

Divide the range from `a` up to, but excluding, `b` into `m` equal-width bins.
Count how many input values belong to each bin. Then print the running totals
of those counts.

Input order:

1. `a`: lower boundary, included.
2. `b`: upper boundary, excluded.
3. `n`: number of values.
4. The `n` values, each satisfying `a <= value < b`.
5. `m`: number of bins, greater than zero.

Spaces, tabs, and newlines all separate input numbers. The program reads one
case per run. `m` comes **after** the values; there is no initial test-case count.

Print exactly two lines:

1. Counts for bins `0` through `m - 1`, separated by spaces.
2. Their cumulative sums, separated by spaces.

### Sample from the image

Input — also saved as [examples/sample.in](examples/sample.in):

```text
0
100
8
3 25 83 94 39 23 16 56
4
```

Output — [examples/sample.out](examples/sample.out):

```text
3 2 1 2
3 5 6 8
```

There are four bins, each with width `(100 - 0) / 4 = 25`:

| Bin | Range | Values in this bin | Count | Cumulative sum |
| --- | --- | --- | --- | --- |
| 0 | `[0, 25)` | 3, 23, 16 | 3 | 3 |
| 1 | `[25, 50)` | 25, 39 | 2 | 5 |
| 2 | `[50, 75)` | 56 | 1 | 6 |
| 3 | `[75, 100)` | 83, 94 | 2 | 8 |

`[0, 25)` means “include 0, exclude 25.” Therefore, `25` goes into bin 1.

## Understand the math and code

The range of bin `i` is:

```text
[a + i*(b-a)/m, a + (i+1)*(b-a)/m)
```

Those boundary divisions are mathematical divisions and can have fractions.
To find a value's bin directly:

```text
bin = floor((value - a) * m / (b - a))
```

`floor` rounds down. C++ integer division already rounds down for nonnegative
integers. For `value = 39` in the sample:

```text
bin = floor((39 - 0) * 4 / (100 - 0))
    = floor(156 / 100)
    = 1
```

When the width is an integer, the equivalent formula is:

```text
width = (b - a) / m
bin   = (value - a) / width
```

The program uses this shortcut only when `(b - a)` is divisible by `m`.
For example, `[10, 20)` divided into three bins has fractional boundaries,
and the ten integers `10` through `19` have counts `4 3 3`.
See [examples/fractional_width.in](examples/fractional_width.in).

The photograph's sentence about divisibility conflicts with its sample.
This solution follows the displayed interval definition and handles both
integer and fractional bin widths.

The functions in `main.cpp` do the following:

| Function | What it does |
| --- | --- |
| `binIndex` | Finds the zero-based bin using exact integer arithmetic |
| `countBins` | Starts every count at zero, then increments each value's bin |
| `printLine` | Prints counts, or adds them one at a time to print cumulative sums |
| `inputError` | Reports malformed input to the error stream |
| `solve` | Reads and checks input, then calculates and prints the result |
| `main` | Connects keyboard input and screen output to `solve` |

A `std::vector` is a resizable array. `counts[i]` is the count for bin `i`.
`++counts[i]` adds one. A running sum starts at zero and repeatedly adds the
next count: `3`, then `3 + 2 = 5`, then `5 + 1 = 6`, then `6 + 2 = 8`.

For very large numbers, multiplying `(value - a) * m` can overflow even when
the final bin index is small. `binIndex` includes an exact integer fallback
that handles this without compiler-specific types or floating-point rounding.

Time is **O(n + m)** for fixed-width integer types. The rare overflow fallback
takes at most one step per bit of `unsigned long long` for each affected value
(64 steps on the tested platform). Memory is **O(n + m)**: the program stores
the values because `m` is read last, plus the counts.

The image does not show numeric limits. On typical 64-bit C++ implementations,
this program accepts input integers from `0` through `9223372036854775807`,
subject to `a < b`, `m > 0`, and sufficient memory for `n` values and `m` bins.
It also handles `n = 0`, empty bins, and more bins than distinct integer values.

## Run in CLion

1. Open this **folder** in CLion so it loads `CMakeLists.txt`.
2. Let CMake configure the project using your C++ toolchain.
3. Select the `cumulative_sum` executable and run it.
4. Paste the entire sample input into the program's input console. Press Enter
   after the final `4`.
5. Compare the two output lines with the sample above.

CLion's run console can also show its own startup and exit messages; these are
separate from the program's two output lines. To run the automated C++ tests,
select the `cumulative_sum_tests` executable and run it instead.

## Run in VS Code

1. Open this folder with **Open Folder**.
2. Run the **Build cumulative_sum** task, or use **Run Build Task**.
3. Run the **Run cumulative_sum** task.
4. Paste the sample input into its terminal and press Enter after the last number.

The supplied tasks use `c++` on macOS/Linux and `g++` on Windows. The compiler
must be available on `PATH`; an editor alone does not supply a C++ compiler.
If you use Microsoft's compiler, follow the MSVC command below or use the
CMake project.

To test the functions automatically, run the **Run C++ tests** task. It compiles
and runs the C++ test file.

## Compile and run in a terminal

Run these commands from this folder. You only need a C++ compiler.

### macOS or Linux: Clang/GCC

```sh
c++ -std=c++11 -O2 -Wall -Wextra -Wpedantic main.cpp -o cumulative_sum
./cumulative_sum < examples/sample.in
```

For interactive input, run `./cumulative_sum` and paste the sample instead.
You can replace `c++` with `clang++` or `g++` if that is your compiler command.

With Make installed, `make` builds the program and `make test` compiles and runs
the C++ tests. You can choose another compiler with `make CXX=clang++`.

### Windows: GCC/MinGW

Compile in a terminal where `g++` is available:

```text
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic main.cpp -o cumulative_sum.exe
```

In PowerShell:

```powershell
Get-Content examples/sample.in | .\cumulative_sum.exe
```

In Command Prompt:

```bat
cumulative_sum.exe < examples\sample.in
```

### Windows: Microsoft Visual C++ (MSVC)

Use a Visual Studio Developer Command Prompt:

```bat
cl /nologo /EHsc /W4 /std:c++14 main.cpp /Fe:cumulative_sum.exe
cumulative_sum.exe < examples\sample.in
```

MSVC's C++14 mode supports this program's C++11 features.

### CMake: CLion or another CMake-capable environment

With CMake 3.16 or later and a C++ compiler available:

```sh
cmake -S . -B build/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/cmake --config Release
```

On macOS/Linux, run `./build/cmake/cumulative_sum < examples/sample.in`.
With a Visual Studio multi-configuration generator, the executable is normally
`build/cmake/Release/cumulative_sum.exe`.

On your Mac, CLion's bundled CMake was found at:

```text
/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake
```

If `cmake` is not on `PATH`, open the project in CLion or replace `cmake` in the
commands with that full path. No separate CMake installation is needed for
the CLion workflow.

## Verify the solution

To test the raw `main.cpp`, first compile it, then run the executable with the
sample input:

```sh
c++ -std=c++11 main.cpp -o cumulative_sum
./cumulative_sum < examples/sample.in
```

Expected output:

```text
3 2 1 2
3 5 6 8
```

For your own inputs, run `./cumulative_sum` and type `a`, `b`, `n`, the `n`
values, then `m`. Press Enter after `m`. A `.cpp` file is source code; the
compiler creates the executable that you run.

To test individual functions and the complete input/output behavior, compile
the C++ test file by itself:

```sh
c++ -std=c++11 -O2 -Wall -Wextra -Wpedantic tests/test_cumulative_sum.cpp -o cumulative_sum_tests
./cumulative_sum_tests
```

The test file includes `main.cpp` and disables only its normal `main` function.
It calls the actual `binIndex`, `countBins`, `printLine`, and `solve` functions.
For input/output checks, C++ string streams supply input and capture output
without requiring you to type each case. No test framework is required.

Expected result:

```text
PASS: 1056 valid cases, 14 invalid-input checks, and 16 direct function checks.
```

On Windows with GCC/MinGW:

```powershell
g++ -std=c++11 -O2 tests/test_cumulative_sum.cpp -o cumulative_sum_tests.exe
.\cumulative_sum_tests.exe
```

With MSVC, compile `tests\test_cumulative_sum.cpp` using the same compiler flags
shown above, with `/Fe:cumulative_sum_tests.exe`, then run that executable.

The suite checks **1,056 valid cases**, **14 invalid-input cases**, and
**16 direct function checks**.
It covers the image's sample, boundaries, nonzero `a`, fractional widths, empty
bins, large integers, 1,000 reproducible random cases, and properties such as
preserving counts when all values and boundaries are shifted equally.
An independent C++ reference builds exact integer interval boundaries and
checks each value against them.
Every valid test also checks exact output formatting.

Local verification on 7 October 2026 passed with Apple Clang 17 in C++11 mode,
Make, and CLion's bundled CMake 4.3.1 with CTest. The entire suite also passed
with AddressSanitizer and UndefinedBehaviorSanitizer, and the source compiled
with strict warnings treated as errors. Runtime validation was performed on
macOS; the Windows and Linux instructions have not been executed on those systems.

After building the CMake project, run its registered C++ test with:

```sh
cd build/cmake
ctest -C Release --output-on-failure
```

Return to the project folder with `cd ../..` afterward. When using the bundled
CLion tools outside CLion, `ctest` is alongside the bundled `cmake` executable.

## Submit to the online judge

1. Select C++11 or a later C++ version.
2. Upload [main.cpp](main.cpp), or paste its complete contents.
3. Do not include the guides, test script, or project configuration in the submission.

The program prints no menus or input prompts. It prints the required two lines
with single spaces between numbers and a final newline. Invalid input produces
a message on `stderr` and a nonzero exit status.

If the program appears to wait, make sure you supplied all `n` values **and the
final `m`**. There is no need to type an end-of-file character after a complete case.
# NTU-1
