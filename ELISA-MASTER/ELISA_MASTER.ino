#include<Wire.h>
#include<SoftwareSerial.h>
#include<MPU6050_6Axis_MotionApps20.h>
#include<hardwareEnclosure.h>
#include<gButton.h>
#include<gLed.h>
//#include<gMatrix.h>

#define intPin 2
#define calTime 8000
#define bitENABLE 0
#define bitRIGHT  1
#define bitLEFT   2
#define bStatus   13

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void beginMPU();
void readSensors();

volatile bool mpuInt;
void dmpDataReady();

MPU6050 mpu;
SoftwareSerial bt(10,11);
gButton enable(3,true), right(4,true), left(5,true);

//gMatrix Rx(3,3), Ry(3,3), Rz(3,3), Rt(3,3);
//gMatrix p(3), pr(3);

uint16_t packetSize;
uint8_t  intStatus;
uint16_t fifoCount;
uint8_t  fBuffer[64];
unsigned long lastTime;
unsigned long lastTime2;
bool     auxFlag;
uint16_t data[6];
uint8_t  btBuffer[16] = {'$', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\r', '\n'};

Quaternion q;
VectorFloat gravity;
float angle[3];
float pos[3];
float offsetAngle[3];

  void setup(){
    Wire.begin();
    Wire.setClock(400000);
    Serial.begin(115200);
    bt.begin(38400);
    beginAll();

    pinMode(intPin, INPUT);

    beginMPU();
    
    lastTime = millis();
    lastTime2 = millis();
    offsetAngle[0] = 0;
    offsetAngle[1] = 0;
    offsetAngle[2] = 0;

    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;

    auxFlag = true;

    while(auxFlag){
//      Serial.println(millis() - lastTime);
      while(!mpuInt && fifoCount<packetSize){}
      
      readSensors();
      
      if(millis() - lastTime  > calTime){
      offsetAngle[0] = angle[0]; 
      offsetAngle[1] = angle[1]; 
      offsetAngle[2] = angle[2];
      
      auxFlag = false;
      }
    }
}


void loop(){

  updateAll();
  while(!mpuInt && fifoCount<packetSize){}  
  readSensors();

//  Serial.print(0);
//  Serial.print("\t");
//  Serial.print(0);
//  Serial.print("\t");
//  Serial.print(0);
//  Serial.print("\t \t");
//  Serial.print(angle[2]);
//  Serial.print("\t");
//  Serial.print(angle[1]);
//  Serial.print("\t");
//  Serial.println(angle[0]);

//  bt.print(0);
//  bt.print('a');
//  bt.print(0);
//  bt.print('b');
//  bt.print(0);
//  bt.print('c');
//  bt.print(angle[2]);
//  bt.print('d');
//  bt.print(angle[1]);
//  bt.print('e');
//  bt.print(angle[0]);
//  bt.print('$');


  data[0] = uint16_t(pos[0]) * 65535 / 150;
  data[1] = uint16_t(pos[1]) * 65535 / 150;
  data[2] = uint16_t(pos[2]) * 65535 / 150;

  data[3] = uint16_t(fmap(angle[2], -PI/2.0, PI/2.0, 0, 65535));
  data[4] = uint16_t(fmap(angle[1], -PI/2.0, PI/2.0, 0, 65535));
  data[5] = uint16_t(fmap(angle[0], -PI, PI, 0, 65535));

  btBuffer[1]  = data[0] >> 8;
  btBuffer[2]  = data[0];
  btBuffer[3]  = data[1] >> 8;
  btBuffer[4]  = data[1];
  btBuffer[5]  = data[2] >> 8;
  btBuffer[6]  = data[2];
  btBuffer[7]  = data[3] >> 8;
  btBuffer[8]  = data[3];
  btBuffer[9]  = data[4] >> 8;
  btBuffer[10] = data[4];
  btBuffer[11] = data[5] >> 8;
  btBuffer[12] = data[5];
  
  bitWrite(btBuffer[13], bitENABLE, enable.click());
  bitWrite(btBuffer[13], bitRIGHT,  right.sustained());
  bitWrite(btBuffer[13], bitLEFT,   left.sustained());

  if(millis() - lastTime2 > 20){
    bt.write(btBuffer, 16);
//Serial.println(left.sustained());
//    Serial.print(pos[0]);
//    Serial.print("\t");
//    Serial.print(pos[1]);
//    Serial.print("\t");
//    Serial.print(pos[2]);
//    Serial.print("\t\t");
//    Serial.print(angle[0]);
//    Serial.print("\t");
//    Serial.print(angle[1]);
//    Serial.print("\t");
//    Serial.println(angle[2]);
    
    lastTime2 = millis();
  }
  
//  Serial.print(data[3], BIN);
//  Serial.print("\t");
//  Serial.print(btBuffer[7],BIN);
//  Serial.println(btBuffer[8],BIN);

}











void beginMPU(){
  mpu.initialize();

  mpu.dmpInitialize();

  mpu.setXAccelOffset(-2622);
  mpu.setYAccelOffset(2593);
  mpu.setZAccelOffset(671);
  mpu.setXGyroOffset(105);               
  mpu.setYGyroOffset(6);
  mpu.setZGyroOffset(0);

  mpuInt = false;
  
  mpu.setDMPEnabled(true);

  attachInterrupt(digitalPinToInterrupt(intPin), dmpDataReady, RISING);

  intStatus = mpu.getIntStatus();

  packetSize = mpu.dmpGetFIFOPacketSize();
}

void dmpDataReady(){
  mpuInt = true;
}

void readSensors(){
  mpuInt = false;
  
  intStatus = mpu.getIntStatus();

  fifoCount = mpu.getFIFOCount();

  if ((intStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
  }
  else if (intStatus & 0x02) {
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

      mpu.getFIFOBytes(fBuffer, packetSize);
      fifoCount -= packetSize;

      mpu.dmpGetQuaternion(&q, fBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(angle, &q, &gravity);

      angle[0] *= -1;

      angle[0] -= offsetAngle[0];
      angle[1] -= offsetAngle[1];
      angle[2] -= offsetAngle[2];   

//      Rz(0,0) = cos(angle[0]); Rz(0,1) = -sin(angle[0]); Rz(0,2) = 0,
//      Rz(1,0) = sin(angle[0]); Rz(1,1) = cos(angle[0]);  Rz(1,2) = 0;
//      Rz(2,0) = 0;              Rz(2,1) = 0;               Rz(2,2) = 1;
//
//      Ry(0,0) = cos(angle[1]);  Ry(0,1) = 0; Ry(0,2) = sin(angle[1]),
//      Ry(1,0) = 0;              Ry(1,1) = 1; Ry(1,2) = 0;
//      Ry(2,0) = -sin(angle[1]); Ry(2,1) = 0; Ry(2,2) = cos(angle[1]);
//
//      Rx(0,0) = 1; Rx(0,1) = 0;              Rx(0,2) = 0,
//      Rx(1,0) = 0; Rx(1,1) = cos(angle[2]); Rx(1,2) = -sin(angle[2]);
//      Rx(2,0) = 0; Rx(2,1) = sin(angle[2]); Rx(2,2) = cos(angle[2]); 
//
//      p(0) = 0;
//      p(1) = 1;
//      p(2) = 0;
//
//      Rt = Ry * Rx;
//      Rt = Rz * Rt;
//
//      pr = Rt * p;
  }
}











