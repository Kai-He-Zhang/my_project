#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
float PI = 3.14159265359;

int zz_matrix[8][8] = {
	{1, 2, 6, 7, 15, 16, 28, 29},
	{3, 5, 8, 14, 17, 27, 30, 43},
	{4, 9, 13, 18, 26, 31, 42, 44},
	{10, 12, 19, 25, 32, 41, 45, 54},
	{11, 20, 24, 33, 40, 46, 53, 55},
	{21, 23, 34, 39, 47, 52, 56, 61},
	{22, 35, 38, 48, 51, 57, 60, 62},
	{36, 37, 49, 50, 58, 59, 63, 64}};

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

void read_size(char *file_name, int *size);
void output_bmp(ImgRGB **RGB, FILE *outfile, Bitmap bmpheader, int skip);
void InputData(char *R_name, char *G_name, char *B_name, ImgRGB **array, int H, int W, int skip);
ImgRGB **malloc_2D(int row, int col);
ImgYCbCr **malloc_YCbCr_2D(int row, int col);
Bitmap create_header(int *size);

// decoder 1
void Read_Qtable(char *Qtable_Y, char *Qtable_Cb, char *Qtable_Cr, int QY_table[8][8], int QC_table[8][8]);
void Read_Quantization(ImgYCbCr **YCbCr_Data, char *QFY_in, char *QFCb_in, char *QFCr_in, int block_H, int block_W);
void Read_EorQ_YCbCr(ImgYCbCr **EorQ_YCbCr_Data, char *EFY_in, char *EFCb_in, char *EFCr_in, int block_H, int block_W);
ImgYCbCr **Recons_Qvalue(ImgYCbCr **YCbCr_Data, int QY_table[8][8], int QC_table[8][8], int block_H, int block_W);
ImgYCbCr **Recons_EorQvalue(ImgYCbCr **YCbCr_Data, ImgYCbCr **EorQ_YCbCr_Data, int QY_table[8][8], int QC_table[8][8], int block_H, int block_W);
ImgYCbCr **IDCT(ImgYCbCr **RQ_YCbCr_data, int block_H, int block_W);
ImgRGB **YCbCr_to_RGB(ImgYCbCr **recons_YCbCr, int height, int width);
void RGB_SQNR(ImgRGB **recons_RGB, char *origin_bmp);
void Read_img(FILE *fp, ImgRGB **array, int H, int W, int skip);
void readheader(FILE *fp, Bitmap *x);

// decoder 2
ImgYCbCr **Read_ascii_YCbCr(char input_name[13], int size[2]);
ImgYCbCr **Read_binary_YCbCr(char input_name[13], int size[2]);
ImgYCbCr **Inverse_zigzag(ImgYCbCr **YCbCr, int block_H, int block_W);
void ZZ_find(int pos[2], int i);
ImgYCbCr **Recover_DPCM(ImgYCbCr **IZZ_YCbCr, int block_H, int block_W);

