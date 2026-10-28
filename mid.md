# 第一章：資料結構與演算法總結

本文件整理自《第一章：資料結構簡介》（`2_intro.pdf`），包含課程中介紹的核心演算法與其 C 語言實作細節。

---

## 一、 選擇排序法（Selection Sort）
* **演算法核心概念**：從目前未排序的整數中找出最小值，並把它放到已排序列表的下一個位置。
* **虛擬碼（Pseudo code）**：
  ```c
  for (i = 0; i < n; i++) {
      檢查 list[i] 到 list[n-1]，假設最小的整數是 list[min]
      互換 list[i] 與 list[min]
  }
  ```
* **C 語言實作細節**：
  ```c
  void sort(int list[], int n) {
      int i, j, min, temp;
      for (i = 0; i < n - 1; i++) {
          min = i;
          for (j = i + 1; j < n; j++) {
              if (list[j] < list[min])
                  min = j;
          }
          SWAP(list[i], list[min], temp);
      }
  }
  ```

---

## 二、 二元搜尋法（Binary Search）
* **演算法核心概念**：在一個已排序的陣列中，透過不斷將搜尋範圍對半分割（比較中間值與目標值），來尋找目標數字的位置。

### 1. 迴圈實作版（Iterative Version）
* **C 語言實作細節**：
  ```c
  int compare(int x, int y) {
      /* 回傳 -1 代表小於，0 代表等於，1 代表大於 */
  }

  int binsearch(int list[], int searchno, int left, int right) {
      while (left <= right) {
          int middle = (left + right) / 2;
          switch (COMPARE(list[middle], searchno)) {
              case -1: 
                  left = middle + 1;
                  break;
              case 0: 
                  return middle;
              case 1: 
                  right = middle - 1;
                  break;
          }
      }
      return -1; // 若找不到回傳 -1
  }
  ```

### 2. 遞迴實作版（Recursive Version）
* **C 語言實作細節**：
  ```c
  int binsearch(int list[], int searchno, int left, int right) {
      if (left <= right) {
          int middle = (left + right) / 2;
          switch (COMPARE(list[middle], searchno)) {
              case -1: 
                  return binsearch(list, searchno, middle + 1, right);
              case 0: 
                  return middle;
              case 1: 
                  return binsearch(list, searchno, left, middle - 1);
          }
      }
      return -1;
  }
  ```

---

## 三、 遞迴演算法範例（Recursive Algorithms）

### 1. 遞迴階乘（Recursive Factorial）
* **數學定義**：$n! = n \times (n-1)!$，且 $0! = 1$
* **C 語言實作細節**：
  ```c
  int fact(int n) {
      if (n == 0)
          return (1);
      else
          return (n * fact(n - 1));
  }
  ```

### 2. 遞迴乘法（Recursive Multiplication）
* **數學定義**：$a \times b = a \times (b-1) + a$，且 $a \times 1 = a$
* **C 語言實作細節**：
  ```c
  int mult(int a, int b) {
      if (b == 1)
          return (a);
      else
          return (mult(a, b - 1) + a);
  }
  ```

### 3. 遞迴加總（Recursive Summation）
* **數學定義**：$sum(1, n) = sum(1, n-1) + n$，且 $sum(1, 1) = 1$
* **C 語言實作細節**：
  ```c
  int sum(int n) {
      if (n == 1)
          return (1);
      else
          return (sum(n - 1) + n);
  }
  ```

### 4. 遞迴列表加總（Recursive List Summation）
* **C 語言實作細節**：
  ```c
  float rsum(float list[], int n) {
      if (n) {
          return rsum(list, n - 1) + list[n - 1];
      }
      return list[0];
  }
  ```

### 5. 遞迴排列組合（Recursive Permutations）
* **演算法核心概念**：產生字串或陣列的全排列（例如字串 $\{a, b, c\}$ 的排列）。
* **C 語言實作細節**：
  ```c
  void perm(char *list, int i, int n) {
      int j, temp;
      if (i == n) {
          for (j = 0; j <= n; j++)
              cout << list[j];
          cout << endl;
      }
      else {
          for (j = i; j <= n; j++) {
              SWAP(list[i], list[j], temp);
              perm(list, i + 1, n);
              SWAP(list[i], list[j], temp);
          }
      }
  }
  ```

---

## 四、 矩陣相加（Matrix Addition）
* **演算法核心概念**：將兩個相同維度的矩陣對應位置相加，存入第三個矩陣中。
* **C 語言實作細節**：
  ```c
  void add(matrix a, matrix b, matrix c, int rows, int cols) {
      int i, j;
      for (i = 0; i < rows; i++) {
          for (j = 0; j < cols; j++) {
              c[i][j] = a[i][j] + b[i][j];
          }
      }
  }
  ```
* **效能優化建議**：如果 `rows < cols`，可以考慮將外層與內層迴圈對調（互換 rows 與 cols 的迴圈順序），以符合記憶體存取最佳化。
# 陣列與資料結構：演算法與實作細節總結

