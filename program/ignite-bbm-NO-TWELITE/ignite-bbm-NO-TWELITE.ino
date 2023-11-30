#include <INA226_asukiaaa.h>    //https://asukiaaa.blogspot.com/2019/10/ina226arduino.html
#include <Wire.h>
#define IGNITEplus 26
#define IGNITEminus 27
#define ALART 28
#define SW 29

//↓include欄に書いたブログ参照のこと
const uint16_t ina226calib = INA226_asukiaaa::calcCalibByResistorMilliOhm(2);
// const uint16_t ina226calib = INA226_asukiaaa::calcCalibByResistorMicroOhm(2000);
INA226_asukiaaa voltCurrMeter(INA226_ASUKIAAA_ADDR_A0_GND_A1_GND, ina226calib);

void setup() {
  //デジタルピンの設定
  pinMode(IGNITEplus, OUTPUT);
  pinMode(IGNITEminus, OUTPUT);
  pinMode(ALART, OUTPUT);
  pinMode(SW, INPUT);

  Wire.begin();
  // Wire.begin(address); Slaveモードも同様
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);

  //電流計通信の設定
  Serial.begin(115200);
  if (voltCurrMeter.begin() != 0) {
    Serial.println("Failed to begin INA226");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int time=0;
  //一秒間スイッチを押す
  while(time<=10){
    if(digitalRead(SW)==1){
      time++;
      delay(10);
    }
    else{break;}
  }
  if(time==1001){
    //アラートを５秒間点滅→一秒駆動
    for(int i=0; i<=5; i++){
      digitalWrite(SW, HIGH);
      delay(500);
      digitalWrite(SW, LOW);
      delay(500);
    }
    digitalWrite(ALART, HIGH);
    delay(1000);
    //アラート計6秒後点火。５秒間点火駆動
    digitalWrite(IGNITEplus, HIGH);
    digitalWrite(IGNITEminus, HIGH);
    for(int i=0; i<50; i++){
      if(readVoltage()<5.0 || readCurrent()<5.0 || readVoltage()/readCurrent()<1.0){
        break;
      }else{
        delay(100);
      }
    }
    //５秒後点火終了。二度目は行わないようにする。
    digitalWrite(IGNITEplus, LOW);
    digitalWrite(IGNITEminus, LOW);
    
    while(1){
      //一度点火を実行したらもう何もしないように、ひたすらループするプログラムを入れた
    }
  }
}

//電圧を読む。0.01×10=0.1秒の平均を読む。
double readVoltage(void){
  int16_t ma, mv, mw;
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
  Serial.println(String(sum/10) + "mV");
  return sum/10;
}