int main(int argc, char **argv)
{
	int condition = atoi(argv[1]);

	if (condition == 0)
	{
		printf("Decoder 0:\n");
		if (argc != 7)
		{
			printf("參數錯誤!!!請執行 ./decoder 0 ResKimberly.bmp R.txt G.txt B.txt dim.txt\n");
			return 1;
		}
		char *dim_txt = argv[6];
		char *R_out = argv[3];
		char *G_out = argv[4];
		char *B_out = argv[5];
		char *out_name = argv[2];
		int img_size[2] = {0};

		// 讀取圖片大小
		printf("Read image size");
		read_size(dim_txt, img_size);
		printf(" height = %d width = %d\n", img_size[0], img_size[1]);

		int skip = (4 - (img_size[1] * 3) % 4);
		if (skip == 4)
			skip = 0;

		// 讀取圖片
		ImgRGB **Data_RGB = malloc_2D(img_size[0], img_size[1]); // 0->height, 1->width
		printf("\nLoading image.........\n");
		InputData(R_out, G_out, B_out, Data_RGB, img_size[0], img_size[1], skip);
		printf("done!\n");

		// 寫入圖片
		printf("\nWrite image...........\n");
		Bitmap out_header = create_header(img_size);
		FILE *out_file = fopen(out_name, "wb");
		output_bmp(Data_RGB, out_file, out_header, skip);
		fclose(out_file);
		printf("done!\n");
		printf("\n");
	}
	else if (condition == 1)
	{
		char *recons_bmp;
		char *origin_bmp;
		char *Qtable_Y;
		char *Qtable_Cb;
		char *Qtable_Cr;
		char *dim_in;
		char *QFY_in;
		char *QFCb_in;
		char *QFCr_in;
		char *EFY_in;
		char *EFCb_in;
		char *EFCr_in;
		printf("Decoder 1:\n");
		if (argc != 11 && argc != 13)
		{
			printf("參數錯誤!!!請執行(a) ./decoder 1 QResKimberly.bmp Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw\n");
			printf("          請執行(b) ./decoder 1 ResKimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw\n");
			return 1;
		}
		if (argc == 11)
		{
			recons_bmp = argv[2];
			origin_bmp = argv[3];
			Qtable_Y = argv[4];
			Qtable_Cb = argv[5];
			Qtable_Cr = argv[6];
			dim_in = argv[7];
			QFY_in = argv[8];
			QFCb_in = argv[9];
			QFCr_in = argv[10];
		}
		else if (argc == 13)
		{
			recons_bmp = argv[2];
			Qtable_Y = argv[3];
			Qtable_Cb = argv[4];
			Qtable_Cr = argv[5];
			dim_in = argv[6];
			QFY_in = argv[7];
			QFCb_in = argv[8];
			QFCr_in = argv[9];
			EFY_in = argv[10];
			EFCb_in = argv[11];
			EFCr_in = argv[12];
		}

		int size[1] = {};
		read_size(dim_in, size);
		// printf("%d, %d\n", size[0], size[1]);
		int H = size[0];
		int W = size[1];
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

		int QY_table[8][8] = {};
		int QC_table[8][8] = {};
		printf("Read Quantization table...............................\n");
		Read_Qtable(Qtable_Y, Qtable_Cb, Qtable_Cr, QY_table, QC_table);
		printf("done!\n");

		printf("Read Quantization YCbCr value.........................\n");
		ImgYCbCr **YCbCr_Data = malloc_YCbCr_2D(block_H * 8, block_W * 8);
		Read_Quantization(YCbCr_Data, QFY_in, QFCb_in, QFCr_in, block_H, block_W);
		printf("done!\n");

		ImgYCbCr **EorQ_YCbCr_Data = malloc_YCbCr_2D(block_H * 8, block_W * 8);
		if (argc == 13)
		{
			printf("Read Quantization Error value.....................\n");
			Read_EorQ_YCbCr(EorQ_YCbCr_Data, EFY_in, EFCb_in, EFCr_in, block_H, block_W);
			printf("done!\n");
		}

		ImgYCbCr **RQ_YCbCr_data;
		if (argc == 11)
		{
			printf("Reconstruction Quantization value.................\n");
			RQ_YCbCr_data = Recons_Qvalue(YCbCr_Data, QY_table, QC_table, block_H, block_W);
			printf("done!\n");
		}
		else if (argc == 13)
		{
			printf("Reconstruction Quantization value with error......\n");
			RQ_YCbCr_data = Recons_EorQvalue(YCbCr_Data, EorQ_YCbCr_Data, QY_table, QC_table, block_H, block_W);
			printf("done!\n");
		}

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", RQ_YCbCr_data[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Computing Inverse DCT.................................\n");
		ImgYCbCr **recons_YCbCr = IDCT(RQ_YCbCr_data, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", recons_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("YCbCr to RGB..........................................\n");
		ImgRGB **recons_RGB = YCbCr_to_RGB(recons_YCbCr, H, W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].R);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].G);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].B);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");

		int skip = (4 - (W * 3) % 4);
		if (skip == 4)
			skip = 0;

		printf("Write reconstruction Bitmap...........................\n");
		Bitmap out_header = create_header(size);
		FILE *out_file = fopen(recons_bmp, "wb");
		output_bmp(recons_RGB, out_file, out_header, skip);
		printf("done!\n");

		if (argc == 11)
		{
			printf("Computing RGB SQNR....................................\n");
			RGB_SQNR(recons_RGB, origin_bmp);
			printf("done!\n");
		}

		printf("\nDecoder 1 finish!!!!!!\n\n");
	}
	else if (condition == 2)
	{
		printf("Decoder 2:\n");
		if (argc != 5)
		{
			printf("參數錯誤!!!請執行(a) ./decoder 2 QResKimberly.bmp ascii rle_code.txt\n");
			printf("          請執行(b) ./decoder 2 QResKimberly.bmp binary rle_code.bin\n");
		}
		char *recons_bmp = argv[2];
		char *write_mode = argv[3];
		char input_name[13];
		if (strcmp(write_mode, "ascii") == 0)
		{
			strcpy(input_name, argv[4]);
		}
		else if (strcmp(write_mode, "binary") == 0)
		{
			strcpy(input_name, argv[4]);
		}
		int size[2] = {0};
		ImgYCbCr **recons_rle;
		if (strcmp(write_mode, "ascii") == 0)
		{
			printf("Read RLE data by ascii..........\n");
			recons_rle = Read_ascii_YCbCr(input_name, size);
		}
		else if (strcmp(write_mode, "binary") == 0)
		{
			printf("Read RLE data by binary..........\n");
			recons_rle = Read_binary_YCbCr(input_name, size);
		}
		printf("done!\n");

		// for (int i = 8; i < 16; i++)
		// {
		// 	for (int j = 8; j < 16; j++)
		// 	{
		// 		printf("%f ", recons_rle[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		int H = size[0];
		int W = size[1];
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

		printf("Inverse ZigZag.......................\n");
		ImgYCbCr **IZZ_YCbCr = Inverse_zigzag(recons_rle, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", IZZ_YCbCr[44 * 8 + i][41 * 8 + j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Reconstruction DPCM..................\n");
		ImgYCbCr **recons_DPCM = Recover_DPCM(IZZ_YCbCr, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 16; i++)
		// {
		// 	for (int j = 0; j < 16; j++)
		// 	{
		// 		printf("%f ", recons_DPCM[48 * 8 + i][48 * 8 + j].Y);
		// 	}
		// 	printf("\n");
		// }

		int DCT_tableY[8][8] = {
			{16, 11, 10, 16, 24, 40, 51, 61},
			{12, 12, 14, 19, 26, 58, 60, 55},
			{14, 13, 16, 24, 40, 57, 69, 56},
			{14, 17, 22, 29, 51, 87, 80, 62},
			{18, 22, 37, 56, 68, 109, 103, 77},
			{24, 35, 55, 64, 81, 104, 113, 92},
			{49, 64, 78, 87, 103, 121, 120, 101},
			{72, 92, 95, 98, 112, 100, 103, 99}};

		int DCT_tableC[8][8] = {
			{17, 18, 24, 47, 99, 99, 99, 99},
			{18, 21, 26, 66, 99, 99, 99, 99},
			{24, 26, 56, 99, 99, 99, 99, 99},
			{47, 66, 99, 99, 99, 99, 99, 99},
			{99, 99, 99, 99, 99, 99, 99, 99},
			{99, 99, 99, 99, 99, 99, 99, 99},
			{99, 99, 99, 99, 99, 99, 99, 99},
			{99, 99, 99, 99, 99, 99, 99, 99}};

		ImgYCbCr **RQ_YCbCr_data;
		printf("Reconstruction Quantization value.................\n");
		RQ_YCbCr_data = Recons_Qvalue(recons_DPCM, DCT_tableY, DCT_tableC, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", RQ_YCbCr_data[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("Computing Inverse DCT.................................\n");
		ImgYCbCr **recons_YCbCr = IDCT(RQ_YCbCr_data, block_H, block_W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%f ", recons_YCbCr[i][j].Y);
		// 	}
		// 	printf("\n");
		// }

		printf("YCbCr to RGB..........................................\n");
		ImgRGB **recons_RGB = YCbCr_to_RGB(recons_YCbCr, H, W);
		printf("done!\n");

		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].R);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].G);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
		// for (int i = 0; i < 8; i++)
		// {
		// 	for (int j = 0; j < 8; j++)
		// 	{
		// 		printf("%d ", recons_RGB[i][j].B);
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");

		int skip = (4 - (W * 3) % 4);
		if (skip == 4)
			skip = 0;

		printf("Write reconstruction Bitmap...........................\n");
		Bitmap out_header = create_header(size);
		FILE *out_file = fopen(recons_bmp, "wb");
		output_bmp(recons_RGB, out_file, out_header, skip);
		printf("done!\n");
	}
	else if (condition == 3)
	{
		if (argc != 6)
		{
			printf("參數錯誤!!!請執行(a) ./decoder 3 QResKimberly.bmp ascii codebook.txt huffman_code.txt\n");
			printf("          請執行(b) ./decoder 3 QResKimberly.bmp binary codebook.txt huffman_code.bin\n");
		}
		printf("Decoder 3:\n");
		printf("Job done!\n");
	}
	else
	{
		printf("Condition input error!!! The range is [0, 3].\n");
	}
	return 0;
}

