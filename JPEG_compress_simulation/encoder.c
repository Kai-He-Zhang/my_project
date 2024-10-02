#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
float PI = 3.14159265359;

const int DCT_tableY[8][8] = {
	{16, 11, 10, 16, 24, 40, 51, 61},
	{12, 12, 14, 19, 26, 58, 60, 55},
	{14, 13, 16, 24, 40, 57, 69, 56},
	{14, 17, 22, 29, 51, 87, 80, 62},
	{18, 22, 37, 56, 68, 109, 103, 77},
	{24, 35, 55, 64, 81, 104, 113, 92},
	{49, 64, 78, 87, 103, 121, 120, 101},
	{72, 92, 95, 98, 112, 100, 103, 99}};

const int DCT_tableC[8][8] = {
	{17, 18, 24, 47, 99, 99, 99, 99},
	{18, 21, 26, 66, 99, 99, 99, 99},
	{24, 26, 56, 99, 99, 99, 99, 99},
	{47, 66, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99}};

/*construct a structure of BMP header*/
typedef struct Bmpheader
{
	char identifier[2];		 // 0x0000
	unsigned int filesize;	 // 0x0002
	unsigned short reserved; // 0x0006
	unsigned short reserved2;
	unsigned int bitmap_dataoffset; // 0x000A
	unsigned int bitmap_headersize; // 0x000E
	unsigned int width;				// 0x0012
	unsigned int height;			// 0x0016
	unsigned short planes;			// 0x001A
	unsigned short bits_perpixel;	// 0x001C
	unsigned int compression;		// 0x001E
	unsigned int bitmap_datasize;	// 0x0022
	unsigned int hresolution;		// 0x0026
	unsigned int vresolution;		// 0x002A
	unsigned int usedcolors;		// 0x002E
	unsigned int importantcolors;	// 0x0032
	unsigned int palette;			// 0x0036
} Bitmap;

/*construct a structure of RGB*/
typedef struct RGB
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
} ImgRGB;

typedef struct YCbCr
{
	float Y;
	float Cb;
	float Cr;
} ImgYCbCr;

void readheader(FILE *fp, Bitmap *x);
ImgRGB **malloc_2D(int row, int col);
void InputData(FILE *fp, ImgRGB **array, int H, int W, int skip);
ImgYCbCr **malloc_YCbCr_2D(int row, int col);

// encoder 0
void output_size(char *file_name, int height, int width);
void output_RGB_txt(ImgRGB **RGB, char *R_name, char *G_name, char *B_name, Bitmap bmpheader, int skip);

// encoder 1
void RGB_to_YCbCr(ImgRGB **RGB, ImgYCbCr **YCbCr, int H, int W);
ImgYCbCr **YCbCr_DCT(ImgYCbCr **YCbCr, int H, int W);
ImgYCbCr **Quantization(ImgYCbCr **YCbCr, int block_H, int block_W);
ImgYCbCr **Quantization_Error(ImgYCbCr **Q_YCbCr, ImgYCbCr **YCbCr, int block_H, int block_W);
void Write_table(char *QTY, char *QTCb, char *QTCr);
void Witre_Quantiztion_Value(char *Y_raw, char *Cb_raw, char *Cr_raw, ImgYCbCr **YCbCr, int block_H, int block_W);
void Write_Quantization_Error(char *eY_raw, char *eCb_raw, char *eCr_raw, ImgYCbCr **EorQ_YCbCr, int block_H, int block_W);
void YCbCr_SQNR(ImgYCbCr **YCbCr, ImgYCbCr **EorQ_YCbCr, int block_H, int block_W);

// encoder 2
ImgYCbCr **DPCM(ImgYCbCr **Q_YCbCr, int block_H, int block_W);
void ZigZag(ImgYCbCr **DQ_YCbCr, ImgYCbCr **ZZ_YCbCr, int block_H, int block_W);
void RLE_ascii(ImgYCbCr **ZZ_YCbCr, int block_H, int block_W, char output_name[13], int H, int W);
void RLE_binary(ImgYCbCr **ZZ_YCbCr, int block_H, int block_W, char output_name[13], int H, int W);

// encoder 3

