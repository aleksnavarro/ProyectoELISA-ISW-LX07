#include <SoftwareSerial.h>
#include <gMatrix.h>
#include "Mouse.h"

SoftwareSerial BTserial(10,11);

float raw[6];         //Vector de información raw, recibido desde Master
float coord[2],coord_bak[2];       //Vector de información con coordenadas en pantalla, tras procesar raw
int D=100;             //Distancia de la pantalla virtual, cm
int DPC=4;            // Pixeles por cm

void setup() {
  Serial.begin(9600);
  BTserial.begin(38400); 

  coord_bak[0]=0; coord_bak[1]=0;

  Mouse.begin();
}

void loop() {
  RawData(raw);
  Intersect(raw,coord);

  Mouse.move(coord[0]-coord_bak[0],coord[1]-coord_bak[1],0);

  coord_bak[0]=coord[0];
  coord_bak[1]=coord[1];
  
}

void RawData(float raw[]){
/*
  La función RawData permite procesar la información desde el vector raw, recibido vía bluetooth
  desde Master, para que este en un formato comprensible.  
*/
 
  char aux;           //Carácter auxiliar
  int a,b,c,d,e;      //Enteros auxiliares
  String data;        //Cadena de datos que guardará la información procesada

  delay(50);         //Un retraso de tiempo entre lectura y proceso

  //El siguiente condicional lee la información sí existe un dispositivo BTserial
  //Concatena la cadena aux en data hasta que el delimitador "$" aparece
  if(BTserial.available()){
    while(true){
      if(BTserial.available()){
        aux=BTserial.read();
        data += aux;
        if(aux=='$'){break;}
      }
    }
  }

  //Serial.println(data);

  //De la cadena data extrae la información hasta que el delimitador indicado aparesca
  //!!Revisar la documentación https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/indexof/ !!
  a=data.indexOf('a');
  b=data.indexOf('b');
  c=data.indexOf('c');
  d=data.indexOf('d');
  e=data.indexOf('e');

  //Extrae la subcadina entre los límites indicados y lo convierte a float, luego es guardado en raw
  raw[0]=data.substring(0  ,a).toFloat();
  raw[1]=data.substring(a+1,b).toFloat();
  raw[2]=data.substring(b+1,c).toFloat();
  raw[3]=data.substring(c+1,d).toFloat();
  raw[4]=data.substring(d+1,e).toFloat();
  raw[5]=data.substring(e+1,data.length()-1).toFloat();

  //Auxilio de impresión en serial, para verificar la integridad de raw
//  Serial.print(raw[0]);
//  Serial.print("\t");
//  Serial.print(raw[1]);
//  Serial.print("\t");
//  Serial.print(raw[2]);
//  Serial.print("\t\t");
//  Serial.print(raw[3]);
//  Serial.print("\t");
//  Serial.print(raw[4]);
//  Serial.print("\t");
//  Serial.println(raw[5]);
  
}

void Intersect(float raw[],float coord[]){
 /*
  La función Intersect permite calcular un rayo que intersecta con una pantalla virtual
  en forma de una coordenada.
*/
  gMatrix Rx(3,3),Ry(3,3),Rz(3,3),Rt(3,3);                    //Matrices de rotación y su multiplicación final
  gMatrix Gam(3),GamDag(3),Tvec(3);                           //Matrices Gama, Gama Daga y vector T para calcular Gama Daga
  gMatrix Pauli(2,2),Tau(2),prm(2),TauDag(2);           //Matriz de traslación (Pauli), vetor Tau, de parámetros alfa y beta y Tau Daga 
  float T,alpha,betha;                                        //Numero T calculado a partir de Gamma Daga Z y parámetros alfa y beta
  

  //Matrices de rotación, con los angulos existentes en raw
  Rx(0,0)=1;  Rx(0,1)=0;            Rx(0,2)=0;
  Rx(1,0)=0;  Rx(1,1)=cos(raw[3]);  Rx(1,2)=-sin(raw[3]);
  Rx(2,0)=0;  Rx(2,1)=sin(raw[3]);  Rx(2,2)=cos(raw[3]);

  Ry(0,0)=cos(raw[4]);  Ry(0,1)=0;  Ry(0,2)=sin(raw[4]);
  Ry(1,0)=0;            Ry(1,1)=1;  Ry(1,2)=0;
  Ry(2,0)=-sin(raw[4]); Ry(2,1)=0;  Ry(2,2)=cos(raw[4]);

  Rz(0,0)=cos(raw[5]);  Rz(0,1)=-sin(raw[5]); Rz(0,2)=0;
  Rz(1,0)=sin(raw[5]);  Rz(1,1)=cos(raw[5]);  Rz(1,2)=0;
  Rz(2,0)=0;            Rz(2,1)=0;            Rz(2,2)=1;

  //Calcula Gama con las matrices de rotación
  Rt=Rz*(Ry*Rx);
  //Calcula el valor de T, y las componentes de Tau
  T=(D-(Rt(1,0)*raw[0])-(Rt(1,1)*raw[1])-(Rt(1,2)*raw[2]))/Rt(1,1);
  Tau(0)=Rt(0,0)*raw[0]+Rt(0,1)*(T+raw[1])+Rt(0,2)*raw[2];
  Tau(1)=Rt(2,0)*raw[0]+Rt(2,1)*(T+raw[1])+Rt(2,2)*raw[2];
//  Serial.print(Tau(0));
//  Serial.print("\t");
//  Serial.println(Tau(1));
  //Serial.println(Rt(1,1));
  //Calcula los parámetros alfa, para una pantalla de 1366x768 pixeles, con 38x38 ppc (96x96 ppp)
  //!!Revisar advertencia de integer overflow!!
  alpha=(1366/2); betha=(768/2);
  prm(0)=alpha; prm(1)=betha;
  //Hace la matriz de traslación, posiciona el origen en la esquina superior de la pantalla
  Pauli(0,0)=1; Pauli(0,1)=0;
  Pauli(1,0)=0; Pauli(1,1)=-1;
  //Calcula TauDag
  TauDag=(DPC*Pauli*Tau)+prm;
  //Coordenadas finales
  coord[0]=TauDag(0); coord[1]=TauDag(1);

  Serial.print(coord[0]);
  Serial.print("\t");
  Serial.println(coord[1]);
  
}