Bitmap create_header(int *size)
{
	Bitmap header = {
		.identifier = "BM",
		.filesize = size[0] * size[1] * 3 + 54,
		.reserved = 0,
		.reserved2 = 0,
		.bitmap_dataoffset = 54,
		.bitmap_headersize = 40,
		.width = size[1],
		.height = size[0],
		.planes = 1,
		.bits_perpixel = 24,
		.compression = 0,
		.bitmap_datasize = size[0] * size[1] * 3,
		.hresolution = 2835,
		.vresolution = 2835,
		.usedcolors = 0,
		.importantcolors = 0};
	return header;
}
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

// decoder 0
void read_size(char *file_name, int *size)
{
	FILE *fp = fopen(file_name, "r");
	int H = 0, W = 0;
	fscanf(fp, "%d", &H);
	fscanf(fp, "%d", &W);
	size[0] = H;
	size[1] = W;
	fclose(fp);
}

void output_bmp(ImgRGB **RGB, FILE *outfile, Bitmap bmpheader, int skip)
{
	char skip_buf[3] = {0, 0, 0};
	int x, y;
	fwrite(&bmpheader.identifier, sizeof(short), 1, outfile);
	fwrite(&bmpheader.filesize, sizeof(int), 1, outfile);
	fwrite(&bmpheader.reserved, sizeof(short), 1, outfile);
	fwrite(&bmpheader.reserved2, sizeof(short), 1, outfile);
	fwrite(&bmpheader.bitmap_dataoffset, sizeof(int), 1, outfile);
	fwrite(&bmpheader.bitmap_headersize, sizeof(int), 1, outfile);
	fwrite(&bmpheader.width, sizeof(int), 1, outfile);
	fwrite(&bmpheader.height, sizeof(int), 1, outfile);
	fwrite(&bmpheader.planes, sizeof(short), 1, outfile);
	fwrite(&bmpheader.bits_perpixel, sizeof(short), 1, outfile);
	fwrite(&bmpheader.compression, sizeof(int), 1, outfile);
	fwrite(&bmpheader.bitmap_datasize, sizeof(int), 1, outfile);
	fwrite(&bmpheader.hresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader.vresolution, sizeof(int), 1, outfile);
	fwrite(&bmpheader.usedcolors, sizeof(int), 1, outfile);
	fwrite(&bmpheader.importantcolors, sizeof(int), 1, outfile);

	for (x = 0; x < bmpheader.height; x++)
	{
		for (y = 0; y < bmpheader.width; y++)
		{
			fwrite(&RGB[x][y].B, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].G, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].R, sizeof(char), 1, outfile);
		}
		if (skip != 0)
		{
			fwrite(skip_buf, skip, 1, outfile);
		}
	}
}