int main(int argc, char **argv)
{
	int i, j, x, y;
	FILE *fp_in;

	int condition = atoi(argv[1]);

	if (condition == 0)
	{
		printf("Enoder 0:\n");
		if (argc != 7)
		{
			printf("參數錯誤!!!請執行 ./encoder 0 Kimberly.bmp R.txt G.txt B.txt dim.txt\n");
			return 1;
		}
		char *fn_in = argv[2];
		char *R_out = argv[3];
		char *G_out = argv[4];
		char *B_out = argv[5];
		char *dim_out = argv[6];

		fp_in = fopen(fn_in, "rb");
		if (fp_in)
			printf("Open %s as input file!\n", fn_in);
		else
			printf("Fail to open %s as input file!\n", fn_in);

		// read header
		Bitmap bmpheader;
		readheader(fp_in, &bmpheader);
		// struct Bmpheader bmpheader

		// W:3024 x H:4032 for input.bmp
		int H = bmpheader.height;
		int W = bmpheader.width;

		// write size
		output_size(dim_out, H, W);

		// skip paddings at the end of each image line
		int skip = (4 - (bmpheader.width * 3) % 4);
		if (skip == 4)
			skip = 0;

		// 8x8, if not multiples of 8, then bitmap padded, i.e. one more block
		int block_H = H / 8;
		if (H % 8 != 0)
			block_H++;
		int block_W = W / 8;
		if (W % 8 != 0)
			block_W++;

		printf("Image size: Width=%d x Height=%d pixels, total %d bytes\n", W, H, W * H * 3);
		printf("Image line skip=%d bytes\n", skip);
		printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W * block_H);

		// 讀取圖片
		printf("\nLoading BMP data: RRGGBB RRGGBB ... (BGR format in BMP file)\n");
		ImgRGB **Data_RGB = malloc_2D(bmpheader.height, bmpheader.width);
		InputData(fp_in, Data_RGB, bmpheader.height, bmpheader.width, skip);
		fclose(fp_in);
		printf("Data size loaded: %d bytes\n", H * W * 3);

		// 寫入圖片
		printf("\nWrite RGB data.......\n");
		output_RGB_txt(Data_RGB, R_out, G_out, B_out, bmpheader, skip);
		free(Data_RGB[0]);
		free(Data_RGB);
		printf("Job done!\n\n");
	}
	else if (condition == 1)
	{
		printf("Enoder 1:\n");
		if (argc != 13)
		{
			printf("參數錯誤!!!請執行 ./encoder 1 Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw\n");
			return 1;
		}
		char *input_img = argv[2];
		char *Y_name = argv[3];
		char *Cb_name = argv[4];
		char *Cr_name = argv[5];
		char *dim_name = argv[6];
		char *Y_raw = argv[7];
		char *Cb_raw = argv[8];
		char *Cr_raw = argv[9];
		char *eY_raw = argv[10];
		char *eCb_raw = argv[11];
		char *eCr_raw = argv[12];

		fp_in = fopen(input_img, "rb");
		if (fp_in)
			printf("Open %s as input file!\n", input_img);
		else
			printf("Fail to open %s as input file!\n", input_img);

		Bitmap bmpheader;
		readheader(fp_in, &bmpheader);

		int H = bmpheader.height;
		int W = bmpheader.width;

		int block_H = H / 8; // compute block number
		if (H % 8 != 0)
		{
			block_H++;
		}
		int block_W = W / 8; // compute block number
		if (W % 8 != 0)
		{
			block_W++;
		}

		output_size(dim_name, H, W); // wirte size

		int skip = (4 - (bmpheader.width * 3) % 4);
		if (skip == 4)
			skip = 0;

		printf("Loading image......................\n");
		ImgRGB **Data_RGB = malloc_2D(bmpheader.height, bmpheader.width);
		InputData(fp_in, Data_RGB, bmpheader.height, bmpheader.width, skip);
		fclose(fp_in);
		printf("done!\n");

		printf("Transform RGB to YCbCr.............\n");
		ImgYCbCr **Data_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
		RGB_to_YCbCr(Data_RGB, Data_YCbCr, H, W);
		free(Data_RGB[0]);
		free(Data_RGB);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", Data_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Computing DCT for YCbCr............\n"); // F_Y, F_Cb, F_Cr
		ImgYCbCr **Data_YCbCr_dct = YCbCr_DCT(Data_YCbCr, bmpheader.height, bmpheader.width);
		printf("done!\n");

		// 觀察Block.01 YCbCr
		// printf("Block.01 Y =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Y);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// printf("Block.01 Cb =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Cr);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// printf("Block.01 Cr =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Cb);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");

		printf("Quantization YCbCr DCT DATA........\n"); // qF_Y, qF_Cb, qF_Cr
		ImgYCbCr **Q_YCbCr = Quantization(Data_YCbCr_dct, block_H, block_W);
		printf("done!\n");

		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Q_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Computing Quantization Error.......\n");
		ImgYCbCr **EorQ_YCbCr = Quantization_Error(Q_YCbCr, Data_YCbCr_dct, block_H, block_W);
		printf("done!\n");

		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", EorQ_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Write Quantization table...........\n");
		Write_table(Y_name, Cb_name, Cr_name);
		printf("done!\n");

		printf("Write Quantization YCbCr value.....\n");
		Witre_Quantiztion_Value(Y_raw, Cb_raw, Cr_raw, Q_YCbCr, block_H, block_W);
		printf("done!\n");

		printf("Write Quantization Error value.....\n");
		Write_Quantization_Error(eY_raw, eCb_raw, eCr_raw, EorQ_YCbCr, block_H, block_W);
		printf("done!\n");

		printf("Compute YCbCr SQNR.................\n");
		YCbCr_SQNR(Data_YCbCr_dct, EorQ_YCbCr, block_H, block_W);
		printf("done!\n");

		free(Data_YCbCr[0]);
		free(Data_YCbCr);
		free(Data_YCbCr_dct[0]);
		free(Data_YCbCr_dct);
		free(Q_YCbCr[0]);
		free(Q_YCbCr);
		free(EorQ_YCbCr[0]);
		free(EorQ_YCbCr);

		printf("\nEncoder 1 finish!!!!!\n\n");
	}
	else if (condition == 2)
	{
		printf("Enoder 2:\n");
		if (argc != 5)
		{
			printf("參數錯誤!!!請執行(a) ./encoder 2 KimberlyNCat.bmp ascii rle_code.txt\n");
			printf("          請執行(b) ./encoder 2 KimberlyNCat.bmp binary rle_code.bin\n");
		}
		char *input_bmp = argv[2];
		char *write_mode = argv[3];
		char output_name[13];
		if (strcmp(write_mode, "ascii") == 0)
		{
			strcpy(output_name, argv[4]);
		}
		else if (strcmp(write_mode, "binary") == 0)
		{
			strcpy(output_name, argv[4]);
		}

		fp_in = fopen(input_bmp, "rb");
		if (fp_in)
			printf("Open %s as input file!\n", input_bmp);
		else
			printf("Fail to open %s as input file!\n", input_bmp);

		Bitmap bmpheader;
		readheader(fp_in, &bmpheader);

		int H = bmpheader.height;
		int W = bmpheader.width;
		// printf("%d, %d\n", H, W);

		int block_H = H / 8; // compute block number
		if (H % 8 != 0)
		{
			block_H++;
		}
		int block_W = W / 8; // compute block number
		if (W % 8 != 0)
		{
			block_W++;
		}

		int skip = (4 - (bmpheader.width * 3) % 4);
		if (skip == 4)
			skip = 0;

		printf("Loading image......................\n");
		ImgRGB **Data_RGB = malloc_2D(bmpheader.height, bmpheader.width);
		InputData(fp_in, Data_RGB, bmpheader.height, bmpheader.width, skip);
		fclose(fp_in);
		printf("done!\n");

		printf("Transform RGB to YCbCr.............\n");
		ImgYCbCr **Data_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
		RGB_to_YCbCr(Data_RGB, Data_YCbCr, H, W);
		free(Data_RGB[0]);
		free(Data_RGB);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", Data_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Computing DCT for YCbCr............\n"); // F_Y, F_Cb, F_Cr
		ImgYCbCr **Data_YCbCr_dct = YCbCr_DCT(Data_YCbCr, bmpheader.height, bmpheader.width);
		printf("done!\n");

		// 觀察Block.01 YCbCr
		// printf("Block.01 Y =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Y);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// printf("Block.01 Cb =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Cr);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// printf("Block.01 Cr =\n");
		// for(int i = 0; i < 8; i++){
		// 	for(int j = 0; j < 8; j++){
		// 		printf("%f ", Data_YCbCr_dct[i][j].Cb);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");

		printf("Quantization YCbCr DCT DATA........\n"); // qF_Y, qF_Cb, qF_Cr
		ImgYCbCr **Q_YCbCr = Quantization(Data_YCbCr_dct, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 16; i++)
		// {
		// 	for (int j = 0; j < 16; j++)
		// 	{
		// 		printf("%f ", Q_YCbCr[48 * 8 + i][48 * 8 + j].Y);
		// 	}
		// 	printf("\n");
		// }

		free(Data_YCbCr[0]);
		free(Data_YCbCr);
		free(Data_YCbCr_dct[0]);
		free(Data_YCbCr_dct);

		printf("DPCM Quantization YCbCr DCT DATA...\n");
		ImgYCbCr **DQ_YCbCr = DPCM(Q_YCbCr, block_H, block_W);
		printf("done!\n");

		// for(int i = 0; i < 16; i++){
		// 	for(int j = 0; j < 16; j++){
		// 		printf("%f ", DQ_YCbCr[i][j].Cb);
		// 	}
		// 	printf("\n");
		// }

		free(Q_YCbCr[0]);
		free(Q_YCbCr);

		printf("ZigZag ordering....................\n");
		ImgYCbCr **ZZ_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
		ZigZag(DQ_YCbCr, ZZ_YCbCr, block_H, block_W);
		printf("done!\n");

		// for(int i = 8; i < 16; i++){
		// 	for(int j = 8; j < 16; j++){
		// 		printf("%f ", DQ_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// for(int i = 8; i < 16; i++){
		// 	for(int j = 8; j < 16; j++){
		// 		printf("%f ", ZZ_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		if(strcmp(write_mode, "ascii") == 0){
			printf("Run length encoding by ascii.......\n");
			RLE_ascii(ZZ_YCbCr, block_H, block_W, output_name, H, W);
			printf("done!\n");
		}
		else if(strcmp(write_mode, "binary") == 0){
			printf("Run length encoding by binary......\n");
			RLE_binary(ZZ_YCbCr, block_H, block_W, output_name, H, W);
			printf("done!\n");
		}

		printf("Encoder 2 finish!!!!!\n\n");
	}
	else if (condition == 3)
	{
		if (argc != 6)
		{
			printf("參數錯誤!!!請執行(a) ./encoder 3 Kimberly.bmp ascii codebook.txt huffman_code.txt\n");
			printf("          請執行(b) ./encoder 3 Kimberly.bmp binary codebook.txt huffman_code.bin\n");
		}
		printf("Enoder 3:\n");
		printf("Job done!\n");
	}
	else
	{
		printf("Condition input error!!! The range is [0, 3].\n");
	}
	return 0;
}

