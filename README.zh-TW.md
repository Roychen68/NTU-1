# Cumulative Sum（累積總和）— C++ 解答與使用說明

[English guide](README.md)

這個專案解決附圖中的 **Cumulative Sum** 直方圖題目。完整程式在
[main.cpp](main.cpp)，使用標準 C++11 與標準函式庫，不需要第三方 C++ 套件。
自動測試也完全使用 C++ 撰寫。

你目前使用 Mac，可以先用 **CLion 開啟整個資料夾**。
提交到線上評測系統時，**只需要 `main.cpp`**。

## 檔案導覽

| 檔案 | 用途 |
| --- | --- |
| [main.cpp](main.cpp) | 可直接執行或提交的完整程式 |
| [CMakeLists.txt](CMakeLists.txt) | CLion 與其他支援 CMake 的 IDE 專案設定 |
| [.vscode/tasks.json](.vscode/tasks.json) | VS Code 的編譯與執行工作 |
| [Makefile](Makefile) | 透過終端機使用 `make` 編譯 |
| [examples/](examples/) | 輸入範例與對應的正確輸出 |
| [tests/test_cumulative_sum.cpp](tests/test_cumulative_sum.cpp) | 直接測試解答函式的 C++ 測試程式 |
| [README.md](README.md) | 英文使用說明 |

## 題目要做什麼？

將從 `a` 到 `b` 的範圍平均分成 `m` 個區間（bin），計算每個區間內有多少個
輸入數字，再輸出這些數量的累積總和。範圍包含 `a`，但不包含 `b`。

輸入順序：

1. `a`：下界，包含這個數字。
2. `b`：上界，不包含這個數字。
3. `n`：接下來有幾個數字。
4. `n` 個數字，每個數字都要滿足 `a <= value < b`。
5. `m`：要分成幾個區間，必須大於零。

數字之間可以使用空格、Tab 或換行。每次執行讀取一組資料。
注意：**`m` 在所有數字之後**；最前面沒有「測試資料組數」。

輸出兩行：

1. 第 `0` 到第 `m - 1` 個區間的數量，以空格分隔。
2. 這些數量的累積總和，以空格分隔。

### 圖片中的範例

輸入，也儲存在 [examples/sample.in](examples/sample.in)：

```text
0
100
8
3 25 83 94 39 23 16 56
4
```

輸出，見 [examples/sample.out](examples/sample.out)：

```text
3 2 1 2
3 5 6 8
```

分成四個區間，每個區間寬度為 `(100 - 0) / 4 = 25`：

| 區間編號 | 範圍 | 落在此區間的數字 | 數量 | 累積總和 |
| --- | --- | --- | --- | --- |
| 0 | `[0, 25)` | 3、23、16 | 3 | 3 |
| 1 | `[25, 50)` | 25、39 | 2 | 5 |
| 2 | `[50, 75)` | 56 | 1 | 6 |
| 3 | `[75, 100)` | 83、94 | 2 | 8 |

`[0, 25)` 的意思是「包含 0，不包含 25」。所以 `25` 屬於編號 1 的區間。

## 數學與程式怎麼運作？

編號 `i` 的區間範圍是：

```text
[a + i*(b-a)/m, a + (i+1)*(b-a)/m)
```

這裡的區間邊界使用數學上的除法，結果可以有小數。
要直接找出某個數字的區間編號，可以使用：

```text
bin = floor((value - a) * m / (b - a))
```

`floor` 代表向下取整數。C++ 的非負整數除法會直接捨去小數部分，
因此不需要使用浮點數。範例中的 `39`：

```text
bin = floor((39 - 0) * 4 / (100 - 0))
    = floor(156 / 100)
    = 1
```

若每個區間的寬度是整數，也可以用：

```text
width = (b - a) / m
bin   = (value - a) / width
```

程式只在 `(b - a)` 可以被 `m` 整除時使用這個簡化公式。
例如 `[10, 20)` 分成三個區間時，邊界有小數；整數 `10` 到 `19` 的分布為
`4 3 3`，可使用 [examples/fractional_width.in](examples/fractional_width.in) 測試。

圖片中關於整除的文字與範例不一致。本解答依照圖片中的區間公式實作，
因此區間寬度為整數或小數時都能處理。

`main.cpp` 中的函式用途如下：

