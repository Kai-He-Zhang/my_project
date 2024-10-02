# Mini-project-4 Spectrogram
多媒體訊號處理第四次作業 - 生成音訊頻譜圖

學生：張愷和   
學號：411086006   
系級：臺北大學通訊工程學系三年級

## 程式環境

wsl ubuntu gcc編譯和執行sh檔案

## 使用方式

git clone repository後，直接執行mini-project-4.sh檔案 

## 輸出
1. 2個4秒的wav檔案，取樣率分別為8000和16000   
    * s-8k.wav
    * s-16k.wav
2. result_txt資料夾，存放16個txt檔案，分別使用4個不同的set，計算4個wav檔的DFT儲存的數值   
    * s-8k-{Set1~Set4}.txt
    * s-16k-{Set1~Set4}.txt
    * aeueo-8kHz-{Set1~Set4}.txt
    * aeueo-16kHz-{Set1~Set4}.txt
3. result_pdf資料夾，存放16個pdf檔案，使用4個wav檔案和每個wav對應的txt檔案生成出來的頻譜圖
    * s-8k-{set1~set4}.pdf
    * s-16k-{set1~set4}.pdf
    * aeueo-8kHz-{set1~set4}.pdf
    * aeueo-16kHz-{set1~set4}.pdf

## 程式碼說明

### sinegen.c

執行方式：   
`./sinegen fs m wavetype f A T fn.wav ` 

參數說明如下：

| 參數 | 意義 | 單位 | applicable values |
| -------- | -------- | -------- | -------|
| fs     | 取樣率    | Hz | 8000/16000     |
| m      | sample size | bit | 16 |
| wavetype | 波形種類 | 無 | 字串: sine/sawtooth/square/triangle |
| f      | 訊號頻率  | Hz | 取決於 fs     |
| A      | 振幅     | int16範圍 | [int16_min, int16_max]     |
| T      | 產生的音訊長度 | second | possible real value |

執行結果：   
生成給定參數的波， 並把輸出的波用.wav檔案儲存起來。  
這次作業bash執行檔中，會使用這個程式生成80個規定的波型(40個fs為8000Hz wav file、40個fs為16000Hz wav file)。   

規定如下：   
A = [100 2000 1000 500 250 100 2000 1000 500 250]    
f = [0 31.25 500 2000 4000 44 220 440 1760 3960]   
wavetypes = [sine sawtooth square triangle]   
fs = [8000 16000]   
T = 0.1   

### cascade.c   
執行方式：   
`./cascade scp output`

參數說明如下：   
1. scp：需要合併的檔案輸入，輸入格式為wav檔
2. output：合併後的檔案輸出，輸出格式為wav檔   

執行結果：   
將scp檔案結合進output檔案。   
如果output檔案已經有結合wav檔，將scp輸入音訊內容結合進output檔案最尾端。(更改header，合併pcm音訊內容)   
如果output檔案尚未建立，則創建新的output檔案，然後將scp檔案複製進output檔案中。(複製header和pcm音訊內容)   

本次作業使用方式(結合sinegen規定參數使用)：

$$ x(t)=\sum_{j=0}^3\sum_{i=0}^9
\lbrace a_i[u(t-0.1i-j)-u(t-0.1+j)]s_j(t-0.1i-j,f_i) \rbrace 
$$

$$
s_j(t,f_i)= \begin{cases} 
sin(2\pi f_it), & \text {for $j = 0$} \\
f_it-\lfloor f_it \rfloor, & \text{for $j = 1$}\\
sgn(sin(2\pi f_it)), & \text{for $j = 2$}\\ 
2|2(f_it-\lfloor f_it \rfloor)|-1, & \text{for $j = 3$} \end{cases} 
$$

上面的公式白話來說就是，每0.1秒生成對應的wavetype、A、f和fs參數波型，0.1秒更換一次A和fs，1秒更換一次wavetype，生成40個波完換另一個fs生成40個波，最後總合會生成兩個4秒不同採樣率的波型。  
由於想說sinegen會按照cascade的順序生成出來需要的波型，因此每做完一次sinegen就直接把結果cascade進需要的output檔案(s-8k and s-16k)，所以沒有在額外的把生成的檔案輸入進一個scp array中，而是直接生成完然後刪除。

### spectrogram.c
執行方式：   
`./spectrogram w_size w_type dft_size f_itv wav_in spec_out`

參數說明如下：
1. w_size：analysis window size (單位：ms)
2. w_type：hamming or rectangular
3. dft_size : DFT/FFT window size (單位：ms)
    * 備註：如果dft_size比w_size還要大，多出來的部分需要補零 
4. f_itv : 擷取短時間音訊片段的時間間隔，音訊片段要拿來計算DFT/FFT。 (單位：ms)
5. wav_in：輸入wav檔案
6. spec_out：輸出txt檔案

執行結果：   
將設定的參數放入spectrogram執行，輸出txt檔會儲存計算好DFT的wav_in頻譜數值，txt檔中每一行會是每個frame計算好DFT的數值，然後以空格分開。

本次作業使用方式：   
1. Setting 1:   
Analysis window size(w_size) = 32ms  
Analysis window type(w_type) = rectangular   
DFT/FFT window size(dft_size) = 32ms   
Frame interval(f_itv) = 10ms
2. Setting 2:   
Analysis window size = 32ms  
Analysis window type = hamming   
DFT/FFT window size = 32ms   
Frame interval = 10ms
3. Setting 3:   
Analysis window size = 30ms  
Analysis window type = rectangular   
DFT/FFT window size = 32ms   
Frame interval = 10ms
4. Setting 4:   
Analysis window size = 30ms  
Analysis window type = hamming   
DFT/FFT window size = 32ms   
Frame interval = 10ms

將以上規定的參數對每個wav_in做分別四種Set，所以每個wav將會產生4個不同Set的spec_out頻譜數值，最終產生16個txt檔案。

### spectshow.py
import函式庫：   
1. sys
2. numpy
3. matplotlib
4. scipy

執行方式：   
`python3 spectshow.py in_wav in_txt out_pdf`

參數說明如下：
1. in_wav：輸入wav檔案
2. in_txt：輸入txt檔案(上個部分計算完DFT的txt檔)
3. out_pdf : 輸出圖片為pdf形式

執行結果：   
顯示輸入的wav波型並用matplotlib.pyplot畫出波型圖。   
將輸入的txt檔案轉換為spectrogram，並用matplotlib.pyplot畫出圖片。   
將wav波型圖和txt檔轉換的spectrogram組合起來，最後輸出成一張檔案為pdf的圖片。

本次作業使用方式：
將每個wav對應到的4個set分別執行spectshow.py，也就是bash檔的一個for迴圈，這樣會產生出四張pdf圖片。然後將其他的wav也這樣做(sh檔中外圍跑wav array的for迴圈)，即可得出4*4張pdf圖片，圖片上半部分為wav檔的波型圖，下半部分為wav的頻譜圖。