/*read header*/
void readheader(FILE *fp, Bitmap *x)
{
	fread(&x->identifier, sizeof(x->identifier), 1, fp);
	fread(&x->filesize, sizeof(x->filesize), 1, fp);
	fread(&x->reserved, sizeof(x->reserved), 1, fp);
	fread(&x->reserved2, sizeof(x->reserved2), 1, fp);
	fread(&x->bitmap_dataoffset, sizeof(x->bitmap_dataoffset), 1, fp);
	fread(&x->bitmap_headersize, sizeof(x->bitmap_headersize), 1, fp);
	fread(&x->width, sizeof(x->width), 1, fp);
	fread(&x->height, sizeof(x->height), 1, fp);
	fread(&x->planes, sizeof(x->planes), 1, fp);
	fread(&x->bits_perpixel, sizeof(x->bits_perpixel), 1, fp);
	fread(&x->compression, sizeof(x->compression), 1, fp);
	fread(&x->bitmap_datasize, sizeof(x->bitmap_datasize), 1, fp);
	fread(&x->hresolution, sizeof(x->hresolution), 1, fp);
	fread(&x->vresolution, sizeof(x->vresolution), 1, fp);
	fread(&x->usedcolors, sizeof(x->usedcolors), 1, fp);
	fread(&x->importantcolors, sizeof(x->importantcolors), 1, fp);
}

