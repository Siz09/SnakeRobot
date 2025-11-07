#include <ESP32Servo.h>
Servo s1;
void setup() {
  // put your setup code here, to run once:
  s1.attach(27);

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<=90;i++){
s1.write(i);
delay(1000);
  }
  for(int i=90;i>=0,i--){
    s1.write(i);
    delay(1000);
  }

}
