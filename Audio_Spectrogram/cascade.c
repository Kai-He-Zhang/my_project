#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>

typedef struct   //設定wave header，讀取檔案需要使用
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

int main(int argc, char *argv[])
{
    FILE *makefile = fopen(argv[2], "ab");  //做第一次因為沒有開output檔案，所以先開個檔案建檔，沒有會報錯
    fclose(makefile);

    wavefile_header wav_cin = {             //initialize output wav header
        .ChunkID = "RIFF",                  //一開始做合併沒有宣告
        .ChunkSize = 36,                    //會因為讀取錯誤導致複製第一個檔案失敗
        .Format = "WAVE",                   //header在合併會有錯誤
        .Subchunk1ID = "fmt ",
        .Subchunk1Size = 16,
        .AudioFormat = 1,
        .NumChannels = 1,
        .SampleRate = 8000,
        .ByteRate = 8000 * (16 / 8) * 1,
        .BlockAlign = 1 * 16 / 8,
        .BitsPerSample = 16,
        .Subchunk2ID = "data",
        .Subchunk2Size = 0};

    FILE *in = fopen(argv[1], "rb");         //讀取要合併的檔案
    FILE *combine_in = fopen(argv[2], "rb"); //讀取目前已經合併在一起的檔案，原本是用ab+但是寫入會出問題，所以後面關掉再用wb寫入

    fseek(in, 0, SEEK_SET);                  //指定讀取wav header的位置
    fseek(combine_in, 0, SEEK_SET);          //指定讀取wav header的位置
    wavefile_header wav_in;
    fread(&wav_in, sizeof(wav_in), 1, in);           //讀取要合併的wav header
    fread(&wav_cin, sizeof(wav_cin), 1, combine_in); //讀取已經合併的wav header

    wavefile_header wav_out = {              //設定合併後輸出的wav header
        .ChunkID = "RIFF",
        .ChunkSize = 36 + wav_in.Subchunk2Size + wav_cin.Subchunk2Size, //這裡要合併wav_in header和目前已經合併在一起的wav header檔案
        .Format = "WAVE",
        .Subchunk1ID = "fmt ",
        .Subchunk1Size = 16,
        .AudioFormat = 1,
        .NumChannels = 1,
        .SampleRate = (wav_in.SampleRate == 8000) ? 8000 : 16000,  //透過read選擇output合併header檔案要8000還是16000 sample_size
        .ByteRate = (wav_in.SampleRate == 8000) ? 8000 * (16 / 8) * 1 : 16000 * (16 / 8) * 1, //同上
        .BlockAlign = 1 * 16 / 8,
        .BitsPerSample = 16,
        .Subchunk2ID = "data",
        .Subchunk2Size = wav_in.Subchunk2Size + wav_cin.Subchunk2Size}; //合併兩個的pcm內容size

    fseek(combine_in, 44, SEEK_SET);                        //讀取目前已經合併在一起的wav音訊資料部分
    size_t wcin_sample_number = wav_cin.Subchunk2Size / 2;  //計算要開多少大小array儲存
    int16_t *buffer;
    buffer = (int16_t *)malloc(sizeof(int16_t) * wcin_sample_number); //開array
    fread(buffer, sizeof(int16_t), wcin_sample_number, combine_in);   //讀取pcm
    fclose(combine_in);                                           

    FILE *out = fopen(argv[2], "wb");                        //重開一次寫入
    fseek(out, 0, SEEK_SET);
    fwrite(&wav_out, sizeof(wav_out), 1, out);                //將合併後的wav out輸入進來
    fseek(out, 0, SEEK_END);                                  //指定要輸入pcm內容的位置
    fwrite(buffer, sizeof(int16_t), wcin_sample_number, out); //將剛剛存進buffer的pcm內容輸入進out.wav
    
    fseek(in, 44, SEEK_SET);            //指定輸入wav檔案音訊部分開頭位置
    fseek(out, 0, SEEK_END);            //指定輸出wav檔案音訊最末端
    int16_t *x;
    while (fread(&x, sizeof(x), 1, in)) //讀取wav in的輸入pcm檔案在輸出wav的最尾端
    {
        fwrite(&x, sizeof(x), 1, out);
    }                                   //這樣就合併完了

    fclose(in);
    fclose(out);
    remove(argv[1]);                    //然後關檔移除合併的wav檔案
}