/* A function of making two dimensions memory locate array*/
ImgRGB **malloc_2D(int row, int col)
{
	ImgRGB **Array, *Data;
	int i;
	Array = (ImgRGB **)malloc(row * sizeof(ImgRGB *));
	Data = (ImgRGB *)malloc(row * col * sizeof(ImgRGB));
	for (i = 0; i < row; i++, Data += col)
	{
		Array[i] = Data;
	}
	return Array;
}
ImgYCbCr **malloc_YCbCr_2D(int row, int col)
{
	ImgYCbCr **Array, *Data;
	int i;
	Array = (ImgYCbCr **)malloc(row * sizeof(ImgYCbCr *));
	Data = (ImgYCbCr *)malloc(row * col * sizeof(ImgYCbCr));
	for (i = 0; i < row; i++, Data += col)
	{
		Array[i] = Data;
	}
	return Array;
}

/*input data without header into RGB structure*/
void InputData(FILE *fp, ImgRGB **array, int H, int W, int skip)
{
	int temp;
	char skip_buf[3];
	int i, j;
	for (i = 0; i < H; i++)
	{
		for (j = 0; j < W; j++)
		{
			temp = fgetc(fp);
			array[i][j].B = temp;
			temp = fgetc(fp);
			array[i][j].G = temp;
			temp = fgetc(fp);
			array[i][j].R = temp;
		}
		if (skip != 0)
			fread(skip_buf, skip, 1, fp);
	}
}

