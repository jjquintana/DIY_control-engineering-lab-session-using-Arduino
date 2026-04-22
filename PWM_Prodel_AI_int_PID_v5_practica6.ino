/*
Autor: José J. Quintana
Fecha creación: 10/05/2021
Fichero: PWM_Prodel_AI_int_v4
Universidad de Las Palmas de Gran Canaria

Programa base para la utilización del arduino uno como elemento de control, en
la asignatura de Regulación Automática del grado de Ingeniería en Electrónica
Industrial y Automática.
Utiliza una interrupción para un periodo de muestreo ajustable (se ha ajustado a 1s.
Utiliza las entradas analógicas y salidas PWM del arduino.

Implementa:
Un control manual en que La salida PWM (lámpara) sigue al potenciómetro de consigna 
del sistema térmico (entrada A1).
Un control todo-nada.
Un control todo-nada con histéresis.
Un control proporcional.
*/

// Se incluyen las librerías. 
#include "TimerOne.h" // Se necesita para utilizar la interrupción de tiempo


// Se definen las entradas y salidas
const int Pin_PWM_11 = 11;

const int Pin_Ref_A0 = A0;// Entrada de la referencia de temperatura
const int Pin_Tra_A1 = A1;// Entrada del sensor de temperatura

long tiempo; // variable para almacenar el tiempo

int valor_tra, valor_ref, salida_manual, u_int;
float temperatura, referencia;
float error, u_float, yp, yd, yI, h=0.5;
float k=20, Td=2, Ti=50, T=1;
float error1=0, yI1=0;

boolean flag_timer=false;


void setup() {
  // Se inicializa la interrupción de tiempo
  Timer1.initialize(1000000); // set a timer of length 1000000 microseconds 
                             // (or 1 sec - or 1Hz)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
    
  // Se activa la referencia analógica externa
  // ¡¡¡ Conectar en placa AREF con 3.3V !!!
  analogReference(EXTERNAL); 

  // Se define la salida 11 como salida
  pinMode(Pin_PWM_11, OUTPUT);
   // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  if (flag_timer) // Este if se activa cada segundo por la función timerIsr
  {
    tiempo=tiempo + 1; // Se genera el valor del tiempo en segundos

    // Se guardan los valores anteriores del error y de la parte integral
    error1=error;
    yI1=yI;
    
    // Se lee el valor de las entradas analógicas como valores enteros.
    valor_tra = analogRead(Pin_Tra_A1);
    valor_ref = analogRead(Pin_Ref_A0);

    
    // Se escalan las entradas 0-1023 ud (entero)= 0-100 grados centrigrados (float) 
    temperatura=(float)valor_tra*100/1023; // El comando (float) pasa el entero valor_tra a float
    referencia=(float)valor_ref*100/1023;

/////////////// Controles propuestos /////////////////
// Control 1
    // Control manual utilizando la entrada A0 (Potenciometro).
    /*
    u_int=map(valor_ref,0,1203,0,255); // La salida PWM sólo acepta valores entre 0 y 255
    analogWrite(Pin_PWM_11, u_int);
    */
    
// // Control 2
//    // Control todo nada sin histéresis
//    if (temperatura < referencia)
//    {
//      u_int=255; // salida al 100%
//      analogWrite(Pin_PWM_11, u_int);
//    }
//    else
//    {
//      u_int=0; // salida al 0%
//      analogWrite(Pin_PWM_11, u_int);
//    }


// // Control 3
//    // Control todo nada con histéresis
//    if (temperatura < (referencia-h))
//    {
//      u_int=255; // salida al 100%
//      analogWrite(Pin_PWM_11, u_int);
//    }
//    else if (temperatura > (referencia+h))
//    {
//      u_int=0; // salida al 0%
//      analogWrite(Pin_PWM_11, u_int);
//    }


// // Control 4
// Controlador P, PI, PD y PID

    // Se calcula el error
    error=referencia-temperatura;
    
    // Control proporcional 
    yp=k*error; // Se calcula la acción proporcional

    // Control derivativo
    yd=k*Td/T*(error-error1);

    // Acción integral
    yI=yI1+k/Ti*T/2*(error+error1);
    // La acción integral se limita para que no haya saturación (antiwindup)
    if (yI>255) {yI=255;}
    else if (yI<0) {yI=0;}
    

    // // Control P
    // u_float=yp;

    // // Control PD
    // u_float=yp+yd;

    // // Control PI
    // u_float=yp+yI;

    // PID
    u_float=yp+yd+yI;

    // Se envía a la salida PWM
    u_int=(int)u_float; // se pasa a un valor entero
    u_int=constrain(u_int,0,255); // se limita a un valor entre 0 y 255
    analogWrite(Pin_PWM_11, u_int); // se envía a la salida PWM


    //Serial.print("tiempo = "); // En caso de que se quiera mostrar por el puerto serie
    Serial.print(tiempo); //tiempo en décimas de segundo
    Serial.print("\t ");    // Tabulación
    //Serial.print("\t referencia = ");
    Serial.print(referencia);
    Serial.print("\t ");     
    //Serial.print("\t temperatura = ");
    Serial.print(temperatura);
    Serial.print("\t ");     
    //Serial.print("\t salida = ");
    Serial.print(u_int);
    Serial.print("\n"); // Nueva línea
  
    flag_timer=false;
  }
}


/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
// Esta función se activa cada vez que pasa el tiempo definido en la interrupción. 
// En nuestro caso cada segundo
void timerIsr()
{
    flag_timer=true;
 } 
