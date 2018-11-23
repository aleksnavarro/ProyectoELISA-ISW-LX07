#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11);  // Starts bluetooth pins in digital 10 and 11

int state = 0;                    
const int ledPin = 8;             // The pin your led is connected to
const int buttonPin = 2;          // The pin your button is connected to

int JoyStick_X = A0;              // x axis, hint: connect to vry in joystick
int JoyStick_Y = A1;              // y axis, hint: connect to vrx in joystick
int JoyStick_Z = A3;              // key

int buttonState = 0;              // Initial state of the button

void setup() 
{
  // The following three lines marks the pinmodes of the joystick axis and button
  pinMode (JoyStick_X, INPUT);
  pinMode (JoyStick_Y, INPUT);
  pinMode (JoyStick_Z, INPUT_PULLUP);

  // Start the serial modules, 9600 baud for monitor and 38400 baud for bluetooth ¡¡which both baud for bluetooth modules must agree!!
  Serial.begin(9600);
  BTserial.begin(38400);

  // Pinmode for led and button
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

}

void loop() 
{
  int coord[3];                    // Array of int for coordinates of joystick
  buttonState = digitalRead(buttonPin); // State of button

  // Get the coordinates from the joystick, and then storage them in the array coord
  coord[0] = analogRead (JoyStick_X);
  coord[1] = analogRead (JoyStick_Y);
  coord[2] = digitalRead (JoyStick_Z);

  coord[0]=map(coord[0],0,1024,0,1366);
  coord[1]=map(coord[1],1024,0,0,768);
  
  // Prints in serial monitor the valuesi n the array coord and sends it to bluetooth
  Serial.print (coord[0], DEC);
  Serial.print (",");
  Serial.print (coord[1], DEC);
  Serial.print (",");
  Serial.println (coord[2], DEC);
  

  BTserial.print(coord[0]);
  BTserial.print("a");
  BTserial.print(coord[1]);
  BTserial.print("b");
  BTserial.print(coord[2]);
  BTserial.print("$");

//  BTserial.write(coord[0]);
//  BTserial.write("a");
//  BTserial.write(coord[1]);
//  BTserial.write("b");
//  BTserial.write(coord[2]);
//  BTserial.write("$");
  
  delay (100);

//  if (buttonState == HIGH) {
//    digitalWrite(ledPin, HIGH);
//    delay(20);
//    BTserial.write('1');  //sends a 1 through the bluetooth serial link
//    delay (20);
//  }
//  else {
//    BTserial.write('0');
//    digitalWrite(ledPin, LOW);
//    delay(20);
//  }
}