// encoder 0
void output_RGB_txt(ImgRGB **RGB, char *R_name, char *G_name, char *B_name, Bitmap bmpheader, int skip)
{
	int x, y;
	FILE *R_fp = fopen(R_name, "w");
	FILE *G_fp = fopen(G_name, "w");
	FILE *B_fp = fopen(B_name, "w");
	for (x = 0; x < bmpheader.height; x++)
	{
		for (y = 0; y < bmpheader.width; y++)
		{
			fprintf(B_fp, "%d ", RGB[x][y].B);
			fprintf(G_fp, "%d ", RGB[x][y].G);
			fprintf(R_fp, "%d ", RGB[x][y].R);
		}
		if (skip != 0)
		{
			fprintf(B_fp, "0");
			fprintf(G_fp, "0");
			fprintf(R_fp, "0");
		}
		fprintf(B_fp, "\n");
		fprintf(G_fp, "\n");
		fprintf(R_fp, "\n");
	}
	fclose(R_fp);
	fclose(G_fp);
	fclose(B_fp);
}

void output_size(char *file_name, int height, int width)
{
	FILE *fp = fopen(file_name, "w");
	fprintf(fp, "%d ", height);
	fprintf(fp, "%d ", width);
	fclose(fp);
}

// encoder 1
void RGB_to_YCbCr(ImgRGB **RGB, ImgYCbCr **YCbCr, int H, int W)
{
	int i, j;
	for (i = 0; i < H; i++)
	{
		for (j = 0; j < W; j++)
		{
			float R = RGB[i][j].R;
			float G = RGB[i][j].G;
			float B = RGB[i][j].B;
			YCbCr[i][j].Y = 0.299 * R + 0.587 * G + 0.114 * B;
			YCbCr[i][j].Cb = -0.168736 * R - 0.33124 * G + 0.5 * B + 128;
			YCbCr[i][j].Cr = 0.5 * R - 0.418688 * G - 0.081312 * B + 128;
		}
	}
}

ImgYCbCr **YCbCr_DCT(ImgYCbCr **YCbCr, int H, int W)
{
	int row, col, i, j, u, v;
	float sumY, sumCb, sumCr;
	float cu, cv;
	int block_H = H / 8;
	if (H % 8 != 0)
	{
		block_H++;
	}
	int block_W = W / 8;
	if (W % 8 != 0)
	{
		block_W++;
	}

	ImgYCbCr **DCT_result = malloc_YCbCr_2D(block_H * 8, block_W * 8);

	// compute DCT
	for (row = 0; row < block_H; row++)
	{
		for (col = 0; col < block_W; col++)
		{
			for (u = 0; u < 8; u++)
			{
				for (v = 0; v < 8; v++)
				{
					sumY = 0;
					sumCb = 0;
					sumCr = 0;
					cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
					cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;
					for (i = 0; i < 8; i++)
					{
						for (j = 0; j < 8; j++)
						{
							sumY += YCbCr[i + row * 8][j + col * 8].Y * cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16);
							sumCb += YCbCr[i + row * 8][j + col * 8].Cb * cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16);
							sumCr += YCbCr[i + row * 8][j + col * 8].Cr * cos((2 * i + 1) * u * PI / 16) * cos((2 * j + 1) * v * PI / 16);
						}
					}
					DCT_result[u + row * 8][v + col * 8].Y = 0.25 * cu * cv * sumY;
					DCT_result[u + row * 8][v + col * 8].Cb = 0.25 * cu * cv * sumCb;
					DCT_result[u + row * 8][v + col * 8].Cr = 0.25 * cu * cv * sumCr;
				}
			}
		}
	}
	return DCT_result;
}

ImgYCbCr **Quantization(ImgYCbCr **YCbCr, int block_H, int block_W)
{
	ImgYCbCr **Q_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	int i, j, x, y;
	for (i = 0; i < block_H; i++)
	{
		for (j = 0; j < block_W; j++)
		{
			for (x = 0; x < 8; x++)
			{
				for (y = 0; y < 8; y++)
				{
					Q_YCbCr[i * 8 + x][j * 8 + y].Y = round(YCbCr[i * 8 + x][j * 8 + y].Y / DCT_tableY[x][y]);
					Q_YCbCr[i * 8 + x][j * 8 + y].Cr = round(YCbCr[i * 8 + x][j * 8 + y].Cr / DCT_tableC[x][y]);
					Q_YCbCr[i * 8 + x][j * 8 + y].Cb = round(YCbCr[i * 8 + x][j * 8 + y].Cb / DCT_tableC[x][y]);
				}
			}
		}
	}
	return Q_YCbCr;
}

