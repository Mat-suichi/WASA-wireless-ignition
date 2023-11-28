#include <INA226_asukiaaa.h>    //https://asukiaaa.blogspot.com/2019/10/ina226arduino.html
#define IGNITEplus 26
#define IGNITEminus 27
#define ALART 28
#define SW 29

void setup() {
  // put your setup code here, to run once:
  pinMode(IGNITEplus, OUTPUT);
  pinMode(IGNITEminus, OUTPUT);
  pinMode(ALART, OUTPUT);
  pinMode(SW, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int time=0;
  
  //一秒間スイッチを押す
  while(time<=1000){
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
    delay(5000);
    //５秒後点火終了。二度目は行わないようにする。
    digitalWrite(IGNITEplus, LOW);
    digitalWrite(IGNITEminus, LOW);
    exit(0);
  }
}
