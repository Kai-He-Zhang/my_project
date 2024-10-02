gcc encoder.c -o encoder -lm
./encoder 2 KimberlyNCat.bmp binary rle_code.bin
gcc decoder.c -o decoder -lm
./decoder 2 QResKimberly.bmp binary rle_code.bin
mkdir demo2_b_folder
mv *.bin demo2_b_folder
mv QResKimberly.bmp demo2_b_folder
cp KimberlyNCar.bmp demo2_b_folder