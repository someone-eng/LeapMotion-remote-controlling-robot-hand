//HC-05分为有EN和有key两种：
//有EN的需要，在上电前按住按钮不放，再上电，且除了VCC,GND,RX,TX之外的引脚都不接
//有KEY的需要，在上电前将KEY脚与3.3V脚连接，INT脚不接
#include <SoftwareSerial.h>
#include <Servo.h> 
  
Servo thumbservo,indexservo,middleservo,ringservo,littleservo;  // create servo object to control a servo
//使用软件串口，能讲数字口模拟成串口
SoftwareSerial BT(10, 11);  
// Pin10为RX，接HC05的TXD
// Pin11为TX，接HC05的RXD
char val;  //存储接收的变量

typedef union {
    int i;
    char c[sizeof(int)];
} uTemp;

uTemp u;
int i = 0, flag = 0;
 
void setup() {
  Serial.begin(38400);   //与电脑的串口连接，串口监视器波特率设置为9600
  Serial.println("BT is ready!");
  BT.begin(38400);  //设置蓝牙波特率为默认值
  thumbservo.attach(5);  // attaches the servo on pin 5 for the thumb 
  indexservo.attach(6); // attaches the servo on pin 6 for index finger
  middleservo.attach(7); // attaches the servo on pin 9 for the middle finger
  ringservo.attach(8); // attaches the servo on pin 10 for the ring finger
  littleservo.attach(9); // attaches the servo on pin 11 for the little finger   
}
 
void loop() {
  //如果串口接收到数据，就输出到蓝牙串口
  if (Serial.available()) {
    val = Serial.read();
    BT.print(val);
  }
  //接收蓝牙模块数据
  if (BT.available()) {
    val = BT.read();
    // Serial.print(val);

    if (flag != 0){
      u.c[i] = val;
      i++;
    }
    
    if (i==2){
      switch (flag){
        case 1:
          // Serial.print("T\n");
          thumbservo.write(u.i);
          break;
        case 2:
          // Serial.print("I\n");
          indexservo.write(u.i);
          break;
        case 3:
          // Serial.print("M\n");
          middleservo.write(u.i);
          break;
        case 4:
          // Serial.print("R\n");
          ringservo.write(u.i);
          break;
        case 5:
          // Serial.print("L\n");
          littleservo.write(u.i);
      }
      // Serial.print(u.i);
      flag = 0;
      i = 0;
    }

    switch (val){
      case 'T':
        flag = 1;
        break;
      case 'I':
        flag = 2;
        break;
      case 'M':
        flag = 3;
        break;
      case 'R':
        flag = 4;
        break;
      case 'L':
        flag = 5;
    }
  }
}
//当发现串口监视器显示BT is ready!后，输入AT点击键盘回车一次，当看到OK的时候就说明可以进行AT模式设置了。