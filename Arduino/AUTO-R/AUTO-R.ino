/*  AUTO-R
 *  Controlador de semaforos de dos fases vehiculares, implementado
 *  sin el uso de la funcion delay(), de manera que no se detenga la ejecucion del 
 *  controlador, permitiendo ejecutar otras tareas de manera simulatanea.
 *  El controlador corre un modelo de maquina de estados, con 6 estados en total.
 */
 
#include <LiquidCrystal.h>
#include <Time.h>

// Definicion de los pines de salida del Arduino
const int V1 = 6;   // Verde Fase 1
const int Am1 = 7;  // Amarillo Fase 1
const int R1 = 8;   // Rojo Fase 1
const int V2 = 9;   // Verde Fase 2
const int Am2 = 10; // Amarillo Fase 2
const int R2 = 13;  // Rojo Fase 2

// Ciclo (en milisegundos)
unsigned long ciclo = 120000;

// Repartos (en porcentaje)
int fase1 = 60;
int fase2 = 40;

// Tiempos de senales (en milisegundos)
unsigned long tV1;  // tiempo de verde fase 1
unsigned long tA1;  // tiempo de amarillo fase 1
unsigned long tDR1; // tiempo de despeje de rojo fase 1
unsigned long tV2;  // tiempo de verde fase 2
unsigned long tA2;  // tiempo de amarillo fase 2
unsigned long tDR2; // tiempo de despeje de rojo fase 2

// Instante en el que inicia un nuevo ciclo, medido en ms desde el inicio de ejecucion del programa
unsigned long inicioCiclo;

// Representa el estado actual de la maquina de estados 
int estado;

// Trama recibida desde software de configuracion
String trama;

// Variables para el conteo regresivo
unsigned long contadorF1;
unsigned long contadorF2;

// Inicializacion del objto LCD con los pines conectados al display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Configurar los pines de las luminarias como salidas
  pinMode(R1, OUTPUT);
  pinMode(Am1, OUTPUT);
  pinMode(V1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(Am2, OUTPUT);
  pinMode(V2, OUTPUT); 

  // Inicializar el contador de estado en 1 (Verde Fase 1)
  estado = 1;

  // Configurar el tamano del display LCD, columnas y filas
  lcd.begin(16, 2);

  // Inicializar puerto serial
  Serial.begin(9600);

  // Preparar el display LCD:
  lcd.setCursor(0, 1);
  lcd.print("F1:");
  lcd.setCursor(8, 1);
  lcd.print("F2:");

  // Sincronizar reloj
  unsigned long t; // elemento time_t, recibe el tiempo en formato UNIX TimeStamp
  bool banderaSerial = false; // sera true una vez recibida la trama en pto serial y formato sea correcto.
  do {
    if (Serial.available()) {
      trama = Serial.readString();
      if (trama.length() == 23 && trama.charAt(0) == 'H' && trama.charAt(11) == 'T')
          banderaSerial = true;
    }
  } while (!banderaSerial);

  t = (unsigned long)trama.substring(1, 11).toInt();
  setTime(t);

  // Establecer tiempos del semaforo
  ciclo = (unsigned long)trama.substring(12, 15).toInt();
  ciclo = ciclo * 1000;

  fase1 =trama.substring(15, 17).toInt();
  fase2 =trama.substring(17, 19).toInt();

  tA1 = (unsigned long)trama.substring(19, 20).toInt();
  tA1 = tA1 * 1000;
  tDR1 = (unsigned long)trama.substring(20, 21).toInt();
  tDR1 = tDR1 * 1000;
  tA2 = (unsigned long)trama.substring(21, 22).toInt();
  tA2 = tA2 * 1000;
  tDR2 = (unsigned long)trama.substring(22, 23).toInt();
  tDR2 = tDR2 * 1000;
  
  inicioCiclo = millis();

  // Calculo de tiempos de las senales
  tV1 = ((ciclo * fase1 / 100) - tA1 - tDR1); // tiempo verde F1 en milisegundos
  tV2 = ((ciclo * fase2 / 100) - tA2 - tDR2); // tiempo verde F2 en milisegundos 
}