void InputData(char *R_name, char *G_name, char *B_name, ImgRGB **array, int H, int W, int skip)
{
	int temp;
	int i, j;
	FILE *R_fp = fopen(R_name, "r");
	FILE *G_fp = fopen(G_name, "r");
	FILE *B_fp = fopen(B_name, "r");
	for (i = 0; i < H; i++)
	{
		for (j = 0; j < W; j++)
		{
			fscanf(B_fp, "%d", &temp);
			array[i][j].B = temp;
			fscanf(G_fp, "%d", &temp);
			array[i][j].G = temp;
			fscanf(R_fp, "%d", &temp);
			array[i][j].R = temp;
		}
		if (skip != 0)
		{
			fscanf(B_fp, "%d", &temp);
			fscanf(G_fp, "%d", &temp);
			fscanf(R_fp, "%d", &temp);
		}
	}
}

// decoder 1

void Read_Qtable(char *Qtable_Y, char *Qtable_Cb, char *Qtable_Cr, int QY_table[8][8], int QC_table[8][8])
{
	FILE *Yfp = fopen(Qtable_Y, "r");
	FILE *Cbfp = fopen(Qtable_Cb, "r");

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			fscanf(Yfp, "%d", &QY_table[i][j]);
			fscanf(Cbfp, "%d", &QC_table[i][j]);
		}
	}

	// for(int i = 0; i < 8; i++){
	// 	for(int j = 0; j < 8; j++){
	// 		printf("%d ", QY_table[i][j]);
	// 	}
	// 	printf("\n");
	// }

	fclose(Yfp);
	fclose(Cbfp);
}