ImgYCbCr **Quantization_Error(ImgYCbCr **Q_YCbCr, ImgYCbCr **YCbCr, int block_H, int block_W)
{
	ImgYCbCr **EorQ_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	int i, j, x, y;
	for (i = 0; i < block_H; i++)
	{
		for (j = 0; j < block_W; j++)
		{
			for (x = 0; x < 8; x++)
			{
				for (y = 0; y < 8; y++)
				{
					EorQ_YCbCr[i * 8 + x][j * 8 + y].Y = YCbCr[i * 8 + x][j * 8 + y].Y - Q_YCbCr[i * 8 + x][j * 8 + y].Y * DCT_tableY[x][y];
					EorQ_YCbCr[i * 8 + x][j * 8 + y].Cr = YCbCr[i * 8 + x][j * 8 + y].Cr - Q_YCbCr[i * 8 + x][j * 8 + y].Cr * DCT_tableC[x][y];
					EorQ_YCbCr[i * 8 + x][j * 8 + y].Cb = YCbCr[i * 8 + x][j * 8 + y].Cb - Q_YCbCr[i * 8 + x][j * 8 + y].Cb * DCT_tableC[x][y];
				}
			}
		}
	}
	return EorQ_YCbCr;
}

void Write_table(char *QTY, char *QTCb, char *QTCr)
{
	int i, j;
	FILE *Yfp = fopen(QTY, "w");
	FILE *Cbfp = fopen(QTCb, "w");
	FILE *Crfp = fopen(QTCr, "w");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			fprintf(Yfp, "%d ", DCT_tableY[i][j]);
			fprintf(Cbfp, "%d ", DCT_tableC[i][j]);
			fprintf(Crfp, "%d ", DCT_tableC[i][j]);
		}
		fprintf(Yfp, "\n");
		fprintf(Cbfp, "\n");
		fprintf(Crfp, "\n");
	}
	fclose(Yfp);
	fclose(Cbfp);
	fclose(Crfp);
}

void Witre_Quantiztion_Value(char *Y_raw, char *Cb_raw, char *Cr_raw, ImgYCbCr **YCbCr, int block_H, int block_W)
{
	int i, j;
	short Y, Cb, Cr;
	FILE *Yfp = fopen(Y_raw, "wb");
	FILE *Cbfp = fopen(Cb_raw, "wb");
	FILE *Crfp = fopen(Cr_raw, "wb");
	for (int i = 0; i < block_H * 8; i++)
	{
		for (int j = 0; j < block_W * 8; j++)
		{
			Y = YCbCr[i][j].Y;
			Cb = YCbCr[i][j].Cb;
			Cr = YCbCr[i][j].Cr;
			fwrite(&Y, sizeof(short), 1, Yfp);
			fwrite(&Cb, sizeof(short), 1, Cbfp);
			fwrite(&Cr, sizeof(short), 1, Crfp);
		}
	}
	fclose(Yfp);
	fclose(Cbfp);
	fclose(Crfp);
}

void Write_Quantization_Error(char *eY_raw, char *eCb_raw, char *eCr_raw, ImgYCbCr **EorQ_YCbCr, int block_H, int block_W)
{
	int i, j;
	float Y, Cb, Cr;
	FILE *Yfp = fopen(eY_raw, "wb");
	FILE *Cbfp = fopen(eCb_raw, "wb");
	FILE *Crfp = fopen(eCr_raw, "wb");
	for (int i = 0; i < block_H * 8; i++)
	{
		for (int j = 0; j < block_W * 8; j++)
		{
			Y = EorQ_YCbCr[i][j].Y;
			Cb = EorQ_YCbCr[i][j].Cb;
			Cr = EorQ_YCbCr[i][j].Cr;
			fwrite(&Y, sizeof(float), 1, Yfp);
			fwrite(&Cb, sizeof(float), 1, Cbfp);
			fwrite(&Cr, sizeof(float), 1, Crfp);
		}
	}
	fclose(Yfp);
	fclose(Cbfp);
	fclose(Crfp);
}

void YCbCr_SQNR(ImgYCbCr **YCbCr, ImgYCbCr **EorQ_YCbCr, int block_H, int block_W)
{
	int i, j;
	double SQNR;
	double sq, err_sq;

	printf("\nSQNR->Y = \n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			sq = YCbCr[i][j].Y * YCbCr[i][j].Y;
			err_sq = EorQ_YCbCr[i][j].Y * EorQ_YCbCr[i][j].Y;
			SQNR = 10 * log10(sq / err_sq);
			printf("%f ", SQNR);
		}
		printf("\n");
	}

	printf("\nSQNR->Cb = \n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			sq = YCbCr[i][j].Cb * YCbCr[i][j].Cb;
			err_sq = EorQ_YCbCr[i][j].Cb * EorQ_YCbCr[i][j].Cb;
			SQNR = 10 * log10(sq / err_sq);
			printf("%f ", SQNR);
		}
		printf("\n");
	}

	printf("\nSQNR->Cr = \n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			sq = YCbCr[i][j].Cr * YCbCr[i][j].Cr;
			err_sq = EorQ_YCbCr[i][j].Cr * EorQ_YCbCr[i][j].Cr;
			SQNR = 10 * log10(sq / err_sq);
			printf("%f ", SQNR);
		}
		printf("\n");
	}
}

