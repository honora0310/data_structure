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
