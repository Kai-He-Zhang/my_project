#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#define PI 3.14159265358979323846

// define wavefile header 參數
typedef struct
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

// define 生成波型所需參數
typedef struct
{
    double fs;       // 取樣率
    int sample_size; // 取樣深度
    double freq;     // 波頻率
    double A;        // 波振幅
    double T;        // 取樣週期
    double L;        // 波長度
    size_t N;        // 取樣點總數
} gen_wave_data;

// 生成sine波
void gen_sin(FILE *fp, gen_wave_data gwd)
{
    if (gwd.sample_size == 16) // bits per sample 為 16, 其餘計算的跟 8 bits差不多
    {
        short *x;
        double tmp;
        size_t n;
        double *tmpwave;
        x = (short *)malloc(sizeof(short) * gwd.N);
        tmpwave = (double *)malloc(sizeof(double) * gwd.N);
        for (n = 0; n < gwd.N; n++)
        {
            tmp = gwd.A * sin(2 * PI * gwd.freq * n * gwd.T); // 沒有多加位移
            tmpwave[n] = tmp;
            x[n] = (short)floor(tmp + 0.5);
        }
        fwrite(x, sizeof(short), gwd.N, fp);
        fclose(fp);
    }
}

// 生成sawtooth波
void gen_sawtooth(FILE *fp, gen_wave_data gwd)
{
    if (gwd.sample_size == 16) // 除了波型沒有向上位移外，其他皆與8bits sawtooth生成一樣
    {
        double *tmpwave;
        short *x;
        double tmp;
        size_t n;
        double i = 0.0;
        x = (short *)malloc(sizeof(short) * gwd.N);
        tmpwave = (double *)malloc(sizeof(double) * gwd.N);
        for (n = 0; n < gwd.N; n++)
        {
            tmp = gwd.A * 2 * (gwd.freq * n * gwd.T - floor(gwd.freq * n * gwd.T + 0.5));
            x[n] = (short)floor(tmp + 0.5); // 用short儲存16bits per sample quantization(四捨五入)
        }
        fwrite(x, sizeof(short), gwd.N, fp);
        fclose(fp);
    }
}

// 生成方波
void gen_square(FILE *fp, gen_wave_data gwd)
{
    if (gwd.sample_size == 16) // 除了波型沒有向上位移外，其他皆與8bits square生成一樣
    {
        double *tmpwave;
        short *x;
        double tmp;
        size_t n;
        x = (short *)malloc(sizeof(short) * gwd.N);
        tmpwave = (double *)malloc(sizeof(double) * gwd.N);
        for (n = 0; n < gwd.N; n++)
        {
            tmp = sin(2 * PI * gwd.freq * n * gwd.T);
            if (tmp > 0)
                tmp = gwd.A;
            if (tmp <= 0)
                tmp = -gwd.A;
            x[n] = (short)floor(tmp + 0.5); // 用short儲存16bits per sample quantization(四捨五入)
            tmpwave[n] = tmp;
        }
        fwrite(x, sizeof(short), gwd.N, fp);
        fclose(fp);
    }
}

// 生成三角波
void gen_triangle(FILE *fp, gen_wave_data gwd)
{
    if (gwd.sample_size == 16) // 除了波型沒有向上位移外，其他皆與8bits triangle生成一樣
    {
        double *tmpwave;
        short *x;
        double tmp;
        size_t n;
        int dir = 0;
        double i = 0.0;
        x = (short *)malloc(sizeof(short) * gwd.N);
        tmpwave = (double *)malloc(sizeof(double) * gwd.N);
        for (n = 0; n < gwd.N; n++)
        {
            tmp = gwd.A * 2 * fabs(2 * (gwd.freq * n * gwd.T - floor(gwd.freq * n * gwd.T + 0.5))) - gwd.A;
            x[n] = (short)floor(tmp + 0.5); // 用short儲存16bits per sample quantization(四捨五入)
        }
        fwrite(x, sizeof(short), gwd.N, fp);
        fclose(fp);
    }
}

int main(int argc, char *argv[])
{
    // 以下的程式碼編譯後執行輸入所需的參數，./sinegen_xxxxxxxxx fs m wavetype f A T fn.wav sqnr.txt，即可選擇所要的波型

    // 終端機執行少引數報錯
    if (argc != 8)
    {
        printf("error!!! the usage: ./sinegen_xxxxxxxxx fs m wavetype f A T fn.wav\n");
        return 1;
    }
    // 設定生成波所需的檔案
    FILE *fp = fopen(argv[7], "wb"); // wav file 宣告
    char *wavetype = argv[3];
    char *endptr;                                         // 沒用的東西,配合strtod
    gen_wave_data gwd = {                                 // 生成波所需參數，執行參數輸入進來
                         .fs = (double)(atoi(argv[1])),   // sample rate
                         .sample_size = atoi(argv[2]),    // bits per sample
                         .freq = (double)(atoi(argv[4])), // wave frequency
                         .A = strtod(argv[5], &endptr),   // wave amplitude range(0, 1)
                         .T = 1 / gwd.fs,                 // sample period
                         .L = strtod(argv[6], &endptr),   // wave length
                         .N = (size_t)(gwd.L * gwd.fs)};  // total sample number

    // 設定wavheader所需參數
    const short numChannel = 1;                                   // 單聲道
    const short audioformat = 1;                                  // pcm 壓縮 = 1
    int subchunk2size = gwd.N * numChannel * gwd.sample_size / 8; // total data sample number * numchannel * bits per sample / 8

    // initialization wav檔案header
    wavefile_header wav_h = {
        .ChunkID = "RIFF",
        .ChunkSize = 36 + subchunk2size, // 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
        .Format = "WAVE",
        .Subchunk1ID = "fmt ",
        .Subchunk1Size = 16,                                     // bits per sample
        .AudioFormat = audioformat,                              // pcm 壓縮 = 1
        .NumChannels = numChannel,                               // 單聲道
        .SampleRate = gwd.fs,                                    // sample rate
        .ByteRate = gwd.fs * (gwd.sample_size / 8) * numChannel, // 位元組率等於[取樣率X通道數X位元率/8]
        .BlockAlign = numChannel * gwd.sample_size / 8,          // NumChannels X BitsPerSample/8
        .BitsPerSample = gwd.sample_size,                        // bits per sample
        .Subchunk2ID = "data",
        .Subchunk2Size = subchunk2size}; // total data sample number * numchannel * bits per sample / 8

    fwrite(&wav_h, sizeof(wav_h), 1, fp);

    // 選擇需要生成哪種波型，每個使用到的皆為sqnr file寫入檔，fp wav寫入檔，gwd struct相關波型參數
    if (strcmp(wavetype, "sine") == 0)
    {
        // printf("sine\n");
        gen_sin(fp, gwd); // 生成sine波
    }
    else if (strcmp(wavetype, "square") == 0)
    {
        // printf("square\n");
        gen_square(fp, gwd); // 生成square波
    }
    else if (strcmp(wavetype, "sawtooth") == 0)
    {
        // printf("sawtooth\n");
        gen_sawtooth(fp, gwd); // 生成sawtooth波
    }
    else if (strcmp(wavetype, "triangle") == 0)
    {
        // printf("triangle\n");
        gen_triangle(fp, gwd); // 生成triangle波
    }
    else // 沒有對應的輸入波報錯
    {
        printf("error!!! no this wave, please enter sine or sawtooth or square or triangle.\n");
        return 1;
    }
    return 0;
}