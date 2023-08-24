//HC-05分为有EN和有key两种：
//有EN的需要，在上电前按住按钮不放，再上电，且除了VCC,GND,RX,TX之外的引脚都不接
//有KEY的需要，在上电前将KEY脚与3.3V脚连接，INT脚不接
#include <SoftwareSerial.h>
//使用软件串口，能讲数字口模拟成串口
SoftwareSerial BT(10, 11);  
// Pin10为RX，接HC05的TXD
// Pin11为TX，接HC05的RXD
char val;  //存储接收的变量
 
void setup() {
  Serial.begin(38400);   //与电脑的串口连接，串口监视器波特率与蓝牙波特率一致
  Serial.println("BT is ready!");
  BT.begin(38400);  //设置蓝牙波特率为默认值
}
 
void loop() {
  //如果串口接收到数据，就输出到蓝牙串口
  if (Serial.available()) {
    val = Serial.read();
    BT.print(val);
  }
 
  //如果接收到蓝牙模块的数据，输出到屏幕
  if (BT.available()) {
    val = BT.read();
    Serial.print(val);
  }
}
//当发现串口监视器显示BT is ready!后，输入AT点击键盘回车一次，当看到OK的时候就说明可以进行AT模式设置了。