| 函式 | 用途 |
| --- | --- |
| `binIndex` | 使用精確的整數運算，算出從 0 開始的區間編號 |
| `countBins` | 將各區間數量設為 0，再依每個數字所屬區間加 1 |
| `printLine` | 輸出各區間數量，或逐項相加後輸出累積總和 |
| `inputError` | 將輸入錯誤訊息寫入錯誤輸出串流 |
| `solve` | 讀取並檢查輸入，計算結果後輸出 |
| `main` | 將鍵盤輸入與螢幕輸出交給 `solve` |

`std::vector` 可以理解為能調整大小的陣列。`counts[i]` 是第 `i` 個區間的
數量，`++counts[i]` 代表加 1。累積總和從 0 開始，依序相加：先得到 `3`，
再得到 `3 + 2 = 5`，接著 `5 + 1 = 6`，最後 `6 + 2 = 8`。

當數字非常大時，`(value - a) * m` 的乘法可能超過整數能表示的範圍，
即使最後的區間編號很小也會如此。`binIndex` 包含避免乘法溢位的精確計算，
不需要編譯器專用型別，也不會有浮點數四捨五入造成的邊界誤差。

固定整數位元數下，時間複雜度為 **O(n + m)**。少數需要避免乘法溢位的
情況，每個受影響的數字最多計算 `unsigned long long` 的位元數次，
在測試平台上為 64 次。記憶體複雜度為 **O(n + m)**：因為最後才讀入 `m`，
必須先保存 `n` 個數字，另外保存 `m` 個區間的數量。

圖片沒有顯示數值限制。在一般 64 位元 C++ 環境中，程式可讀取 `0` 到
`9223372036854775807` 的輸入整數，並要求 `a < b`、`m > 0`，且有足夠的
記憶體存放 `n` 個數字與 `m` 個區間。程式也能處理 `n = 0`、空區間，
以及區間數大於範圍內整數種類數量的情況。

## 使用 CLion 執行

1. 在 CLion 開啟**整個資料夾**，讓它讀取 `CMakeLists.txt`。
2. 等待 CMake 使用你的 C++ 工具鏈完成專案設定。
3. 選擇 `cumulative_sum` 執行目標並執行。
4. 在程式的輸入主控台貼上完整範例，最後一行 `4` 後按 Enter。
5. 確認程式輸出的兩行與上方範例相同。

CLion 的主控台可能另外顯示啟動與結束訊息，這些是 IDE 顯示的資訊。
程式本身只輸出題目要求的兩行。若要執行 C++ 自動測試，
改選擇 `cumulative_sum_tests` 執行目標並執行即可。

## 使用 VS Code 執行

1. 使用 **Open Folder（開啟資料夾）** 開啟本資料夾。
2. 執行 **Build cumulative_sum** 工作，也可使用 **Run Build Task**。
3. 執行 **Run cumulative_sum** 工作。
4. 在該工作的終端機貼上完整範例，最後一個數字後按 Enter。

提供的工作設定在 macOS／Linux 使用 `c++`，Windows 使用 `g++`。
編譯器需要在 `PATH` 中，只有安裝文字編輯器並不代表已安裝 C++ 編譯器。
若使用微軟編譯器，可使用下方 MSVC 指令或 CMake 專案。

若要自動測試函式，執行 **Run C++ tests** 工作，它會編譯並執行 C++ 測試檔。

## 使用終端機編譯與執行

請在本資料夾中執行指令，只需要 C++ 編譯器。

### macOS／Linux：Clang 或 GCC

```sh
c++ -std=c++11 -O2 -Wall -Wextra -Wpedantic main.cpp -o cumulative_sum
./cumulative_sum < examples/sample.in
```

若要手動輸入，執行 `./cumulative_sum`，再貼上範例即可。
如果你的編譯器指令是 `clang++` 或 `g++`，可用它替換 `c++`。

若已安裝 Make，可執行 `make` 編譯，或執行 `make test` 編譯並執行 C++ 測試。
也能使用 `make CXX=clang++` 指定編譯器。

### Windows：GCC／MinGW

在可以使用 `g++` 的終端機執行：

```text
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic main.cpp -o cumulative_sum.exe
```

使用 PowerShell：

```powershell
Get-Content examples/sample.in | .\cumulative_sum.exe
```

使用命令提示字元（Command Prompt）：

```bat
cumulative_sum.exe < examples\sample.in
```

### Windows：Microsoft Visual C++（MSVC）

使用 Visual Studio 的 Developer Command Prompt：

```bat
cl /nologo /EHsc /W4 /std:c++14 main.cpp /Fe:cumulative_sum.exe
cumulative_sum.exe < examples\sample.in
```

