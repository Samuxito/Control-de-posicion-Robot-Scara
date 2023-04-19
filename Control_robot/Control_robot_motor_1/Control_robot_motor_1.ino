#include <Wire.h>
#include <analogWrite.h>
#include "INA226.h"

INA226 INA;
// Definiciones componentes de la tarjeta
#define salida 25 //Salida analogica para retroalimentacion del sistema
#define IN1 26 // Lado positivo 
#define IN2 27 // Lado negativo
const byte Encoder_Cl = 16; // Entrada de Encoder A
const byte Encoder_C2 = 17; // Entrada de Encoder B

//Variables Globales
volatile float corriente = 0.0;
double u = 0.0, u_1 = 0.0, u_2 = 0.0; //Acción de Control
byte Ts = 1000; //Periodo de muestreo
float dec = 0; //Referencia de entrada
int cond = 1, cont = 0, sum_dec = 0;
char control_dec[5] = {}, tipo_control;
String vals_control;

// ------------------ Variables para definicion de angulo y velocidad del motor ------------------
byte Encoder_C1Last;
int paso = 0, paso_ang = 0;
boolean direccion;
//int direccion = 0;
float rpm = 0;
unsigned long timeold = 0;
char decicion;
int volt = 0;
float r1 = 0;

float kp = 0, kd = 0, RefV = 0.0, EV = 0, EP = 0, REFCORRIENTE = 0;
String Data;
int var = 0;

/* 2.) Control Discreto - Discreto */
float q0 = 5.34, q1 = -5.064, s0 = -0.9671;

volatile float e = 0.0, e_1 = 0.0, e_2 = 0.0;

// ------------------------------ Variables de interrupcion para INA226 ------------------------------
volatile int interruptCounter = 0;
int totalInterruptCounter;

// ------------------------------------- Interrupción  INA 226 -------------------------------------
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// ------------------------------ Variables de interrupcion para encoder ------------------------------
volatile int interruptCounter_encoder = 0;
hw_timer_t * timer_encoder = NULL;
portMUX_TYPE timerMux_encoder = portMUX_INITIALIZER_UNLOCKED;

// ------------------------------------- Interrupción  encoder -------------------------------------
void IRAM_ATTR onTimer_encoder() {
  portENTER_CRITICAL_ISR(&timerMux_encoder);
  interruptCounter_encoder = 1;
  portEXIT_CRITICAL_ISR(&timerMux_encoder);
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  //Configuramos el puerto serial
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(16), calculapulso, CHANGE);
  pinMode(salida, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  INA.begin(); // Default INA226 address is 0x40
  INA.configure(INA226_AVERAGES_4, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT); // Configure INA226
  INA.calibrate(0.1, 2); // Calibrate INA226. Rshunt = 0.1 ohm, Max excepted current = 1A

  // Configuracion Time Corriente
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);

  // Configuracion Timer Encoder
  timer_encoder = timerBegin(1, 80, true);
  timerAttachInterrupt(timer_encoder, &onTimer_encoder, true);
  timerAlarmWrite(timer_encoder, 100000, true);
  timerAlarmEnable(timer_encoder);
}
void loop() {
  if (Serial.available() > 0) {
    Data = Serial.readString();
    var = Data.toInt();
  }

  pinMode(Encoder_C2, INPUT);

  if (interruptCounter >= 1) {
    kp = 0.008; // Motor Verde
    kd = 0.001 ; // Motor Verde
    RefV = 0.0;
    EV = RefV - rpm;
    EP = var - (paso_ang * 0.45); //0.45 2.5
    r1 = (kp * (EP) + kd * (EV));
    pinMode(Encoder_C2, INPUT);
    Serial.print("EV: ");
    Serial.print(EV);
    Serial.print(", EP: ");
    Serial.print(EP);
    Serial.print(", var: ");
    Serial.print(var);
    Serial.print(", Ang: ");
    Serial.print(paso_ang * 0.45);
    Serial.print(", r1: ");
    Serial.print(r1);
    corriente = INA.readShuntCurrent();
    Serial.print(", Amp: ");
    Serial.print(corriente);
    if (corriente >= -0.04 && corriente <= 0.04) {
      corriente = 0;
    }
    e = (r1 - corriente);

    /*u(k) = q0*e(k) + q1*e(k-1) - (so-1)*u(k-1) + so*u(k-2)*/
    u = q0 * e + q1 * e_1 - (s0 - 1) * u_1 + s0 * u_2;
    //u = 0.02444 * e - 0.0299 * e_1 + 0.0099 * e_2 + 1.9215 * u_1 - 0.9215 * u_2;
    Serial.print(", u: ");
    Serial.println(u);
    if (u >= 9) {       //Saturo la accion de control 'uT' en un tope maximo y minimo
      u = 9;
    }
    if (u <= -9) {       //Saturo la accion de control 'uT' en un tope maximo y minimo
      u = -9;
    }
    //Retorno a los valores reales
    e_2 = e_1;
    e_1 = e;
    u_2 = u_1;
    u_1 = u;
    //    Serial.print(u);
    //    Serial.print(", ");
    if (interruptCounter_encoder >= 1) {
      //rpm = paso * (2.4 / (millis() - timeold)) * (60000 / 360); // Samuel
      rpm = paso * (0.45 / (millis() - timeold)) * (60000 / 360); // Nicolas
      timeold = millis();
      paso = 0;
      interruptCounter_encoder = 0;
      if (paso_ang * 0.45 > 2000 || paso_ang * 0.45  < -2000) {
        paso_ang = 0;
      }
    }
    if (u <= 9 && u >= 0.0) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      u = map(u, 0, 9, 0, 255);
      analogWrite(salida, u);
    }
    if (u >= -9 && u <= 0.0) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      u = map(u, 0, -9, 0, 255);
      analogWrite(salida, u);
    }
    interruptCounter = 0;
  }
}

// Funcion para definicion de direccion de motor
void calculapulso() {
  int Lstate = digitalRead(Encoder_Cl);
  if ((Encoder_C1Last == LOW) && Lstate == HIGH) {
    int val = digitalRead(Encoder_C2);
    if (val == LOW && direccion) {
      direccion = false; // Izquierda
    }
    else if (val == HIGH && !direccion) {
      direccion = true; // Derecha
    }
  }
  Encoder_C1Last = Lstate;
  if (!direccion) {
    paso++;
    paso_ang++;
  }
  else {
    paso--;
    paso_ang--;
  }
}
