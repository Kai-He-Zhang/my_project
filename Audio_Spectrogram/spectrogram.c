#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#define PI acos(-1)

typedef struct  //設定wav header儲存struct
{
    char ChunkID[4];
    int ChunkSize;
    char Format[4];

    char Subchunk1ID[4];
    int Subchunk1Size;
    short AudioFormat;
    short NumChannels;
    int SampleRate;
    int ByteRate;
    short BlockAlign;
    short BitsPerSample;

    char Subchunk2ID[4];
    int Subchunk2Size;
} wavefile_header;

double *rect_window(int w_size) //設定rectangular window
{
    double *rw = (double *)malloc(sizeof(double) * w_size);
    for (int i = 0; i < w_size; i++)
    {
        rw[i] = 1;
    }
    return rw;
}

double *hamming_window(int w_size) //設定hamming window
{
    double *hw = (double *)malloc(sizeof(double) * w_size);
    for (int i = 0; i < w_size; i++)
    {
        hw[i] = 0.54 - 0.46 * cos(2 * PI * i / (w_size - 1));
    }
    return hw;
}

int main(int argc, char **argv)
{
    double w_size = atof(argv[1]);        //analysis window size
    char *w_type = argv[2];               //window type
    double dft_size = atof(argv[3]);      //DFT window size
    double fitv = atof(argv[4]);          //frame interval
    FILE *wav_in = fopen(argv[5], "rb");  //wave in
    FILE *spec_out = fopen(argv[6], "w"); //wave dft txt out

    wavefile_header winh;                 //讀入wav檔案header，需要一些資訊換算時間和array
    fseek(wav_in, 0, SEEK_SET);
    fread(&winh, sizeof(winh), 1, wav_in);
    double data_time = (double)(winh.Subchunk2Size / winh.ByteRate) * 1000; //計算檔案時間長度

    double fs = winh.SampleRate;                                //讀取音訊取樣率
    size_t win_sample_number = (int)data_time / 1000 * fs;      //計算音訊資料陣列總共長度
    int16_t *x;
    x = (int16_t *)malloc(sizeof(int16_t) * win_sample_number); //創建音訊內容長度陣列
    fseek(wav_in, 44, SEEK_SET);
    fread(x, sizeof(int16_t), win_sample_number, wav_in);       //讀取音訊內容

    int frame_number = data_time / fitv;                        //計算frame總數
    int aws = (w_size / data_time) * win_sample_number;         //換算w_size從時間變陣列長度(3.2ms -> 256) 
    int dws = (dft_size / data_time) * win_sample_number;       //換算dft_size從時間變陣列長度
    int frame_length = win_sample_number / (data_time / fitv);  //計算frame長度
    double *frame;                //宣告frame陣列，未分配空間
    double X[frame_number][dws];  //宣告二維陣列X儲存計算好的DFT
    double *window;               //宣告window，之後選擇要用哪種類型的window
    if (strcmp(w_type, "rectangular") == 0)
    {
        window = rect_window(aws);
    }
    else if (strcmp(w_type, "hamming") == 0)
    {
        window = hamming_window(aws);
    }
    else
    {
        printf("please enter rectangular or hamming in argv[2]\n");  //沒有選定的window，報錯
        return 1;
    }
    for (int i = 0; i < frame_number; i++)                  //計算dft和把frame套上window
    {
        frame = (double *)malloc(sizeof(double) * dws);     //開frame長度大小的陣列
        int j = 0;
        for (; j < aws; j++)
        {
            frame[j] = x[i * frame_length + j] * window[j]; //將原訊號套上window並且存進frame裡面
        }
        for (; j < dws; j++)
        {
            frame[j] = 0;                                   //zero padding
        }

        for (int k = 0; k < dws; k++)                       //計算dft(log(n^2))
        {
            double real = 0;
            double imag = 0;
            for (int n = 0; n < dws; n++)                   //計算DFT複數實部和虛部
            {
                double theta = 2.0 * PI * n * k / dws;
                real += frame[n] * cos(theta);
                imag -= frame[n] * sin(theta);
            }
            double absol = sqrt(real * real + imag * imag); //取絕對值
            //fprintf(spec_out, "%lf\n", abs);
            if (absol < 1)                               //將較小的數值更改為1，取log才不會出現負數
            {
                absol = 1;
            }
            //fprintf(spec_out, "%lf\n", abs);
            X[i][k] = 20 * log10(absol);                    //更改為dB
            fprintf(spec_out, "%lf ", X[i][k]);             //一個frame算完就輸入進txt檔裡，並且用空格分開
        }
        fprintf(spec_out, "\n");   //換計算下個frame的時候，換行輸入
        free(frame);               //將當前的frame空間釋出，之前寫的會因為開太多空間出現segment_fault
    }

    fclose(wav_in);
    fclose(spec_out);
}
