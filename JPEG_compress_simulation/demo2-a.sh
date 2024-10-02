gcc encoder.c -o encoder -lm
./encoder 2 KimberlyNCat.bmp ascii rle_code.txt
gcc decoder.c -o decoder -lm
./decoder 2 QResKimberly.bmp ascii rle_code.txt

mkdir demo2_a_folder
mv *.txt demo2_a_folder
mv QResKimberly.bmp demo2_a_folder
cp KimberlyNCat.bmp demo2_a_folder