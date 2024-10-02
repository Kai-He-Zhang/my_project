gcc encoder.c -o encoder -lm
./encoder 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw

gcc decoder.c -o decoder -lm
./decoder 1 QResKimberly.bmp KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw
mkdir demo1-a
cp *.txt demo1-a
cp *.raw demo1-a
mv QResKimberly.bmp demo1-a
cp KimberlyNCat.bmp demo1-a

./decoder 1 ResKimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
mkdir demo1-b
mv *.txt demo1-b
mv *.raw demo1-b
mv ResKimberly.bmp demo1-b
cp KimberlyNCat.bmp demo1-b