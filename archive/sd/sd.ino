#include <SPI.h> 
#include <SD.h>

// https://ameblo.jp/geotechlab-workshop/entry-12566563241.html

const int chipSelect = 10; // Arduino UNOでは10、Arduino MEGAでは53

void setup(void)
{
    /* ----- Setting up serial communication with PC ------ */
    /* ここでUSBを介してPCとシリアル通信を始める。9600はシリアル通信のボーレート */
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    //何らかの問題があってシリアルポートに接続できないときは、このループにトラップされる    
    }

    /* ----- Initialisation of SD card ------ */
    Serial.print("Initializing SD card...");
     //see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
       Serial.println("Card failed, or not present");
       // don't do anything more:
      return;
    }
    Serial.println("card initialized.");
}

void loop(void)
{
    int ainput; //読み取ったbit数：intは整数
    float vinput; //bit数を電圧に変換したもの：floatは浮動小数点数

    /* データの読み取り */
    ainput=analogRead(A0); //ピンA0から電圧をbitとして読む
    vinput=5000.0*ainput/1024; //上記を電圧mVに変換
    //ここで5000.0でなく5000とすると、整数としてvinputにキャストされてしまう）
    //粗い値になったり、おかしな値になったりする

    /* PCのシリアルモニタに表示 */
    Serial.print(ainput);
    Serial.println(" bit"); //" bit"と表示、printではなくprintlnとすることで改行する
    Serial.print(vinput);
    Serial.println(" mV"); //" mV"と表示、printではなくprintlnとすることで改行する

    /* SDカードに書き込み */
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile)
    {
      dataFile.print(ainput);
      dataFile.println(" bit"); //" bit"と表示、printではなくprintlnとすることで改行する
      dataFile.print(vinput);
      dataFile.println(" mV"); //" mV"と表示、printではなくprintlnとすることで改行する
    }    
    dataFile.close();
    
    delay(2000); //2000ミリ秒=2秒の停止
}