// encoder 2

ImgYCbCr **DPCM(ImgYCbCr **Q_YCbCr, int block_H, int block_W)
{
	ImgYCbCr **DQ_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	int row, col, i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			DQ_YCbCr[i][j].Y = Q_YCbCr[i][j].Y;
			DQ_YCbCr[i][j].Cb = Q_YCbCr[i][j].Cb;
			DQ_YCbCr[i][j].Cr = Q_YCbCr[i][j].Cr;
		}
	}

	for (row = 0; row < block_H; row++)
	{
		for (col = 0; col < block_W; col++)
		{
			if (row == 0 && col == 0)
				continue;
			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (i == 0 && j == 0)
					{
						if (col == 0) // 第一個col調整減上個row的最後一個col
						{
							DQ_YCbCr[row * 8][col * 8].Y = Q_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Y - Q_YCbCr[row * 8][col * 8].Y;
							DQ_YCbCr[row * 8][col * 8].Cb = Q_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Cb - Q_YCbCr[row * 8][col * 8].Cb;
							DQ_YCbCr[row * 8][col * 8].Cr = Q_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Cr - Q_YCbCr[row * 8][col * 8].Cr;
						}
						else // 前一個減掉現在的
						{
							DQ_YCbCr[row * 8][col * 8].Y = Q_YCbCr[row * 8][(col - 1) * 8].Y - Q_YCbCr[row * 8][col * 8].Y;
							DQ_YCbCr[row * 8][col * 8].Cb = Q_YCbCr[row * 8][(col - 1) * 8].Cb - Q_YCbCr[row * 8][col * 8].Cb;
							DQ_YCbCr[row * 8][col * 8].Cr = Q_YCbCr[row * 8][(col - 1) * 8].Cr - Q_YCbCr[row * 8][col * 8].Cr;
						}
					}
					else // 其餘一樣
					{
						DQ_YCbCr[row * 8 + i][col * 8 + j].Y = Q_YCbCr[row * 8 + i][col * 8 + j].Y;
						DQ_YCbCr[row * 8 + i][col * 8 + j].Cb = Q_YCbCr[row * 8 + i][col * 8 + j].Cb;
						DQ_YCbCr[row * 8 + i][col * 8 + j].Cr = Q_YCbCr[row * 8 + i][col * 8 + j].Cr;
					}
				}
			}
		}
	}

	return DQ_YCbCr;
}

void ZigZag(ImgYCbCr **DQ_YCbCr, ImgYCbCr **ZZ_YCbCr, int block_H, int block_W)
{
	float inputy[8][8] = {0};
	float inputcb[8][8] = {0};
	float inputcr[8][8] = {0};
	float outputy[64] = {0};
	float outputcb[64] = {0};
	float outputcr[64] = {0};
	int row = 0, col = 0;
	int index = 0;
	int numRows = 8;
	int numCols = 8;

	// for(int i = 0; i < block_H * 8; i++){
	// 	for(int j = 0; j < block_W * 8; j++){
	// 		ZZ_YCbCr[i][j].Y = DQ_YCbCr[i][j].Y;
	// 		ZZ_YCbCr[i][j].Cr = DQ_YCbCr[i][j].Cb;
	// 		ZZ_YCbCr[i][j].Cb = DQ_YCbCr[i][j].Cr;
	// 	}
	// }
	
	for (int x = 0; x < block_H; x++)
	{
		for (int y = 0; y < block_W; y++)
		{
			row = 0, col = 0;
			index = 0;
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					inputy[i][j] = DQ_YCbCr[x * 8 + i][y * 8 + j].Y;
					inputcb[i][j] = DQ_YCbCr[x * 8 + i][y * 8 + j].Cb;
					inputcr[i][j] = DQ_YCbCr[x * 8 + i][y * 8 + j].Cr;
				}
			}

			// zigzag
			while (row < numRows && col < numCols)
			{
				outputy[index] = inputy[row][col];
				outputcb[index] = inputcb[row][col];
				outputcr[index] = inputcr[row][col];
				index++;
				if ((row + col) % 2 == 0)
				{ // Even sum means moving up
					if (col == numCols - 1)
					{
						row++;
					}
					else if (row == 0)
					{
						col++;
					}
					else
					{
						row--;
						col++;
					}
				}
				else
				{ // Odd sum means moving down
					if (row == numRows - 1)
					{
						col++;
					}
					else if (col == 0)
					{
						row++;
					}
					else
					{
						row++;
						col--;
					}
				}
			}

			// printf("\nZigzag Order:\n");
			// for (int i = 0; i < 64; i++)
			// {
			// 	printf("%f ", outputy[i]);
			// }
			// printf("\n");

			int counter = 0;
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					ZZ_YCbCr[x * 8 + i][y * 8 + j].Y = outputy[counter];
					ZZ_YCbCr[x * 8 + i][y * 8 + j].Cb = outputcb[counter];
					ZZ_YCbCr[x * 8 + i][y * 8 + j].Cr = outputcr[counter];
					counter++;
				}
			}
		}
	}
}

