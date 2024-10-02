import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

in_wav = sys.argv[1]
in_txt = sys.argv[2]
out_pdf = sys.argv[3]

# Read the WAV file
rate, data = wavfile.read(in_wav) 

# Read the text file
txt = np.loadtxt(in_txt) 
Ttxt = np.array(txt).T                 #transpose 讀入的檔案，畫spectrogram必要步驟
half_column = int((Ttxt.shape[0]) / 2) 
Ttxt = Ttxt[:half_column+1, :]         #只列印一半spectrogram，為了符合oceanaudio顯示圖片
Ttxt -= np.max(Ttxt) / 3               #調整能量顯示色彩(大概最大值/3不會顯示，其他就以灰階色差顯示)
mask = Ttxt <= 0
Ttxt[mask] = 0

# Create time and frequency values
y_range = rate / 2                                #取得spectrogram y軸全距
x_range = int(len(data) / rate * 1000)            #取得spectrogram x軸全距
x_values = np.linspace(0, x_range, Ttxt.shape[1]) #重設Ttxt x和y 全距
y_values = np.linspace(0, y_range, Ttxt.shape[0])
xx, yy = np.meshgrid(x_values, y_values)          

# Create subplots for waveform and spectrogram
plt.figure(figsize=(12, 9))                       #畫圖建figure

# Plot waveform
y_ticks = np.arange(-1, 1.1, 0.2)                 #設定y軸刻度間隔0.2為一格
title = "Waveform " + in_wav.replace(".wav", '')  #設定wav圖片標頭
plotA = plt.subplot(211)                          #設定wav圖片為上方subplot，因為要和spectrogram合併成一張圖
time = np.arange(0, len(data)) / rate * 1000      #設定x軸要用的x軸全距
data = data / 32767                               #normalize data大小，除上一個int16_max(sample_size為16)
plotA.plot(time, data, color='b')                 #畫wav圖片
plotA.set_ylabel("Amplitude(norm)")               #設定y軸標籤和單位
plotA.yaxis.tick_right()                          #將y軸設定在右邊
plotA.yaxis.set_label_position("right")
plotA.minorticks_on()                             #顯示小的刻度
plotA.set_title(title)                            #應用標頭
plotA.set_yticks(y_ticks)                         #應用y軸全距
plotA.set_ylim(-1, 1)                             #設定y軸範圍
plotA.set_xlim(0, x_range)                        #設定x軸範圍
plotA.set_xlabel("Time (ms)")                     #設定x軸標籤和單位

# Plot spectrogram
title = "Spectrogram " + in_txt.strip(".txt")     #設定spectrogram圖片標頭
plotB = plt.subplot(212)                          #設定spectrogram圖片為下半圖片
plotB.imshow(Ttxt, aspect='auto', origin='lower', cmap='gray', extent=[0, x_range, 0, y_range]) #畫spectrogram
plotB.set_xlabel("Time (ms)")                     #設定x軸標籤和單位
plotB.set_ylabel("Frequency (Hz)")                #設定y軸標籤和單位
plotB.yaxis.tick_right()                          #設定y軸在右側
plotB.yaxis.set_label_position("right")
plotB.minorticks_on()                             #顯示小的刻度
plotB.set_title(title)                            #應用標頭

# Show the plot
plt.tight_layout()                                
plt.savefig(out_pdf, format='pdf')                #儲存圖片為pdf檔
plt.close()

