#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

unsigned long time;

void AE_HX711_Init(void);
void AE_HX711_Reset(void);
long AE_HX711_Read(void);
long AE_HX711_Averaging(long adc, char num);
float AE_HX711_getGram(char num);
const int chipSelect = 10; // Arduino UNOでは10、Arduino MEGAでは53

/*-------リセットするごとに新しいファイル作成するための変数-------*/
char filename[12] = "data000.csv";
byte filenum_1 = 0;
byte filenum_10 = 0;
byte filenum_100 = 0;
File dataFile;

//---------------------------------------------------//
// ピンの設定
//---------------------------------------------------//
#define pin_dout 8
#define pin_slk 9

//---------------------------------------------------//
// ロードセル　Ｓ字型　ＳＣ３０１Ａ　１００ｋＧ [P-12036]
//---------------------------------------------------//
#define OUT_VOL   0.002f      //定格出力 [V]
#define LOAD      100000.0f   //定格容量 [g]

float offset;

//------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  Serial.println("loadcell begin");

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    //何らかの問題があってシリアルポートに接続できないときは、このループにトラップされる
  }
  AE_HX711_Init();
  AE_HX711_Reset();
  offset = AE_HX711_getGram(30);

  SD_init(); // SDカードの初期化とファイル選択

  Serial.println(" -----Start----- ");
  //  Serial.println("time weight [g] ");
}


//------------------------------------------------------------------------------------
void loop()
{
  float data;
  char S1[20];
  char s[20];
  //  time = millis();
  data = AE_HX711_getGram(5);
  sprintf(S1, " %s [g] (0x%4x)", dtostrf((data - offset), 5, 3, s), AE_HX711_Read()); // dtostrf(浮動小数点値,文字列の長さ,小数点以下の桁数,文字列バッファ)
  //  Serial.print(time);
  //  Serial.println(S1);

  SD_write_Str((String)S1); // String型にキャストした。こんなことしていいの？

}

//------------------------------------------------------------------------------------

/*------------------function------------------*/

void AE_HX711_Init(void)
{
  pinMode(pin_slk, OUTPUT);
  pinMode(pin_dout, INPUT);
}

void AE_HX711_Reset(void)
{
  digitalWrite(pin_slk, 1);
  delayMicroseconds(100);
  digitalWrite(pin_slk, 0);
  delayMicroseconds(100);
}

long AE_HX711_Read(void)
{
  long data = 0;
  while (digitalRead(pin_dout) != 0)
    ;
  delayMicroseconds(10);
  for (int i = 0; i < 24; i++)
  {
    digitalWrite(pin_slk, 1);
    delayMicroseconds(5);
    digitalWrite(pin_slk, 0);
    delayMicroseconds(5);
    data = (data << 1) | (digitalRead(pin_dout));
  }
  //Serial.println(data,HEX);
  digitalWrite(pin_slk, 1);
  delayMicroseconds(10);
  digitalWrite(pin_slk, 0);
  delayMicroseconds(10);
  return data ^ 0x800000;
}

long AE_HX711_Averaging(long adc, char num)
{
  long sum = 0;
  for (int i = 0; i < num; i++)
    sum += AE_HX711_Read();
  return sum / num;
}

float AE_HX711_getGram(char num)
{
#define HX711_R1 20000.0f
#define HX711_R2 8200.0f
#define HX711_VBG 1.25f
#define HX711_AVDD 4.2987f                  //(HX711_VBG*((HX711_R1+HX711_R2)/HX711_R2))
#define HX711_ADC1bit HX711_AVDD / 16777216 //16777216=(2^24)
#define HX711_PGA 128
#define HX711_SCALE (OUT_VOL * HX711_AVDD / LOAD * HX711_PGA)

  float data;

  data = AE_HX711_Averaging(AE_HX711_Read(), num) * HX711_ADC1bit;
  //Serial.println( HX711_AVDD);
  //Serial.println( HX711_ADC1bit);
  //Serial.println( HX711_SCALE);
  //Serial.println( data);
  data = data / HX711_SCALE;

  return data;
}

/*-----------------SDカードの初期化とファイル選択-----------------*/
/* https://gist.github.com/EnsekiTT/6315684 */
void SD_init() {
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    while (1);
  }
  Serial.println(F("SD ok."));
  while (1) {
    if (SD.exists(filename)) {
      filenum_1++;
      if (filenum_1 > 9) {
        filenum_10++;
        filenum_1 = 0;
        if (filenum_10 > 9) {
          filenum_100++;
          filenum_10 = 0;
        }
      }
      filename[4] = filenum_100 + 48;
      filename[5] = filenum_10 + 48;
      filename[6] = filenum_1 + 48;
    } else {
      break;
    }
  }
  dataFile = SD.open(filename, FILE_WRITE);
  Serial.print(F("ok."));
  Serial.println(filename);
}

/*-----------------受け取ったデータを書き込む-----------------*/
void SD_write_Str(String Data) {
  time = millis(); // 起動してからの時間
  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.print(time);
    dataFile.println(Data);
    dataFile.close();
    Serial.print(time);
    Serial.println(Data);
  }
  else {
    Serial.print(F("error opening "));
    Serial.println(filename);
  }
}
