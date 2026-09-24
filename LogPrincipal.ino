// C++ code
#include <IRremote.hpp> 

//----------- VARIÁVEIS DO MOTOR (PINOS LIMPOS) ---------------------
const int IN1 = 10; 
const int IN2 = 4;  // Controle de polaridade MOTOR 1
const int IN3 = 2;  // Controle de polaridade MOTOR 2
const int IN4 = 3; // Controle de polaridade MOTOR 2

//----------- MÁQUINA DE ESTADOS --------------------
enum Estado {
  ready,
  start,
  stop,
  none
};

#define start_tecla 0x18
#define ready_tecla 0x0C
#define stop_tecla  0x5E

//----------- SENSORES DE BORDA -------------------
const int senborda_LEFT = 9;
const int senborda_RIGHT = 8;

//----------- VARIÁVEIS ULTRASSOM --------------------
const float soundSpeed = 0.0343;
const int Ultra1 = 6;
const int Echo1 = 7;
float distancia;

//--------------- OUTROS ---------------------
const int IRC = 5;
bool bot_stop = false;
bool borda_dir = false;
bool borda_esq = false;     
int direcaoGiro = 1; // 1 = Esquerda, -1 = Direita 

Estado estadoatual = none;

void setup() {
  Serial.begin(9600);
  
  // SENSOR ULTRASSOM
  pinMode(Ultra1, OUTPUT);
  pinMode(Echo1, INPUT);
  
  // MOTOR
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // SENSORES DE BORDA
  pinMode(senborda_LEFT, INPUT);
  pinMode(senborda_RIGHT, INPUT);

  // RECEPTOR IR
  randomSeed(analogRead(A0));
  IrReceiver.begin(IRC, ENABLE_LED_FEEDBACK); 
}

void loop() {
  distancia = DistanciaSonar();

  // LEITURA DO CONTROLE IR
  if (IrReceiver.decode()) {
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      
      uint16_t comando = IrReceiver.decodedIRData.command;
      if (comando == 0x0) {
        comando = (uint16_t) IrReceiver.decodedIRData.decodedRawData;
      }

      Serial.print("Comando Recebido: 0x");
      Serial.println(comando, HEX);

      if (!bot_stop) {
        switch (comando) {
          case ready_tecla:        
            estadoatual = ready;
            break;

          case start_tecla:      
            estadoatual = start;
            break;

          case stop_tecla:
            estadoatual = stop;
            break;
        }
      }
    }
    IrReceiver.resume();
  } 

  // MÁQUINA DE ESTADOS
  switch (estadoatual) {
    case ready:
      parar();
      Serial.print(" READY ");
      break;

    case start:
      combate();
      break;

    case stop:
    case none:
    default:
      parar();
      Serial.print(" PARADO ");
      break;
  }
  
  imprimirSerial(distancia);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50); // Reduzido de 100 para 50ms para maior agilidade dos sensores
}

void combate() {
  // Leitura dos Sensores de Borda (LOW = Linha Branca)
  if (digitalRead(senborda_LEFT) == LOW)  borda_esq = true;
  if (digitalRead(senborda_RIGHT) == LOW) borda_dir = true;

  // Lógica de Evasão de Borda
  if (borda_esq && borda_dir) {
    Backward();
    delay(300); // Recuo rápido sem congelar a rotina básica
    borda_esq = false;
    borda_dir = false;
  }
  else if (borda_esq) {
    Backward();
    delay(150);
    TurnRight();
    delay(200);
    borda_esq = false;
  }
  else if (borda_dir) {
    Backward();
    delay(150);
    TurnLeft();
    delay(200);
    borda_dir = false;
  }
  // Lógica de Caça e Ataque via Ultrassom
  else {
    float dist = DistanciaSonar();
    if (dist < 30 && dist > 0) {
      Foward();
      Serial.print(" ATACANDO ");
    }
    else {
      if (direcaoGiro == 1) TurnLeft();
      else TurnRight();
      Serial.print(" BUSCANDO ");
    }
  }
}

float DistanciaSonar() {
  digitalWrite(Ultra1, LOW);
  delayMicroseconds(2);
  digitalWrite(Ultra1, HIGH);
  delayMicroseconds(10);
  digitalWrite(Ultra1, LOW);
  
  float duracao = pulseIn(Echo1, HIGH, 25000); 
  return (duracao * soundSpeed) / 2;
}

void imprimirSerial(float dist1) {
  Serial.print(" | Distancia: ");
  Serial.print(dist1);
  Serial.println(" cm");
}

void Backward() {
  digitalWrite(IN1, LOW);  // Roda Esquerda: FRENTE
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); // Roda Direita: FRENTE
  digitalWrite(IN4, HIGH);
}

void Foward() {
  digitalWrite(IN1, HIGH); // Roda Esquerda: TRÁS
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // Roda Direita: TRÁS
  digitalWrite(IN4, LOW);
}

void TurnRight() { 
  digitalWrite(IN1, HIGH); // Roda Esquerda: TRÁS
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); // Roda Direita: FRENTE
  digitalWrite(IN4, HIGH);
}

void TurnLeft() {
  digitalWrite(IN1, LOW);  // Roda Esquerda: FRENTE
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  // Roda Direita: TRÁS
  digitalWrite(IN4, LOW);
}

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}