void loop() {
  //Imprimir la hora en el display LCD
  // Si hora es menos que 10, anteponer un cero.
  if (hour() < 10) {
    lcd.setCursor(0, 0);
    lcd.print('0');
    lcd.setCursor(1, 0);
  }
  else
    lcd.setCursor(0, 0);
   lcd.print(hour()); 
   
  // Imprimir los minutos en el display LCD
  lcd.setCursor(2, 0);
  lcd.write(':');
  // Si minutos es menor que 10, anteponer un cero
  if (minute() < 10) {
    lcd.setCursor(3, 0);
    lcd.print('0');
    lcd.setCursor(4, 0);
  }
  else
    lcd.setCursor(3, 0);
  lcd.print(minute());
  
  // Ejecutar el ciclo una y otra vez

  //Selector de estados
  switch (estado) {
    // Verde Fase 1
    case 1:
      digitalWrite(R1, LOW);
      digitalWrite(R2, HIGH);
      digitalWrite(V1, HIGH);
      // calcular contador F1
      contadorF1 = (tV1 + inicioCiclo - millis()) / 1000;
      contadorF2 = contadorF1 + ((tA1 + tDR1) / 1000);
      imprimirContador(0, contadorF1, 2, contadorF2);
      if (millis() - inicioCiclo >= tV1)
        //Si ya se cumplio el tiempo de verde 1:
        estado = 2;
      break;

    // Amarillo Fase 1
    case 2:
      digitalWrite(V1, LOW);
      digitalWrite(Am1, HIGH);
      contadorF1 = (tV1 + tA1 + inicioCiclo - millis()) / 1000;
      contadorF2 = contadorF1 + (tDR1 / 1000);
      imprimirContador(1, contadorF1, 2, contadorF2);
      if ((millis() - inicioCiclo) >= tV1 + tA1)
        estado = 3;
      break;

    // Despeje Fase 1
    case 3:
      digitalWrite(Am1, LOW);
      digitalWrite(R1, HIGH);
      contadorF2 = (tV1 + tA1 + tDR1 + inicioCiclo - millis()) / 1000;
      contadorF1 = contadorF2 + ((tV2 + tA2 + tDR2) / 1000);
      imprimirContador(2, contadorF1, 2, contadorF2);
      if (millis() - inicioCiclo >= tV1 + tA1 + tDR1)
        estado = 4;
      break;
    
    // Verde Fase 2
    case 4:
      digitalWrite(R2, LOW);
      digitalWrite(V2, HIGH);
      contadorF2 = (tV1 + tA1 + tDR1 + tV2 + inicioCiclo - millis()) / 1000;
      contadorF1 = contadorF2 + ((tA2 + tDR2) / 1000);
      imprimirContador(2, contadorF1, 0, contadorF2);
      if (millis() - inicioCiclo >= tV1 + tA1 + tDR1 + tV2)
        estado = 5;
      break;
  
    // Amarillo Fase 2
    case 5:
      digitalWrite(V2, LOW);
      digitalWrite(Am2, HIGH);
      contadorF2 = (tV1 + tA1 + tDR1 + tV2 + tA2 + inicioCiclo - millis()) / 1000;
      contadorF1 = contadorF2 + (tDR2 / 1000);
      imprimirContador(2, contadorF1, 1, contadorF2);
      if (millis() - inicioCiclo >= tV1 + tA1 + tDR1 + tV2 + tA2)
        estado = 6;
      break;
  
    // Despeje Fase 2
    case 6:
      digitalWrite(Am2, LOW);
      digitalWrite(R2, HIGH);
      contadorF1 = (tV1 + tA1 + tDR1 + tV2 + tA2 + tDR2 + inicioCiclo - millis()) / 1000;
      contadorF2 = contadorF1 + ((tV1 + tA1 + tDR1) / 1000);
      imprimirContador(2, contadorF1, 2, contadorF2);
      if (millis() - inicioCiclo >= tV1 + tA1 + tDR1 + tV2 + tA2 + tDR2) {
        estado = 1;
        inicioCiclo = millis();
      }
      break;
  }
  delay(50);
}

void imprimirContador(int luminaria1, unsigned long conteoF1, int luminaria2, unsigned long conteoF2) {
  // Luminarias: Verde = 0, Amarillo = 1, Rojo = 2
  // Contador F1
  ++conteoF1;
  ++conteoF2;
  lcd.setCursor(3, 1);
  if (luminaria1 == 0)
    lcd.write('V');
  else if (luminaria1 == 1)
    lcd.write('A');
  else
    lcd.write('R');
  lcd.setCursor(4, 1);
  if (conteoF1 <= 260) {
    if (conteoF1 < 10)
      lcd.print('0');
    lcd.print(conteoF1);
  }

  // Contador F2
  lcd.setCursor(11, 1);
    if (luminaria2 == 0)
    lcd.write('V');
  else if (luminaria2 == 1)
    lcd.write('A');
  else
    lcd.write('R');
  if (conteoF2 <= 260) {
    if (conteoF2 < 10)
      lcd.print('0');
    lcd.print(conteoF2);
  }
}

