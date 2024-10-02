INCLUDE = -I.

all: build

build:
	gcc encoder.c -o encoder.exe -lm
	gcc decoder.c -o decoder.exe -lm

demo0:
	./encoder.exe 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	./decoder.exe 0 ResKimberly.bmp R.txt G.txt B.txt dim.txt
	@mkdir -p demo0_folder
	@mv *.txt demo0_folder
	@mv ResKimberly.bmp demo0_folder
	@cp KimberlyNCat.bmp demo0_folder
	cmp -l demo0_folder/KimberlyNCat.bmp demo0_folder/ResKimberly.bmp
	diff demo0_folder/KimberlyNCat.bmp demo0_folder/ResKimberly.bmp

demo1:
	./encoder.exe 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	./decoder.exe 1 QResKimberly.bmp KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw
	@mkdir -p demo1_a_folder
	@cp *.txt demo1_a_folder
	@cp *.raw demo1_a_folder
	@mv QResKimberly.bmp demo1_a_folder
	@cp KimberlyNCat.bmp demo1_a_folder
	./decoder.exe 1 ResKimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	@mkdir -p demo1_b_folder
	@mv *.txt demo1_b_folder
	@mv *.raw demo1_b_folder
	@mv ResKimberly.bmp demo1_b_folder
	@cp KimberlyNCat.bmp demo1_b_folder
	cmp -l demo1_b_folder/KimberlyNCat.bmp demo1_b_folder/ResKimberly.bmp
	diff demo1_b_folder/KimberlyNCat.bmp demo1_b_folder/ResKimberly.bmp

demo2:
	./encoder.exe 2 KimberlyNCat.bmp ascii rle_code.txt
	./decoder.exe 2 QResKimberly.bmp ascii rle_code.txt
	@mkdir -p demo2_a_folder
	@mv *.txt demo2_a_folder
	@mv QResKimberly.bmp demo2_a_folder
	@cp KimberlyNCat.bmp demo2_a_folder
	./encoder.exe 2 KimberlyNCat.bmp binary rle_code.bin
	./decoder.exe 2 QResKimberly.bmp binary rle_code.bin
	@mkdir -p demo2_b_folder
	@mv *.bin demo2_b_folder
	@mv QResKimberly.bmp demo2_b_folder
	@cp KimberlyNCat.bmp demo2_b_folder
	cmp -l demo2_a_folder/QResKimberly.bmp demo1_a_folder/QResKimberly.bmp
	diff demo2_a_folder/QResKimberly.bmp demo1_a_folder/QResKimberly.bmp
	cmp -l demo2_b_folder/QResKimberly.bmp demo1_a_folder/QResKimberly.bmp
	diff demo2_b_folder/QResKimberly.bmp demo1_a_folder/QResKimberly.bmp

demo3:
	./encoder.exe 3 KimberlyNCat.bmp ascii codebook.txt huffman_code.txt
	./decoder.exe 3 QResKimberly.bmp ascii codebook.txt huffman_code.txt
	./encoder.exe 3 Kimberly.bmp binary codebook.txt huffman_code.bin
	./decoder.exe 3 QResKimberly.bmp binary codebook.txt huffman_code.bin

clean:
	@rm -rf demo2_b_folder demo2_a_folder demo1_b_folder demo1_a_folder demo0_folder encoder.exe decoder.exe QResKimberly.bmp ResKimberly.bmp

.PHONY: build demo0 demo1 clean demo2 demo3
