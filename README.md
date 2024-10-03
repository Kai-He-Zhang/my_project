# 目錄

1. [Audio_Spectrogram](#audio_spectrogram)
2. [DIC_loeffler_1D_DCT](#dic_loeffler_1d_dct)
3. [DIP_number-plate_detection](#dip_number-plate_detection)
4. [JPEG_compress_simulation](#jpeg_compress_simulation)
5. [n-puzzle and n-queen](#n-puzzle-and-n-queen)

---

### Audio_Spectrogram
此專案生成多個 `.wav` 音訊檔案，並將它們串接合併成一個音訊檔。合併後，專案會計算並繪製合併音訊信號的頻譜圖。為了簡化整個操作流程，專案提供了一個 Bash 腳本進行整合，只需執行 `.sh` 文件即可自動完成所有步驟，包括生成音訊文件、串接音訊、計算頻譜數據以及繪製頻譜圖。
#### 主要功能：
- **WAV 文件生成**：生成帶有自定義音訊數據的 `.wav` 文件。
- **WAV 文件串接**：將多個 `.wav` 文件串接合併為一個音訊流。
- **頻譜圖視覺化**：生成合併音訊的頻率譜視覺化，展示頻率內容隨時間的變化。

#### 步驟說明(專案資料夾內有詳細內容)：
1. **生成自定義的 `.wav` 文件**：
   - 使用 `singen.c` 程式來生成多個自定義的 `.wav` 文件。`singen.c` 允許用戶根據需求創建特定頻率、波形和時間長度的音訊文件。
   
2. **將 `.wav` 文件串接成一個音訊文件**：
   - 使用 `casade.c` 程式將生成的多個 `.wav` 文件串接合併為一個長音訊文件。`casade.c` 會依照順序將每個 `.wav` 文件無縫串接在一起。

3. **計算音訊的頻譜數據**：
   - 使用 `spectrogram.c` 程式對合併後的音訊文件進行區間離散傅立葉變換(DFT，使用不同的window，rectangular or hamming)，計算頻譜數據，提取出頻率隨時間的變化數據。也可直接使用現成的wav檔案，來計算頻譜數據。

4. **繪製音訊的頻譜圖**：
   - 使用 `spectshow.py` 程式將計算出的頻譜數據進行可視化，繪製音訊的頻譜圖。這個步驟會將頻率和時間關係以圖形化的形式展現出來，方便進行分析與觀察。

#### 執行結果：
![16k aeueo wav hamming_window spectrogram](https://github.com/Kai-He-Zhang/my_project/blob/main/Audio_Spectrogram/aeueo-16kHz-set4.png)

![8k wav hamming_window spectrogram](https://github.com/Kai-He-Zhang/my_project/blob/main/Audio_Spectrogram/s-8k-set4.png)

### DIC_loeffler_1D_DCT
#### 專案簡介

此專案基於 `hybrid_dct_final` 論文，實現了一維離散餘弦轉換（1D DCT）。設計中使用了 11 個 multiplier 和 29 個 adder，並在 0.18 微米製程下實現，時脈運行在 100MHz。專案流程包括使用 Verilog 實作 1D DCT，撰寫 tickle 檔案並通過多個工具進行功能驗證、面積、時序與功耗報告的生成，最終完成版圖設計。

#### 流程說明

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

#### 報告分析

- **面積報告 (Area Report)**：   
![area report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/area.png)
- **時間報告 (Time Report)**：   
![time report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/time.png)
- **功耗報告 (Power Report)**：   
![power report](https://github.com/Kai-He-Zhang/my_project/blob/main/loeffler_1D_DCT/power.png)
- 更詳細的內容可至此專案資料夾閱讀loeffler_1D_DCT.pdf以及1D_DCT_APR_report.pdf

### DIP_number-plate_detection


### JPEG_compress_simulation


### n-puzzle and n-queen