void Read_Quantization(ImgYCbCr **YCbCr_Data, char *QFY_in, char *QFCb_in, char *QFCr_in, int block_H, int block_W)
{
	FILE *Yfp = fopen(QFY_in, "rb");
	FILE *Cbfp = fopen(QFCb_in, "rb");
	FILE *Crfp = fopen(QFCr_in, "rb");
	short Y, Cb, Cr;
	for (int i = 0; i < block_H * 8; i++)
	{
		for (int j = 0; j < block_W * 8; j++)
		{
			fread(&Y, sizeof(short), 1, Yfp);
			fread(&Cb, sizeof(short), 1, Cbfp);
			fread(&Cr, sizeof(short), 1, Crfp);

			YCbCr_Data[i][j].Y = (float)Y;
			YCbCr_Data[i][j].Cb = (float)Cb;
			YCbCr_Data[i][j].Cr = (float)Cr;
		}
	}

	// for(int i = 0; i < 8; i++){
	// 	for(int j = 0; j < 8; j++){
	// 		printf("%f ", YCbCr_Data[i][j].Y);
	// 	}
	// 	printf("\n");
	// }

	fclose(Yfp);
	fclose(Cbfp);
	fclose(Crfp);
}

void Read_EorQ_YCbCr(ImgYCbCr **EorQ_YCbCr_Data, char *EFY_in, char *EFCb_in, char *EFCr_in, int block_H, int block_W)
{
	FILE *Yfp = fopen(EFY_in, "rb");
	FILE *Cbfp = fopen(EFCb_in, "rb");
	FILE *Crfp = fopen(EFCr_in, "rb");

	float Y, Cb, Cr;
	for (int i = 0; i < block_H * 8; i++)
	{
		for (int j = 0; j < block_W * 8; j++)
		{
			fread(&Y, sizeof(float), 1, Yfp);
			fread(&Cb, sizeof(float), 1, Cbfp);
			fread(&Cr, sizeof(float), 1, Crfp);

			EorQ_YCbCr_Data[i][j].Y = Y;
			EorQ_YCbCr_Data[i][j].Cb = Cb;
			EorQ_YCbCr_Data[i][j].Cr = Cr;
		}
	}

	// printf("nice!\n");

	// for (int i = 0; i < 8; i++)
	// {
	// 	for (int j = 0; j < 8; j++)
	// 	{
	// 		printf("%f ", EorQ_YCbCr_Data[i][j].Y);
	// 	}
	// 	printf("\n");
	// }
	fclose(Yfp);
	fclose(Cbfp);
	fclose(Crfp);
}

ImgYCbCr **Recons_Qvalue(ImgYCbCr **YCbCr_Data, int QY_table[8][8], int QC_table[8][8], int block_H, int block_W)
{
	ImgYCbCr **RQ_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);

	for (int x = 0; x < block_H; x++)
	{
		for (int y = 0; y < block_W; y++)
		{
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					RQ_YCbCr[x * 8 + i][y * 8 + j].Y = round(YCbCr_Data[x * 8 + i][y * 8 + j].Y * QY_table[i][j]);
					RQ_YCbCr[x * 8 + i][y * 8 + j].Cb = round(YCbCr_Data[x * 8 + i][y * 8 + j].Cb * QC_table[i][j]);
					RQ_YCbCr[x * 8 + i][y * 8 + j].Cr = round(YCbCr_Data[x * 8 + i][y * 8 + j].Cr * QC_table[i][j]);
				}
			}
		}
	}

	return RQ_YCbCr;
}

ImgYCbCr **Recons_EorQvalue(ImgYCbCr **YCbCr_Data, ImgYCbCr **EorQ_YCbCr_Data, int QY_table[8][8], int QC_table[8][8], int block_H, int block_W)
{
	ImgYCbCr **RQ_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);

	for (int x = 0; x < block_H; x++)
	{
		for (int y = 0; y < block_W; y++)
		{
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					RQ_YCbCr[x * 8 + i][y * 8 + j].Y = round(YCbCr_Data[x * 8 + i][y * 8 + j].Y * QY_table[i][j]) + EorQ_YCbCr_Data[x * 8 + i][y * 8 + j].Y;
					RQ_YCbCr[x * 8 + i][y * 8 + j].Cb = round(YCbCr_Data[x * 8 + i][y * 8 + j].Cb * QC_table[i][j]) + EorQ_YCbCr_Data[x * 8 + i][y * 8 + j].Cb;
					RQ_YCbCr[x * 8 + i][y * 8 + j].Cr = round(YCbCr_Data[x * 8 + i][y * 8 + j].Cr * QC_table[i][j]) + EorQ_YCbCr_Data[x * 8 + i][y * 8 + j].Cr;
				}
			}
		}
	}

	return RQ_YCbCr;
}

