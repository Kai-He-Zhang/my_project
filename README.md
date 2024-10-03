# 目錄

1. [Audio_Spectrogram](#audio_spectrogram)
2. [DIC_loeffler_1D_DCT](#dic_loeffler_1d_dct)
3. [DIP_number-plate_detection](#dip_number-plate_detection)
4. [JPEG_compress_simulation](#jpeg_compress_simulation)
5. [n-puzzle and n-queen](#n-puzzle-and-n-queen)

---

## Audio_Spectrogram
此專案生成多個 `.wav` 音訊檔案，並將它們串接合併成一個音訊檔。合併後，專案會計算並繪製合併音訊信號的頻譜圖。為了簡化整個操作流程，專案提供了一個 Bash 腳本進行整合，只需執行 `.sh` 文件即可自動完成所有步驟，包括生成音訊文件、串接音訊、計算頻譜數據以及繪製頻譜圖。
### 主要功能：
- **WAV 文件生成**：生成帶有自定義音訊數據的 `.wav` 文件。
- **WAV 文件串接**：將多個 `.wav` 文件串接合併為一個音訊流。
- **頻譜圖視覺化**：生成合併音訊的頻率譜視覺化，展示頻率內容隨時間的變化。

### 步驟說明(專案資料夾內有詳細內容)：
1. **生成自定義的 `.wav` 文件**：
   - 使用 `singen.c` 程式來生成多個自定義的 `.wav` 文件。`singen.c` 允許用戶根據需求創建特定頻率、波形和時間長度的音訊文件。
   
2. **將 `.wav` 文件串接成一個音訊文件**：
   - 使用 `casade.c` 程式將生成的多個 `.wav` 文件串接合併為一個長音訊文件。`casade.c` 會依照順序將每個 `.wav` 文件無縫串接在一起。

3. **計算音訊的頻譜數據**：
   - 使用 `spectrogram.c` 程式對合併後的音訊文件進行區間離散傅立葉變換(DFT，使用不同的window，rectangular or hamming)，計算頻譜數據，提取出頻率隨時間的變化數據。也可直接使用現成的wav檔案，來計算頻譜數據。

4. **繪製音訊的頻譜圖**：
   - 使用 `spectshow.py` 程式將計算出的頻譜數據進行可視化，繪製音訊的頻譜圖。這個步驟會將頻率和時間關係以圖形化的形式展現出來，方便進行分析與觀察。

### 執行結果：
![16k aeueo wav hamming_window spectrogram](https://github.com/Kai-He-Zhang/my_project/blob/main/Audio_Spectrogram/aeueo-16kHz-set4.png)

![8k wav hamming_window spectrogram](https://github.com/Kai-He-Zhang/my_project/blob/main/Audio_Spectrogram/s-8k-set4.png)

## DIC_loeffler_1D_DCT
### 專案簡介

此專案基於 `hybrid_dct_final` 論文，實現了一維離散餘弦轉換（1D DCT）。設計中使用了 11 個 multiplier 和 29 個 adder，並在 0.18 微米製程下實現，時脈運行在 100MHz。專案流程包括使用 Verilog 實作 1D DCT，撰寫 tickle 檔案並通過多個工具進行功能驗證、面積、時序與功耗報告的生成，最終完成版圖設計。

### 流程說明

1. **Verilog 實作 1D DCT**：
   - 首先，使用 Verilog 語言實作 1D DCT，基於 `hybrid_dct_final` 論文中的算法架構設計。
   - 實作的 1D DCT 設計包含 11 個 multiplier 和 29 個 adder，針對高效能運算優化。

2. **功能驗證 (vsim)**：
   - 使用 `vsim` 進行功能模擬，以檢查 Verilog 設計的正確性。
   - 觀察模擬結果，確保 DCT 運算的輸入與輸出功能正確。

3. **合成 (Design Compiler)**：
   - 使用 Synopsys Design Compiler 對 Verilog 設計進行邏輯合成，生成面積（area）、時間（time）和功耗（power）報告。
   - 設計目標為 0.18 微米製程，時脈運行在 100MHz。

4. **功耗分析 (PrimeTime)**：
   - 使用 `vsim` 模擬生成的 `vcd` 檔案，並使用 Synopsys PrimeTime 對設計進行精確的功耗分析。
   - 生成精確的功耗報告，與合成報告進行比對。

5. **APR (Auto Place and Route)**：
   - 進行自動佈局與繞線 (APR) 的設計流程，將合成結果映射到實際物理設計上。
   - 完成佈局與繞線後，將最終的結果輸入 Design Compiler 和 PrimeTime，重新生成面積、時間和功耗報告。

### 報告分析

- **面積報告 (Area Report)**：   
![area report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/area.png)
- **時間報告 (Time Report)**：   
![time report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/time.png)
- **功耗報告 (Power Report)**：   
![power report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/power.png)
- 更詳細的內容可至此專案資料夾閱讀loeffler_1D_DCT.pdf以及1D_DCT_APR_report.pdf

## DIP_number-plate_detection
### 專案簡介

本專案使用 MATLAB 實現車牌號碼位置辨識。該專案利用數位影像處理技術，從輸入的車輛圖片中辨識並輸出車牌的位置座標。核心流程包括對圖片進行預處理、邊緣偵測、形態學操作以及車牌區域的篩選與判斷，最終將車牌位置座標輸出至 `.txt` 文件中。

### 流程說明

1. **圖片預處理**：
   - 將輸入圖片進行多步處理，以便更容易識別車牌位置：
     - **灰階化**：將彩色圖片轉換為灰階圖片，降低計算複雜度。
     - **去噪處理**：使用濾波器消除圖片中的雜訊，以減少偽邊緣和錯誤的區塊判斷。
     - **邊緣偵測**：使用邊緣檢測技術（如 Canny 邊緣偵測）來突出圖片中的物體輪廓。
     - **二值化**：將圖片轉換為黑白二值圖片，使目標區域更易於識別。
     - **開運算與閉運算**：使用形態學操作（開運算消除小區塊、閉運算填補區塊內的空隙）以更準確地提取可能的車牌區域。

2. **車牌區域篩選**：
   - 使用 **bounding box**（邊界框）來判斷圖片中的黑色面積區域，篩選出可能符合車牌大小與長寬比的區域。
   - 對符合條件的面積區塊進行進一步處理，進行二次圖像處理以提高精確度，包括灰階化、消噪、邊緣偵測及二值化。

3. **車牌辨識判斷**：
   - 根據以下規則判斷該區域是否為車牌：
     - **黑色面積數量**：根據二值化後的結果，判斷區域內黑色面積數量是否符合車牌中號碼數。
     - **黑色面積水平排列**：判斷號碼是否呈水平排列，符合車牌號碼的格式。
     - **黑色面積（號碼）的長寬比**：判斷黑色區塊（號碼）的長寬比是否符合一般車牌字元的特徵。

4. **結果輸出**：
   - 最終，對每張圖片識別出的車牌位置（座標）將記錄在 `.txt` 檔案中，作為最終輸出。

### 輸出格式
每張圖片對應的車牌號碼位置報告將以如下格式記錄：

- **照片名稱**
- **車牌號碼數量**
- **各個號碼的位置**（每個號碼一行，記錄其座標與大小）
- 詳細內容可看專案資料夾txt檔案
  

## JPEG_compress_simulation
### 專案簡介

本專案使用 C 語言模擬 JPEG 壓縮過程，並通過 `encoder` 將圖片壓縮為 `.bin` 文件，使用 `decoder` 則將該 `.bin` 文件還原為圖片。專案主要觀察壓縮過程中產生的壓縮比，並對不同過程的輸出進行分析，包括原始圖片與壓縮還原後圖片的比較。

### 主要功能與流程

### Encoder (編碼器)
1. **讀取 BMP RGB**：
   - 讀取 BMP 圖片的 RGB 資料。
   
2. **RGB 轉 YCbCr**：
   - 將 RGB 資料轉換為 YCbCr 色彩空間。

3. **YCbCr DCT 計算**：
   - 對 YCbCr 的各通道計算離散餘弦變換 (DCT)。

4. **量化 (Quantization)**：
   - 對 DCT 係數進行量化壓縮。

5. **DPCM (差分脈衝編碼調變)**：
   - 對 DC 成分進行差分脈衝編碼 (DPCM)。

6. **ZigZag 排序與 RLE 壓縮**：
   - 使用 ZigZag 排序並對數據進行游程編碼 (RLE) 壓縮。

### Decoder (解碼器)
Decoder 將 `encoder` 過程反向執行，以還原壓縮後的 `.bin` 文件，並生成最終圖片。

- **還原步驟**：
   1. 反 ZigZag 排序。
   2. 逆量化（inverse quantization）。
   3. 逆 DCT 計算。
   4. YCbCr 轉換回 RGB。
   5. 輸出還原的 BMP 圖片。

### 分部分介紹

#### 第 0 部分：讀取與寫入 BMP RGB

- 讀取 BMP 圖片的 RGB 資料，並將其重新寫入一個新的 BMP 檔案。觀察兩者的差異。

#### 第 1 部分：量化後的還原

- 完成量化壓縮後，使用 `decoder` 從逆量化開始，重新生成 BMP 圖片。
- 額外計算量化誤差 (quantization error) 以及 YCbCr 的信號噪聲比 (SQNR)。

#### 第 2 部分：RLE 壓縮與完整還原

- 經過 RLE 壓縮後，使用 `decoder` 將整個編碼過程還原，並輸出 BMP 圖片。

### 壓縮率觀察

- 在每個編碼過程中，生成的 `.bin` 文件可以通過觀察其大小來了解壓縮率。通過編碼器與解碼器的交互，使用者可以在不同階段檢查圖像壓縮效果與質量損失。

### 使用方式

#### 編譯與執行

1. 使用 `makefile` 進行整合，輸入以下命令以編譯所有 `.c` 文件：
   ```bash
   make build
   ```
2. 執行各個部分的 Demo：
   ```bash
   ./demo0
   ./demo1
   ./demo2
   ```
### 結果與觀察
- 透過 `demo0` 可以比較原始圖片與讀寫後圖片的差異，驗證讀寫正確性。
- `demo1` 讓用戶觀察量化後還原圖片與量化誤差、SQNR。
- `demo2` 則展示完整的壓縮和還原流程，並比較壓縮前後的檔案大小。

## n-puzzle and n-queen

### 專案簡介

本專案實現了 `n-puzzle` 和 `n-queen` 問題的求解，尺寸分別為 8 和 15。採用了多種演算法，包括 A* 搜索、GBFS（Greedy Best-First Search）、RBFS（Recursive Best-First Search）、UCS（Uniform Cost Search）和 IDS（Iterative Deepening Search），以高效地尋找解決方案。

### n-puzzle

#### 執行方式

- 專案已將 `n-puzzle` 的執行方式整合成一個 Bash 檔案，可以直接執行以觀察結果。執行命令如下：
  ```bash
  ./execute.sh
  ```
  若要更改輸入盤面，需在程式碼中進行相應的修改。

#### 功能

專案提供以下額外的記錄：
- (a) 從初始狀態到目標狀態的移動次數（狀態變更數）。
- (b) 在過程中內存中保存的最大狀態數量。

### n-queen

#### 執行方式

`n-queen` 專案的執行方式是直接執行編譯過的 C++ 檔案。使用者需要先輸入盤面上有多少個皇后，然後輸入每個皇后的座標位置，以便開始判斷該盤面是否有解。執行命令如下：

  ```bash
  ./n_queen
  ```

輸入範例可直接從程式碼中複製。

#### 功能

本專案同樣提供對於每個搜索的額外記錄：
- (a) 從初始狀態到目標狀態的移動次數（狀態變更數）。
- (b) 在過程中內存中保存的最大狀態數量。
