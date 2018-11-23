// Basic Bluetooth sketch HC-05_02_9600+ECHO
// Connect the HC-05 module and communicate using the serial monitor
//
// The HC-05 defaults to commincation mode when first powered on.
// The default baud rate for communication mode is 9600. Your module may have a different speed.
//
#include <SoftwareSerial.h>
SoftwareSerial BTserial(2,3);// RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX. 
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider.
char c =' ';
void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready");
// HC-05 default serial speed for communication mode is 38400
    BTserial.begin(38400);
    Serial.println("BTserial started at 38400");
//   HC-05 default serial speed for communication mode is 9600
//    BTserial.begin(9600);
//    Serial.println("BTserial started at 9600");
}
void loop(){
// Keep reading from HC-05 and send to Arduino Serial Monitor
if(BTserial.available())
{
  c =BTserial.read();
  Serial.write(c);
}
// Keep reading from Arduino Serial Monitor and send to HC-05
if(Serial.available())
{
 c =Serial.read();
// Copy the serial data back to to the serial monitor. 
// This makes it easy to follow the commands and replies
   Serial.write(c);
   BTserial.write(c);
}
}