MSVC 的 C++14 模式可以編譯這份使用 C++11 功能的程式。

### CMake：CLion 或其他支援 CMake 的環境

若可以使用 CMake 3.16 以上版本與 C++ 編譯器：

```sh
cmake -S . -B build/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/cmake --config Release
```

macOS／Linux 可使用 `./build/cmake/cumulative_sum < examples/sample.in` 執行。
若使用 Visual Studio 的多組態產生器，執行檔通常位於
`build/cmake/Release/cumulative_sum.exe`。

已在你的 Mac 找到 CLion 內附的 CMake：

```text
/Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake
```

如果終端機找不到 `cmake`，可直接使用 CLion 開啟專案，或將指令中的
`cmake` 替換成以上完整路徑。透過 CLion 操作時，不必另外安裝 CMake。

## 自動驗證

要測試原始的 `main.cpp`，先編譯，再將範例輸入交給執行檔：

```sh
c++ -std=c++11 main.cpp -o cumulative_sum
./cumulative_sum < examples/sample.in
```

正確輸出：

```text
3 2 1 2
3 5 6 8
```

若要自行輸入資料，執行 `./cumulative_sum`，依序輸入 `a`、`b`、`n`、
`n` 個數字，最後輸入 `m` 並按 Enter。`.cpp` 是原始程式碼，必須先由編譯器
產生執行檔，才能執行。

若要直接測試各個函式與完整輸入／輸出流程，只需要編譯 C++ 測試檔：

```sh
c++ -std=c++11 -O2 -Wall -Wextra -Wpedantic tests/test_cumulative_sum.cpp -o cumulative_sum_tests
./cumulative_sum_tests
```

測試檔會包含 `main.cpp`，並只停用其中平常使用的 `main`。
它直接呼叫真正的 `binIndex`、`countBins`、`printLine` 與 `solve` 函式。
輸入／輸出測試使用 C++ 字串串流提供資料並擷取結果，不必手動輸入每組測試，
也不需要額外測試框架。

正確結果：

```text
PASS: 1056 valid cases, 14 invalid-input checks, and 16 direct function checks.
```

Windows 使用 GCC／MinGW 時：

```powershell
g++ -std=c++11 -O2 tests/test_cumulative_sum.cpp -o cumulative_sum_tests.exe
.\cumulative_sum_tests.exe
```

若使用 MSVC，使用上方相同的編譯器選項編譯 `tests\test_cumulative_sum.cpp`，
並設定 `/Fe:cumulative_sum_tests.exe`，再執行產生的執行檔。

測試包含 **1,056 組合法案例**、**14 組錯誤輸入檢查**與 **16 組直接函式檢查**。
內容包含圖片範例、邊界值、非零下界、小數區間寬度、空區間、大整數、
1,000 組可重現的隨機資料，以及「所有數字與邊界一起平移後數量不變」等性質。
C++ 參考程式獨立計算精確的整數區間邊界，再檢查每個數字所屬的區間；
每組合法案例也會檢查輸出格式是否完全正確。

2026 年 10 月 7 日已在本機使用 Apple Clang 17 的 C++11 模式、Make，
以及 CLion 內附的 CMake 4.3.1 與 CTest 驗證通過。整套測試也通過
AddressSanitizer 與 UndefinedBehaviorSanitizer 檢查，程式在嚴格警告視為錯誤的
設定下仍能成功編譯。實際執行驗證是在 macOS 進行；Windows 與 Linux 指令
尚未在對應系統實際執行。

完成 CMake 專案編譯後，也能執行已註冊的 C++ 測試：

```sh
cd build/cmake
ctest -C Release --output-on-failure
```

完成後使用 `cd ../..` 回到專案資料夾。若在 CLion 外使用它內附的工具，
`ctest` 與內附的 `cmake` 執行檔位於同一個目錄。

## 提交到線上評測系統

1. 選擇 C++11 或更新的 C++ 版本。
2. 上傳 [main.cpp](main.cpp)，或貼上它的完整內容。
3. 不需要提交說明文件、測試程式或專案設定。

程式不會印出選單或輸入提示。正常輸出只有題目要求的兩行，數字之間使用
一個空格，行末有換行。輸入錯誤時，錯誤訊息寫入 `stderr`，並回傳非零結束狀態。

如果程式看起來一直等待，請確認已輸入全部 `n` 個數字，**以及最後的 `m`**。
輸入完整的一組資料後，不需要再輸入結束檔案字元。
