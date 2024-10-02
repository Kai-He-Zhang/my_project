#!/bin/bash
gcc sinegen.c -o sinegen -lm
gcc spectrogram.c -o spectrogram -lm
gcc cascade.c -o cascade

rm *k.wav

amplitude=(100 2000 1000 500 250 100 2000 1000 500 250)
#amplitude=(10000 20000 10000 5000 25000 10000 20000 10000 5000 25000)
f=(0 31.25 500 2000 4000 44 220 440 1760 3960)
sample_sizes=16
wavetypes="sine sawtooth square triangle"
sample_freq="8000 16000"
T=0.1
length=${#amplitude[@]}
i=1

echo "Generate and cascade wav file"
for fs in ${sample_freq}; do
    for m in ${wavetypes}; do
        for ((j = 0; j < ${length}; j++, i++)); do
            ./sinegen ${fs} $sample_sizes ${m} ${f[$j]} ${amplitude[$j]} $T ${i}_${fs}.wav
            #./sinegen ${fs} $sample_sizes ${m} ${f[$j]} 16000 $T ${i}_${fs}.wav
            ./cascade ${i}_${fs}.wav s-${fs}.wav
        done
    done
done
echo "done"

mv s-8000.wav s-8k.wav
mv s-16000.wav s-16k.wav
rm cascade sinegen

wav="s-8k s-16k aeueo-8kHz aeueo-16kHz"
for w in ${wav}; do
    echo "computing ${w}.wav DFT Set1"
    ./spectrogram 32 rectangular 32 10 ${w}.wav ${w}-Set1.txt
    echo "computing ${w}.wav DFT Set2"
    ./spectrogram 32 hamming 32 10 ${w}.wav ${w}-Set2.txt
    echo "computing ${w}.wav DFT Set3"
    ./spectrogram 30 rectangular 32 10 ${w}.wav ${w}-Set3.txt
    echo "computing ${w}.wav DFT Set4"
    ./spectrogram 30 hamming 32 10 ${w}.wav ${w}-Set4.txt
done
echo "done"

for w in ${wav}; do
    for (( i=1; i<=4; i++ )); do
    echo "generating ${w}.wav Set${i} spectrogram"
    python3 spectshow.py ${w}.wav ${w}-Set${i}.txt ${w}-set${i}.pdf
    done
done
echo "done"

rm spectrogram
mkdir result_txt
mkdir result_pdf
mv *.txt result_txt
mv *.pdf result_pdf
