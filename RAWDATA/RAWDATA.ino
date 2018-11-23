#include <SoftwareSerial.h>
#include <gMatrix.h>

SoftwareSerial BTserial(10,11);

float raw[6];         //Vector de información raw, recibido desde Master
float coord[2];       //Vector de información con coordenadas en pantalla, tras procesar raw
int D=10;             //Distancia de la pantalla virtual

void setup() {
}

void loop() {
}

void RawData(float raw[]){
/*
  La función RawData permite procesar la información desde el vector raw, recibido vía bluetooth
  desde Master, para que este en un formato comprensible.  
*/
 
  char aux;           //Carácter auxiliar
  int a,b,c,d,e;      //Enteros auxiliares
  String data;        //Cadena de datos que guardará la información procesada

  delay(100);         //Un retraso de tiempo entre lectura y proceso

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
  Serial.print(raw[0]);
  Serial.print(",");
  Serial.print(raw[1]);
  Serial.print(",");
  Serial.print(raw[2]);
  Serial.print(";");
  Serial.print(raw[3]);
  Serial.print(",");
  Serial.print(raw[4]);
  Serial.print(",");
  Serial.print(raw[5]);
  
}

void Intersect(float raw[],float coord[]){
 /*
  La función Intersect permite calcular un rayo que intersecta con una pantalla virtual
  en forma de una coordenada.
*/
  gMatrix Rx(3,3),Ry(3,3),Rz(3,3);                            //Matrices de rotación
  gMatrix Gam(3,3),GamDag(3,3),Tvec(1,3);                     //Matrices Gama, Gama Daga y vector T para calcular Gama Daga
  gMatrix GamDagx(3,1),GamDagy(3,1);                          //Componentes Gama Daga X y Y
  gMatrix Pauli(2,2),Tau(1,2),prm(1,2),TauDag(2,1);           //Matriz de traslación (Pauli), vetor Tau, de parámetros alfa y beta y Tau Daga 
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
  Gam=Rz*(Ry*Rx);
  //Calcula el valor de T
  T=(D-(Gam(2,0)*raw[0])-(Gam(2,1)*raw[1])-(Gam(2,2)*raw[2]))/Gam(2,1);
  //Vector T, T más los componentes X,Y,Z en raw
  Tvec(0,0)=raw[0]; Tvec(1,0)=(T+raw[1]); Tvec(2,0)=raw[2];
  //Calcula Gama Daga a partir de Gama y Vector T
  GamDag=Gam*Tvec;
  //Extrae las componentes X y Y de Gama y las guarda en Gama Daga X y Y respectivamente
  GamDagx(0,0)=GamDag(0,0);  GamDagx(0,1)=GamDag(0,1);  GamDagx=GamDag(0,2);
  GamDagy(0,0)=GamDag(0,0);  GamDagy(0,1)=GamDag(0,1);  GamDagy=GamDag(0,2);
  //Calcula los parámetros alfa, para una pantalla de 1366x768 pixeles, con 38x38 ppc (96x96 ppp)
  //!!Revisar advertencia de integer overflow!!
  alpha=(1366*38)-(1366/2); betha=(768*38)-(768/2);
  prm(0,0)=alpha; prm(0,1)=betha;
  //Hace la matriz de traslación, posiciona el origen en la esquina superior de la pantalla
  Pauli(0,0)=1; Pauli(0,1)=0;
  Pauli(1,0)=0; Pauli(1,1)=-1;
  //Tau Daga, Tau con la traslación y coordenadas útiles.
  TauDag=Pauli*Tau+prm;
  //Vector de coordenadas, para posicionar el puntero en pantalla.
  coord[0]=TauDag(0,0); coord[1]=TauDag(1,0);
  
}
