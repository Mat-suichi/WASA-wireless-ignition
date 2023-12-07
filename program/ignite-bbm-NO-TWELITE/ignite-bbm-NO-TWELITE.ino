#include <INA226_asukiaaa.h>    //https://asukiaaa.blogspot.com/2019/10/ina226arduino.html
#include <Wire.h>
#define IGNITEplus 26
#define IGNITEminus 27
#define ALART 28
#define SW 29
#define TEST 2

//↓include欄に書いたブログ参照のこと
const uint16_t ina226calib = INA226_asukiaaa::calcCalibByResistorMilliOhm(2);
// const uint16_t ina226calib = INA226_asukiaaa::calcCalibByResistorMicroOhm(2000);
INA226_asukiaaa voltCurrMeter(INA226_ASUKIAAA_ADDR_A0_GND_A1_GND, ina226calib);

long timeNow;		//現在時刻
long timeStart;         //タイマー開始時刻
int swState;		//スイッチのステータス

void setup() {
  //デジタルピンの設定
  pinMode(IGNITEplus, OUTPUT);
  pinMode(IGNITEminus, OUTPUT);
  pinMode(ALART, OUTPUT);
  pinMode(SW, INPUT);

  Wire.begin();
  // Wire.begin(address); Slaveモードも同様
  /*pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);*/

  //電流計通信の設定
  Serial.begin(115200);
  // Wire2.begin(25, 26);
  // voltCurrMeter.setWire(&Wire2);
  if (voltCurrMeter.begin() != 0) {
    Serial.println("Failed to begin INA226. Check wiring and device connection.");
  }
}

void loop() {
  //現在時刻の取得 
  timeNow = millis();  

  //一秒間スイッチを押す
  if(analogRead(SW)>500 && swState==0){
    swState=1;
    timeStart = timeNow;
  }else if(analogRead(SW)>500 && swState==1){
    if(timeNow-timeStart>=1000){
      if(Connect(1)==1){
        Serial.println("WARNING : IGNITING THE ENGINE\nSTATUS : ALART IN PROCCESS\n\n");
        swState=2;
        doIgnite();
      }else if(Connect(1)==0){
        Serial.println("Igniter Not Connected. Check if the igniter is valid.");
      }
    }else{}
  }else if(swState==2){
    while(1){}		//プログラムの不備があるといけないので点火一度したらひたすらループ
  }
}



/*　　　　　　　　　　　　　　　　　　この下は関数                               */

//LED、ブザーを用いてのAlart
void Alart(void){
  //アラートを５秒間点滅→一秒駆動
  int AlartTimes=0;       //何秒経ったか。0秒から始まる

  //現在時刻の取得 
  timeStart = millis();

  while(AlartTimes<=5){
    timeNow = millis();
    
    //Alartピンの作動状況の条件分岐
    if(timeNow-timeStart<=500 && AlartTimes<5){    // .5秒まではAlartピンがHIGH→LEDとブザー作動
      Serial.println("WARNING : IGNITING THE ENGINE in %d seconds\nSTATUS : ALART IN PROCESS\n\n", 5 - AlartTimes);
      digitalWrite(ALART, HIGH);
    }else if(timeNow-timeStart>500 && AlartTimes<5){        // .5秒まではAlartピンがLOW→LEDとブザー消灯
      digitalWrite(ALART, LOW);
    }

    //一秒経過後の時間関連の処理
    if(timeNow-timeStart>=1000){
      timeStart=timeNow;    //一秒ごとにtimeStartを更新
      AlartTimes++;         //一秒ごとにAlartTimesを一つ増やす
    }

    //5秒経過したら1秒Alartを行う。1秒後、
    if(timeStart<=1000 || AlartTimes==5){
      digitalWrite(ALART, HIGH);
    }else if(AlartTimes==5){
      AlartTimes++;     //冗長化
      break;            //Alartを終了
    }

    //スイッチから手が離れたら終了
    if(analogRead(SW)<500){
      Serial.println("ANNOUNCEMENT : ABORT IGNITION\nHAND WAS REMOVED FROM THE SWITCH\n\n");
      while(true){}
    }
  }
}

//点火操作を行う
void doIgnite(void){
  //アラート計6秒後点火。５秒間点火駆動
  Alart();

  //点火を行う
  digitalWrite(IGNITEplus, HIGH);
  digitalWrite(IGNITEminus, HIGH);

  //電流電圧を測定する。
  int N_ig=0;

  for(int i=0; i<50; i++){
    if(Connect(1)==0){
      N_ig++;
    }else if(N_ig==2){
      break;
    }else{
      N_ig=0;
      delay(100);
    }
  }

  //５秒後点火終了。二度目は行わないようにする。
  digitalWrite(IGNITEplus, LOW);
  digitalWrite(IGNITEminus, LOW);
  
  while(true){
    //一度点火を実行したらもう何もしないように、ひたすらループするプログラムを入れた
   }
}


//イグナイターがくっついていない→0、くっついている→1、エラー→３
int Connect(int p){
  int i;
  int16_t ma, mv, mw;
  double sumv=0, suma=0;
  double AveV, AveA, r;

  for(i=0; i<5; i++){
    if (voltCurrMeter.readMV(&mv) == 0) {
      sumv+=mv;
    } else {
      Serial.println("Cannot read voltage.");
      return 3;
    }
    if (voltCurrMeter.readMA(&ma) == 0) {
      suma+=ma;
    } else {
      Serial.println("Cannot read current.");
      return 3;
    }
    delay(1);
  }
  AveV=sumv/5;
  AveA=suma/5;
  r=AveV/AveA;
  if(p==1){
    Serial.println(String(AveV) + "mV\t" + String(AveA) + "mA\t"+ String(r) + "Ω");
  }
  if(AveA<5 || AveV<5 || r<1 || r>10000){
    return 0;
  }else{
    return 1;
  }
}

/*この先不要
//電圧を読む。0.01×10=0.1秒の平均を読む。
double readVoltage(void){
  int16_t mv;
  int i;
  double sum=0;
  for(i=0; i<10; i++){
    if (voltCurrMeter.readMV(&mv) == 0) {
      sum+= String(mv).toDouble();
    } else {
      Serial.println("Cannot read voltage.");
      return 10000;
      exit(0);
    }
    delay(10);
  }
  Serial.println(String(sum/10) + "mV");
  return sum/10;
}

//電流を読む。0.01×10=0.1秒の平均を読む。
double readCurrent(void){
  int16_t ma, mv, mw;
  int i;
  double sum=0;
  for(i=0; i<10; i++){
    if (voltCurrMeter.readMA(&ma) == 0) {
      sum+= String(ma).toDouble();
    } else {
      Serial.println("Cannot read current.");
      return 10000;
      exit(0);
    }
    delay(10);
  }
  Serial.println(String(sum/10) + "mA");
  return sum/10;
}
