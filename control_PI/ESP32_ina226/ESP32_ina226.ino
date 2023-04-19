#include <Wire.h>
#include <analogWrite.h>
#include "INA226.h"

INA226 INA;
// Definiciones componentes de la tarjeta
#define REF 25 //Salida analogica para retroalimentacion del sistema
#define mas 16 // Lado positivo 
#define p 17 // Lado negativo
//Variables Globales
volatile float salida = 0.0;
double u = 0.0, u_1 = 0.0, u_2 = 0.0; //Acción de Control
byte Ts = 1000; //Periodo de muestreo
float r1 = 0.2, dec = 0; //Referencia de entrada
int cond = 1, cont = 0, sum_dec = 0;
char control_dec[5] = {}, tipo_control;
String vals_control;

/* 1.) Control Continuo - Discreto */
//float q0 = 0.4014/2, q1 = -0.3820/2, s0 = -1.0004;

/* 2.) Control Discreto - Discreto */
float q0 = 5.34, q1 = -5.064, s0 = -0.9671;

/* 3.) Control LGR */
//float K = 1.6113, q0 = -0.9528, q1 = -0.9923, q2 = q1*q2, s0 = -0.9528;

/* 4.) Anulacion de Planta */
//float q0 = 0.068342/2, q1 = -0.06512/2, s0 = -0.9923;

/* 5.) Tiempo de respuesta minimo */
//float q0 = 6.87, q1 = -6.54, s0 = -0.9923;

volatile float e = 0.0, e_1 = 0.0, e_2 = 0.0;

volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  //Configuramos el puerto serial
  Serial.begin(115200);
  pinMode(REF, OUTPUT);
  pinMode(mas, OUTPUT);
  pinMode(p, OUTPUT);

  INA.begin(); // Default INA226 address is 0x40
  INA.configure(INA226_AVERAGES_4, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT); // Configure INA226
  INA.calibrate(0.1, 2); // Calibrate INA226. Rshunt = 0.1 ohm, Max excepted current = 1A

  // Timer Configuration
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}
void loop() {
  if (Serial.available() > 0) {
    for (int i = 0; i <= 4; i++) {
      control_dec[i] = Serial.read(); // ultimo valor define el tipo de control
    }
    for (int j = 0; j < 4; j++) {
      vals_control += control_dec[j];
    }
    tipo_control = control_dec[4];
    dec = vals_control.toFloat(); // Valor de referencia de entrada
    Serial.println(dec);
  }
  if (interruptCounter >= 1) {
    if (r1 >= 0.2) {
      cont--;
    }
    if (r1 <= -0.2) {
      cont++;
    }
    if (cont >= 120) {
      r1 = r1 + 0.002;
    }
    if (cont <= -120) {
      r1 = r1 - 0.002;
    }

    salida = INA.readShuntCurrent();
    e = (r1 - salida);

    /*u(k) = q0*e(k) + q1*e(k-1) - (so-1)*u(k-1) + so*u(k-2)*/
    u = q0 * e + q1 * e_1 - (s0 - 1) * u_1 + s0 * u_2;

    /*u(k) = q0*e(k) + q1*e(k - 1) + q2*e(k - 2) + b0*u(k - 1) - b1*u(k - 2)*/
    //u = q0 * e - q1 * e_1 + q2 * e_2 - (s0 - 1) * u_1 + s0 * u_2;

    //Retorno a los valores reales
    e_2 = e_1;
    e_1 = e;
    u_2 = u_1;
    u_1 = u;
    //        Serial.print(u);
    //        Serial.print(", ");
    if (u >= 12) {       //Saturo la accion de control 'uT' en un tope maximo y minimo
      u = 12;
    }
    if (u <= -12) {       //Saturo la accion de control 'uT' en un tope maximo y minimo
      u = -12;
    }
    if (u <= 12 && u >= 0.0) {
      digitalWrite(mas, HIGH);
      digitalWrite(p, LOW);
      u = map(u, 0, 12, 0, 255);
      analogWrite(REF, u);
    }
    if (u >= -12 && u <= 0.0) {
      digitalWrite(mas, LOW);
      digitalWrite(p, HIGH);
      u = map(u, 0, -12, 0, 255);
      analogWrite(REF, u);
    }
    interruptCounter = 0;
    //        Serial.print(r1);
    //        Serial.print(", ");
    //        Serial.print(salida);
    //        Serial.print(", ");
    //        Serial.print(e);
    //        Serial.print(", ");
    //        Serial.println(u);
  }
}