ImgYCbCr **IDCT(ImgYCbCr **RQ_YCbCr_data, int block_H, int block_W)
{
	ImgYCbCr **Recons_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);

	int row, col, u, v, i, j;
	float sumY, sumCb, sumCr;
	float ci, cj;
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
					for (i = 0; i < 8; i++)
					{
						for (j = 0; j < 8; j++)
						{
							ci = (i == 0) ? 1.0 / sqrt(2) : 1.0;
							cj = (j == 0) ? 1.0 / sqrt(2) : 1.0;
							sumY += ci * cj * RQ_YCbCr_data[i + row * 8][j + col * 8].Y * cos((2 * u + 1) * i * PI / 16) * cos((2 * v + 1) * j * PI / 16);
							sumCb += ci * cj * RQ_YCbCr_data[i + row * 8][j + col * 8].Cb * cos((2 * u + 1) * i * PI / 16) * cos((2 * v + 1) * j * PI / 16);
							sumCr += ci * cj * RQ_YCbCr_data[i + row * 8][j + col * 8].Cr * cos((2 * u + 1) * i * PI / 16) * cos((2 * v + 1) * j * PI / 16);
						}
					}
					Recons_YCbCr[u + row * 8][v + col * 8].Y = 0.25 * sumY;
					Recons_YCbCr[u + row * 8][v + col * 8].Cb = 0.25 * sumCb;
					Recons_YCbCr[u + row * 8][v + col * 8].Cr = 0.25 * sumCr;
				}
			}
		}
	}

	return Recons_YCbCr;
}

ImgRGB **YCbCr_to_RGB(ImgYCbCr **recons_YCbCr, int height, int width)
{
	ImgRGB **Recons_RGB = malloc_2D(height, width);
	float Y, Cb, Cr;
	float R, G, B;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Y = recons_YCbCr[i][j].Y;
			Cb = recons_YCbCr[i][j].Cb;
			Cr = recons_YCbCr[i][j].Cr;
			R = round(Y - 0.000001218894189 * (Cb - 128) + 1.401999589 * (Cr - 128));
			G = round(Y - 0.3441356782 * (Cb - 128) - 0.7141361556 * (Cr - 128));
			B = round(Y + 1.772000066 * (Cb - 128) + 0.0000004062980629 * (Cr - 128));

			if (R > 255)
				R = 255;
			if (R < 0)
				R = 0;
			if (G > 255)
				G = 255;
			if (G < 0)
				G = 0;
			if (B > 255)
				B = 255;
			if (B < 0)
				B = 0;

			Recons_RGB[i][j].R = R;
			Recons_RGB[i][j].G = G;
			Recons_RGB[i][j].B = B;
		}
	}

	return Recons_RGB;
}

void RGB_SQNR(ImgRGB **recons_RGB, char *origin_bmp)
{
	FILE *fp = fopen(origin_bmp, "r");
	Bitmap bmpheader;
	readheader(fp, &bmpheader);

	int H = bmpheader.height;
	int W = bmpheader.width;
	int skip = (4 - (W * 3) % 4);
	if (skip == 4)
		skip = 0;

	ImgRGB **Origin_RGB = malloc_2D(bmpheader.height, bmpheader.width);
	Read_img(fp, Origin_RGB, bmpheader.height, bmpheader.width, skip);

	double sumR = 0, sumG = 0, sumB = 0;
	double eorR = 0, eorG = 0, eorB = 0;
	double SQNR;
	for (int i = 0; i < bmpheader.height; i++)
	{
		for (int j = 0; j < bmpheader.width; j++)
		{
			sumR += Origin_RGB[i][j].R * Origin_RGB[i][j].R;
			sumG += Origin_RGB[i][j].G * Origin_RGB[i][j].G;
			sumB += Origin_RGB[i][j].B * Origin_RGB[i][j].B;
			eorR += (Origin_RGB[i][j].R - recons_RGB[i][j].R) * (Origin_RGB[i][j].R - recons_RGB[i][j].R);
			eorG += (Origin_RGB[i][j].G - recons_RGB[i][j].G) * (Origin_RGB[i][j].G - recons_RGB[i][j].G);
			eorB += (Origin_RGB[i][j].B - recons_RGB[i][j].B) * (Origin_RGB[i][j].B - recons_RGB[i][j].B);
		}
	}
	SQNR = 10 * log10(sumR / eorR);
	printf("SQNR->R = %f\n", SQNR);
	SQNR = 10 * log10(sumG / eorG);
	printf("SQNR->G = %f\n", SQNR);
	SQNR = 10 * log10(sumB / eorB);
	printf("SQNR->B = %f\n", SQNR);

	free(Origin_RGB[0]);
	free(Origin_RGB);
}

