gcc encoder.c -o encoder -lm
./encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt
gcc decoder.c -o decoder -lm
./decoder 0 ResKimberly.bmp R.txt G.txt B.txt dim.txt
mkdir demo0
mv *.txt demo0
mv ResKimberly.bmp demo0
cp KimberlyNCat.bmp demo0