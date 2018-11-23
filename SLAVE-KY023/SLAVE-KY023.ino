#include <SoftwareSerial.h>
#include "Mouse.h"

SoftwareSerial BTserial(10, 11);

int state = 0;
int responseDelay = 5;        // response delay of the mouse, in ms
char aux;
  int coord[3];

void setup() {
  Serial.begin(9600);
  BTserial.begin(38400);

  // initialize digital pin 8 as an output.
  pinMode(8, OUTPUT);

  // take control of the mouse:
  Mouse.begin();

}

void loop() {
  int a;
  int b;
  String data;
  int d = 20;
  
  delay(100);
  
  if(BTserial.available()){
    while(true){
      if(BTserial.available()){
        aux=BTserial.read();
        data += aux;
        if(aux=='$')
          break;
      }   
    }
  }

  a = data.indexOf('a');
  b = data.indexOf('b');

  coord[0] = data.substring(0,a).toInt();
  coord[1] = data.substring(a+1,b).toInt();

  Serial.print(coord[0]);
  Serial.print("\t");
  Serial.println(coord[1]);

  if(coord[0] > 1300){
    Mouse.move(d,0,0);
  }
   if(coord[0] < 680){
    Mouse.move(-d,0,0);
   }
    if(coord[1] > 700){
    Mouse.move(0,d,0);
    }
   if(coord[1] < 300){
    Mouse.move(0,-d,0);
   }

  //Mouse.move(coord[0],coord[1],0);
  
//  int coord[3];
//  
//  delay(responseDelay);
//  
//  if (BTserial.available() > 0) { // Checks whether data is comming from the serial port
//
//  while(true){
//    
//    }
//    else{
//      data+=aux;
//    }
//  }
//
//  //Mouse.move(coord[0],coord[1],0);
//    
////    state = BTserial.read();      // Reads the data from the serial port
//  }
//
//   Serial.println(data);
    
//  // Controlling the LED
//  if (state == '1') {
//    digitalWrite(8, HIGH); // LED ON
//    state = 0;
//  }
//  else if (state == '0') {
//    digitalWrite(8, LOW); // LED ON
//    state = 0;
//  }
}