void RLE_ascii(ImgYCbCr **ZZ_YCbCr, int block_H, int block_W, char output_name[13], int H, int W){
	FILE *fp = fopen(output_name, "w");
	int row, col, i, j;
	int Y, Cb, Cr;
	// dim r c
	// block row col zn n zn n zn n ..... E
	//
	//
	fprintf(fp, "%d ", H);
	fprintf(fp, "%d\n", W);
	for(row = 0; row < block_H; row++){
		for(col = 0; col < block_W; col++){
			int counter = 0;
			fprintf(fp, "%d ", row);
			fprintf(fp, "%d ", col);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Y = ZZ_YCbCr[row * 8 + i][col * 8 + j].Y;
					if(Y == 0){
						counter++;
					}
					else if(Y != 0){
						fprintf(fp, "%d %d ", counter, Y);
						counter = 0;
					}
				}
			}
			fprintf(fp, "%d%d%d\n", 0, 0, 0);
			counter = 0;

			fprintf(fp, "%d ", row);
			fprintf(fp, "%d ", col);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Cb = ZZ_YCbCr[row * 8 + i][col * 8 + j].Cb;
					if(Cb == 0){
						counter++;
					}
					else if(Cb != 0){
						fprintf(fp, "%d %d ", counter, Cb);
						counter = 0;
					}
				}
			}
			fprintf(fp, "%d%d%d\n", 0, 0, 0);
			counter = 0;

			fprintf(fp, "%d ", row);
			fprintf(fp, "%d ", col);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Cr = ZZ_YCbCr[row * 8 + i][col * 8 + j].Cr;
					if(Cr == 0){
						counter++;
					}
					else if(Cr != 0){
						fprintf(fp, "%d %d ", counter, Cr);
						counter = 0;
					}
				}
			}
			fprintf(fp, "%d%d%d\n", 0, 0, 0);	
			counter = 0;	
		}
	}
	fclose(fp);
}

void RLE_binary(ImgYCbCr **ZZ_YCbCr, int block_H, int block_W, char output_name[13], int H, int W){
	FILE *fp = fopen(output_name, "wb");
	int row, col, i, j;
	short Y, Cb, Cr;
	// dim r c
	// block row col zn n zn n zn n ..... E
	//
	//
	short h = H;
	short w = W;
	short end = 3003;
	fwrite(&h, sizeof(short), 1, fp);
	fwrite(&w, sizeof(short), 1, fp);
	fwrite(&end, sizeof(short), 1, fp);
	for(row = 0; row < block_H; row++){
		for(col = 0; col < block_W; col++){
			short counter = 0;
			fwrite(&row, sizeof(short), 1, fp);
			fwrite(&col, sizeof(short), 1, fp);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Y = ZZ_YCbCr[row * 8 + i][col * 8 + j].Y;
					if(Y == 0){
						counter++;
					}
					else if(Y != 0){
						fwrite(&counter, sizeof(short), 1, fp);
						fwrite(&Y, sizeof(short), 1, fp);
						counter = 0;
					}
				}
			}

			fwrite(&end, sizeof(short), 1, fp);
			counter = 0;

			fwrite(&row, sizeof(short), 1, fp);
			fwrite(&col, sizeof(short), 1, fp);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Cb = ZZ_YCbCr[row * 8 + i][col * 8 + j].Cb;
					if(Cb == 0){
						counter++;
					}
					else if(Cb != 0){
						fwrite(&counter, sizeof(short), 1, fp);
						fwrite(&Cb, sizeof(short), 1, fp);
						counter = 0;
					}
				}
			}
			fwrite(&end, sizeof(short), 1, fp);
			counter = 0;

			fwrite(&row, sizeof(short), 1, fp);
			fwrite(&col, sizeof(short), 1, fp);
			for(i = 0; i < 8; i++){
				for(j = 0; j < 8; j++){
					Cr = ZZ_YCbCr[row * 8 + i][col * 8 + j].Cr;
					if(Cr == 0){
						counter++;
					}
					else if(Cr != 0){
						fwrite(&counter, sizeof(short), 1, fp);
						fwrite(&Cr, sizeof(short), 1, fp);
						counter = 0;
					}
				}
			}
			fwrite(&end, sizeof(short), 1, fp);	
			counter = 0;	
		}
	}
	fclose(fp);
}

// encoder 3

