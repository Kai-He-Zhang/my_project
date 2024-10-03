## Topics

### 1. Audio_Spectrogram
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

### 2. DIC_loeffler_1D_DFT


### 3. DIP_number-plate_detection


### 4. JPEG_compress_simulation


### 5. n-puzzle and n-queen

