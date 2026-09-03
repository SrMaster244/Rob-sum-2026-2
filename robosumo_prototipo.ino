// C++ code
#include <IRremote.hpp> 
//----------- VARIÁVEIS DO MOTOR---------------------
const int ATM1 = 10;   // Ativação MOTOR 1
const int ATM2 = 2;    // Ativação MOTOR 2
const int IN1 = 11; // Controle de polaridade MOTOR 1
const int IN2 = 12; // Controle de polaridade MOTOR 1
const int IN3 = 3;  // Controle de polaridade MOTOR 2
const int IN4 = 4;  // Controle de polaridade MOTOR 2
//----------- Variáveis Ultrasom --------------------
const float soundSpeed = 0.0343;
const int Ultra1 = 7;
const int Ultra2 = 8;
const int Echo1 = 6;
const int Echo2 = 9;
//--------------- OUTROS ---------------------
const int IRC = 5;
const int SenBorda = 13;
int IR_At = 0; // "Autorização" para iniciar
int Borda = 0; // CONTROLE DE AÇÃO


void setup()
{
  Serial.begin(9600);
  // SENSOR ULTRASOM
  pinMode(Ultra1, OUTPUT);
  pinMode(Ultra2, OUTPUT);
  pinMode(Echo1, INPUT);
  pinMode(Echo2, INPUT);
  // MOTOR
  pinMode(ATM1, OUTPUT);
  pinMode(ATM2, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(ATM1, HIGH);
  digitalWrite(ATM2, HIGH);
  // OUTROS
  randomSeed(analogRead(A0));
  IrReceiver.begin(IRC, ENABLE_LED_FEEDBACK);
  pinMode(SenBorda, INPUT);
}

void loop()
{
  // Ações primárias de controle
  if(IrReceiver.decode()){
    IR_At = 1;
  }
  //if(digitalRead(SenBorda) == HIGH){
    //Borda = true;
  //}
  // Lógica principal
  if(IR_At == 1){
   if(DistanciaSonar(Ultra1, Echo1) < 100 ||
      DistanciaSonar(Ultra2, Echo2) < 100)
   {
      Foward(10000);
   }
    else{ 
      TurnLeft(30000);
    }
  }
  if(Borda == 1){
    Backward(2000);
    Borda = false;
  }
  imprimirSerial(DistanciaSonar(Ultra1, Echo1),DistanciaSonar(Ultra2, Echo2));
  
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(250); 
}
float DistanciaSonar(int ultra, int echo){
  digitalWrite(ultra, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultra, LOW);
 
  
  float duracao = pulseIn(echo, HIGH);
  
  return (duracao * soundSpeed) / 2;
}
void imprimirSerial(float dist1, float dist2){
  Serial.print("Distancia1:");
  Serial.print(dist1);
  Serial.print("             ");
  Serial.print("Distancia2:");
  Serial.print(dist2);
  Serial.print("\n");
}
void Foward(int temp){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delayMicroseconds(temp);
  parar();
  
}
void Backward(int temp){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delayMicroseconds(temp);
  parar();
  
}
void TurnLeft(int temp){ 
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delayMicroseconds(temp);
  parar();
}
void TurnRight(int temp){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delayMicroseconds(temp);
  parar();
}
void parar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}