void Read_img(FILE *fp, ImgRGB **array, int H, int W, int skip)
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

// decoder 2

ImgYCbCr **Read_ascii_YCbCr(char input_name[13], int size[2])
{
	FILE *fp = fopen(input_name, "r");
	char line[1000000];
	// char split_line[1000000][10];
	char *token;
	fgets(line, sizeof(line), fp);
	token = strtok(line, " ");
	int i = 0, c = 0;
	while (token != NULL)
	{
		char number[10];
		if (sscanf(token, "%s", number) == 1)
		{
			if (strcmp(number, "000") != 0)
			{
				size[i] = atoi(number);
				i++;
			}
		}
		token = strtok(NULL, " ");
	}

	int H = size[0];
	int W = size[1];
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
	// printf("%d %d\n", block_H, block_W);
	ImgYCbCr **recons_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		line[strcspn(line, "\n")] = '\0';
		char string_arr[300][10];
		token = strtok(line, " ");
		i = 0;
		int rc[2] = {0}; // block row and col
		while (token != NULL)
		{
			// 在這裡處理每個 token
			char number[10];
			if (sscanf(token, "%s", number) == 1)
			{
				strcpy(string_arr[i], number);
				i++;
			}
			// 取得下一個 token
			token = strtok(NULL, " ");
		}
		c++;
		int row = atoi(string_arr[0]);
		int col = atoi(string_arr[1]);
		// printf("%d %d\n", row, col);
		int pos = 0;
		int value;
		if (c % 3 == 1) // 寫入Y
		{
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					// printf("%d %d\n", row * 8 + x, col * 8 + y);
					recons_YCbCr[row * 8 + x][col * 8 + y].Y = 0;
				}
			}

			for (int x = 2; x < i - 1; x += 2)
			{
				pos += atoi(string_arr[x]);
				value = atoi(string_arr[x + 1]);
				recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Y = value;
				pos++;
			}
		}
		else if (c % 3 == 2) // 寫入Cb
		{
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					// printf("%d %d\n", row * 8 + x, col * 8 + y);
					recons_YCbCr[row * 8 + x][col * 8 + y].Cb = 0;
				}
			}

			for (int x = 2; x < i - 1; x += 2)
			{
				pos += atoi(string_arr[x]);
				value = atoi(string_arr[x + 1]);
				recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Cb = value;
				pos++;
			}
		}
		else if (c % 3 == 0) // 寫入Cr
		{
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					// printf("%d %d\n", row * 8 + x, col * 8 + y);
					recons_YCbCr[row * 8 + x][col * 8 + y].Cr = 0;
				}
			}

			for (int x = 2; x < i - 1; x += 2)
			{
				pos += atoi(string_arr[x]);
				value = atoi(string_arr[x + 1]);
				recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Cr = value;
				pos++;
			}
		}
	}
	return recons_YCbCr;
}

ImgYCbCr **Inverse_zigzag(ImgYCbCr **YCbCr, int block_H, int block_W)
{
	ImgYCbCr **inverse_ZZ = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	int row, col, i;
	int pos[2] = {0};
	for (row = 0; row < block_H; row++)
	{
		for (col = 0; col < block_W; col++)
		{
			for (i = 0; i < 64; i++)
			{
				ZZ_find(pos, i);
				inverse_ZZ[row * 8 + pos[0]][col * 8 + pos[1]] = YCbCr[row * 8 + i / 8][col * 8 + i % 8];
			}
		}
	}
	return inverse_ZZ;
}

void ZZ_find(int pos[2], int i)
{
	int flag = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (zz_matrix[x][y] == i + 1)
			{
				pos[0] = x;
				pos[1] = y;
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			break;
	}
}