本文件整理並歸納自所提供教材（Chapter 2: Arrays and structures）中的核心演算法與實作細節，涵蓋多項式運算、稀疏矩陣轉置與相乘，以及字串比對演算法。

---

## 1. 多項式加法 (Polynomial Addition)

多項式可以用不同的方式表示與運算。教材中探討了利用結構陣列（Struct Array）來儲存具有非零係數與指數的項次。

* **資料結構設計：**
  * 使用全域陣列 (`terms`) 依序儲存所有多項式的項次，每個項次包含係數 (`coef`) 與指數 (`expon`)。
  * 每個多項式由其在陣列中的起始位置 (`start`) 與結束位置 (`finish`) 來界定。

* **演算法核心精神：**
  * 同時走訪兩個多項式 $A$ 與 $B$ 的非零項。
  * 利用比較函數 (`COMPARE`) 比較兩者當前的最高指數：
    1. **指數相同時：** 將兩者係數相加。若和大於零，則附加 (`attach`) 到結果多項式 $D$ 中，並同時推進兩個多項式的指標。
    2. **$A$ 的指數大於 $B$ 的指數：** 將 $A$ 當前的項次附加到 $D$ 中，並推進 $A$ 的指標。
    3. **$B$ 的指數大於 $A$ 的指數：** 將 $B$ 當前的項次附加到 $D$ 中，並推進 $B$ 的指標。
  * 迴圈結束後，將未處理完畢的剩餘項次依序附加至結果多項式中。
  * **時間複雜度：** $O(n + m)$，其中 $n$ 與 $m$ 分別為兩多項式的非零項個數。

---

## 2. 稀疏矩陣轉置 (Sparse Matrix Transpose)

當矩陣中包含大量的零元素時，直接使用二維陣列會造成空間浪費。稀疏矩陣採用三元組 `<row, column, value>` 進行壓縮儲存。

### 2.1 一般轉置 (Standard Transpose)
* **實作細節：** 依序掃描原矩陣 $A$ 的每一欄，並在 $A$ 中尋找符合該欄號的元素，將其行與欄對調後寫入矩陣 $B$。
* **時間複雜度：** $O(\text{columns} \times \text{elements})$，當非零元素極多時效率較差。

### 2.2 快速轉置 (Fast Transpose)
* **實作細節：**
  1. 引入輔助陣列 `row_terms`，統計原矩陣每一欄中非零元素的總個數。
  2. 引入輔助陣列 `starting_pos`，計算出轉置後每一列在目標陣列中的起始存放位置。
  3. 僅需對原矩陣進行一次掃描，即可直接將每個元素放置到目標矩陣的正確對應位置上。
* **時間複雜度：** $O(\text{columns} + \text{elements})$，大幅提升了轉置效率。

---

## 3. 稀疏矩陣相乘 (Sparse Matrix Multiplication)

* **演算法核心精神：**
  * 為了有效計算 $D = A \times B$，首先對矩陣 $B$ 進行快速轉置 (`fast_transpose`)，使 $B$ 的每一欄轉化為連續儲存的資料列，便於後續的內積計算。
  * 固定矩陣 $A$ 的某一列，與轉置後的 $B$ 之各欄進行對應元素的指數或行欄匹配。
  * 當行與欄匹配成功時，將兩者數值相乘並累加到 `sum` 變數中。
  * 計算完畢後，透過 `storesum` 函數將不為零的乘積結果存入結果矩陣 $D$ 的三元組陣列中。
* **時間複雜度：** $O(\text{cols\_b} \times \text{total\_a} + \text{rows\_a} \times \text{total\_b})$。

---

## 4. 字串模式比對：KMP 演算法 (Knuth-Morris-Pratt Algorithm)

傳統的字串暴力比對法在遇到不匹配時會將指標回溯，時間複雜度高達 $O(m \times n)$。KMP 演算法透過前處理有效避免不必要的重複比對。

* **兩階段實作：**
  * **階段一：計算前綴函數/失敗函數 ($\Pi$ 陣列 / Failure Function)**
    * 建立一個陣列來記錄模式字串 $P$ 中各個子字串的最長共同前後綴長度。
    * 當比對失敗時，利用 $\Pi$ 陣列決定模式字串要向右滑動多少距離，而不需要將文字指標 $i$ 回溯。
  * **階段二：模式匹配 (`pmatch`)**
    * 同時掃描文字 $S$ 與模式 $P$。
    * 若字元相符，兩者指標皆加一。
    * 若字元不符且 $j > 0$，則利用失敗函數更新 $j$ 的位置（$j = \text{failure}[j-1] + 1$）；若 $j == 0$，則僅將文字指標 $i$ 往後推進。
* **時間複雜度：** $O(m + n)$，其中 $m$ 為模式長度（用於計算前綴函數），$n$ 為文字長度（用於字串搜尋）。
