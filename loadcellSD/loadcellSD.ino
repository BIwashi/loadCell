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

/*-------------------------------------*/
char filename[12] = "data000.log";
byte filenum_1 = 0;
byte filenum_10 = 0;
byte filenum_100 = 0;
File dataFile;
/*-------------------------------------*/

////ファイル名を記憶しておくリスト変数
//char** nameList;
////リスト内に含まれるファイル数
//int numList;
//
//int numFile = 1;
//char fileName[12] ;

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

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ６０１　１２０ｋＧ [P-12035]
//---------------------------------------------------//
//#define OUT_VOL   0.001f      //定格出力 [V]
//#define LOAD      120000.0f   //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ１３３　２０ｋＧ [P-12034]
//---------------------------------------------------//
// #define OUT_VOL 0.002f //定格出力 [V]
// #define LOAD 20000.0f  //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ビーム型）　ＳＣ６１６Ｃ　５００ｇ[P-12532]
//---------------------------------------------------//
//#define OUT_VOL   0.0007f   //定格出力 [V]
//#define LOAD      500.0f    //定格容量 [g]

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

  SD_init();










  //  /* ----- Initialisation of SD card ------ */
  //  Serial.print("Initializing SD card...");
  //  //see if the card is present and can be initialized:
  //  if (!SD.begin(chipSelect)) {
  //    Serial.println("Card failed, or not present");
  //    // don't do anything more:
  //    return;
  //  }
  //  Serial.println("card initialized.");



  //  /*--------------------------------------------------------------*/
  //  //内部のファイルにアクセス
  //  File root = SD.open("/");  //SDカードのルートフォルダ
  //  String listTemp = "";
  //  while (true) {
  //    File entry = root.openNextFile();
  //    if (!entry) {
  //      //これ以上ファイルがない場合
  //      break;
  //    }
  //
  //    //ディレクトリ名ではない場合
  //    if (!entry.isDirectory()) {
  //      String fileName = entry.name();
  //      listTemp += String(fileName);
  //      listTemp += ",";
  //
  //      /*
  //        //特定の種類のファイルだけを選び出すことも可能(ここでは音楽ファイルを抽出)
  //        //ファイルの拡張子を取り出す
  //        //一番後ろのピリオドから後の文字を抽出
  //        String ext = fileName.substring(fileName.lastIndexOf('.'));
  //        //拡張子が指定のものだけを入れていく
  //        if (ext.equalsIgnoreCase(".wav") || ext.equalsIgnoreCase(".mp3")) {  //大文字か小文字かを無視する
  //        listTemp += String(fileName);
  //        listTemp += ",";
  //        }
  //      */
  //    }
  //    else {
  //      //ディレクトリ内部を　検索する場合は、再起関数として同じ処理を呼び出す
  //    }
  //  }
  //
  //  if (listTemp.length() > 0) {
  //    Serial.println(listTemp);
  //
  //    //リストの要素数を数え上げる
  //    for (int i = 0; i < listTemp.length(); i++) {
  //      i = listTemp.indexOf(',', i);  //コンマの位置を探す
  //      numList++;
  //    }
  //
  //    //リストの初期化
  //    nameList = new char*[numList];
  //
  //    for (int i = 0; i < numList; i++) {
  //      //カンマの位置を見つけ、
  //      int index = listTemp.indexOf(',');
  //      String temp = String(listTemp.substring(0, index));
  //      nameList[i] = new char[temp.length() + 1];
  //      temp.toCharArray(nameList[i], temp.length() + 1);
  //      Serial.println(nameList[i]);
  //      listTemp.remove(0, index + 1);
  //    }
  //
  //    Serial.println(numList);
  //  }
  //  else {
  //    //ファイルが見つからなかった場合は強制終了
  //    return;
  //  }
  //  /*--------------------------------------------------------------*/
  //
  //  numFile += (int)numList;
  //  fileName = numFile
  //             //  + '.csv'
  //
  //             /*--------------------------------------------------------------*/
  Serial.println(" -----Start----- ");
  //  Serial.println("time weight [g] ");
}


//------------------------------------------------------------------------------------
void loop()
{
  float data;
  char S1[20];
  char s[20];
  time = millis();
  data = AE_HX711_getGram(5);
  sprintf(S1, " %s [g] (0x%4x)", dtostrf((data - offset), 5, 3, s), AE_HX711_Read());
  Serial.print(time);
  Serial.println(S1);

//  SD_write_int((String)S1); // String型にキャストした。こんなことしていいの？

  //  /* SDカードに書き込み */
  //  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  //  if (dataFile)
  //  {
  //    dataFile.print(time);
  //    dataFile.println(S1);
  //  }
  //  dataFile.close();
}

// dtostrf(浮動小数点値,文字列の長さ,小数点以下の桁数,文字列バッファ)

//------------------------------------------------------------------------------------

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
//  SdFile::dateTimeCallback( &dateTime );
}

/*-----------------受け取ったデータを書き込む-----------------*/
void SD_write_int(String Data) {
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