ImgYCbCr **Recover_DPCM(ImgYCbCr **IZZ_YCbCr, int block_H, int block_W)
{
	ImgYCbCr **recover_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	int row, col, i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			recover_YCbCr[i][j].Y = IZZ_YCbCr[i][j].Y;
			recover_YCbCr[i][j].Cb = IZZ_YCbCr[i][j].Cb;
			recover_YCbCr[i][j].Cr = IZZ_YCbCr[i][j].Cr;
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
							recover_YCbCr[row * 8][col * 8].Y = recover_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Y - IZZ_YCbCr[row * 8][col * 8].Y;
							recover_YCbCr[row * 8][col * 8].Cb = recover_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Cb - IZZ_YCbCr[row * 8][col * 8].Cb;
							recover_YCbCr[row * 8][col * 8].Cr = recover_YCbCr[(row - 1) * 8][(block_W - 1) * 8].Cr - IZZ_YCbCr[row * 8][col * 8].Cr;
						}
						else // 前一個減掉現在的
						{
							recover_YCbCr[row * 8][col * 8].Y = recover_YCbCr[row * 8][(col - 1) * 8].Y - IZZ_YCbCr[row * 8][col * 8].Y;
							recover_YCbCr[row * 8][col * 8].Cb = recover_YCbCr[row * 8][(col - 1) * 8].Cb - IZZ_YCbCr[row * 8][col * 8].Cb;
							recover_YCbCr[row * 8][col * 8].Cr = recover_YCbCr[row * 8][(col - 1) * 8].Cr - IZZ_YCbCr[row * 8][col * 8].Cr;
						}
					}
					else // 其餘一樣
					{
						recover_YCbCr[row * 8 + i][col * 8 + j].Y = IZZ_YCbCr[row * 8 + i][col * 8 + j].Y;
						recover_YCbCr[row * 8 + i][col * 8 + j].Cb = IZZ_YCbCr[row * 8 + i][col * 8 + j].Cb;
						recover_YCbCr[row * 8 + i][col * 8 + j].Cr = IZZ_YCbCr[row * 8 + i][col * 8 + j].Cr;
					}
				}
			}
		}
	}
	return recover_YCbCr;
}

ImgYCbCr **Read_binary_YCbCr(char input_name[13], int size[2])
{
	FILE *fp = fopen(input_name, "rb");
	short h, w;
	short byte;
	fread(&h, sizeof(short), 1, fp);
	fread(&w, sizeof(short), 1, fp);
	fread(&byte, sizeof(short), 1, fp);
	size[0] = h;
	size[1] = w;
	int block_H = size[0] / 8; // compute block number
	if (size[0] % 8 != 0)
	{
		block_H++;
	}
	int block_W = size[1] / 8; // compute block number
	if (size[1] % 8 != 0)
	{
		block_W++;
	}
	ImgYCbCr **recons_YCbCr = malloc_YCbCr_2D(block_H * 8, block_W * 8);
	size_t bytesRead;
	int i = 0;
	int c = 0;
	while ((bytesRead = fread(&byte, 1, sizeof(short), fp)) > 0)
	{
		short buffer[300];
		if (byte == 3003)
		{
			c++;
			int row = buffer[0];
			int col = buffer[1];
			// printf("%d %d\n", row, col);
			int pos = 0;
			int value;
			if (c % 3 == 1) // 寫入Y
			{
				for (int x = 0; x < 8; x++)
				{
					for (int y = 0; y < 8; y++)
					{
						// printf("%d %d\n", row * 8 + x, col * 8 + y);
						recons_YCbCr[row * 8 + x][col * 8 + y].Y = 0;
					}
				}

				for (int x = 2; x < i - 1; x += 2)
				{
					pos += buffer[x];
					value = buffer[x + 1];
					recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Y = value;
					pos++;
				}
			}
			else if (c % 3 == 2) // 寫入Cb
			{
				for (int x = 0; x < 8; x++)
				{
					for (int y = 0; y < 8; y++)
					{
						// printf("%d %d\n", row * 8 + x, col * 8 + y);
						recons_YCbCr[row * 8 + x][col * 8 + y].Cb = 0;
					}
				}

				for (int x = 2; x < i - 1; x += 2)
				{
					pos += buffer[x];
					value = buffer[x + 1];
					recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Cb = value;
					pos++;
				}
			}
			else if (c % 3 == 0) // 寫入Cr
			{
				for (int x = 0; x < 8; x++)
				{
					for (int y = 0; y < 8; y++)
					{
						// printf("%d %d\n", row * 8 + x, col * 8 + y);
						recons_YCbCr[row * 8 + x][col * 8 + y].Cr = 0;
					}
				}

				for (int x = 2; x < i - 1; x += 2)
				{
					pos += buffer[x];
					value = buffer[x + 1];
					recons_YCbCr[row * 8 + pos / 8][col * 8 + pos % 8].Cr = value;
					pos++;
				}
			}
			i = 0;
		}
		else
		{
			buffer[i] = byte;
			i++;
		}
	}
	return recons_YCbCr;
}

// decoder 3