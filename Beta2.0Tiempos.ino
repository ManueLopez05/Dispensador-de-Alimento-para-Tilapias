#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

//Creamos los objetos RTC, LCD Y Servo
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myServo;

//Asignamos los pines
const int servoPin = 9;
const int joystickPin = A1;
int angApertura = 0;
int angCierre = 160;

//Definimos los valores en los que se puede navegar con el joystick
#define Umax 1023
#define Umin 900
#define Dmax 514
#define Dmin 490
#define Lmax 342
#define Lmin 330
#define Rmax 356
#define Rmin 240
#define Mmax 205
#define Mmin 180

int menuLevel = 0; // Nivel del menú (0: menú principal, 1: selección de número)
int selectedOption = 0; // Opción seleccionada en el menú
int number = 2;     // Número seleccionado (peso del pez en gramos)


void setup() {
  //Inicialiazamos la comunicación serial, RTC y LCD
  Serial.begin(9600);
  lcd.init();           //Inicializamos la LCD
  lcd.backlight();
  myServo.attach(servoPin);   //Mandamos el servo al pin análogico seleccionado
  myServo.write(angCierre);   //Establecemos el servo a su posición de cerrado

  // Verificar si el RTC está conectado correctamente
  if (!rtc.begin()) {
    lcd.print("No RTC found");
    while (1);
  }

  if (rtc.lostPower()) {
    lcd.print("Ajustando RTC...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(2000);
    lcd.clear();
  }

  lcd.setCursor(1, 0);
  lcd.print("BIENVENIDO");
  lcd.setCursor(1, 1);
  lcd.print("DISPENSADOR");
  lcd.setCursor(1, 2);
  lcd.print("TILAPIA");
  delay(2000);
  lcd.clear();

}

void loop() {
  DateTime now = rtc.now();     //Obtenemos la hora actual

  lcd.setCursor(0, 0);
  lcd.print("MENU");
  lcd.setCursor(6, 0);
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year() % 100);
  lcd.setCursor(15, 0);
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());

  // Leer el valor del joystick
  int value = analogRead(joystickPin);

  //Navegar por el menú principal según el valor del joystick
  if(menuLevel == 0){
    navegarMenuPrincipal(value);
  }
  else if(menuLevel == 1){
    seleccionarNumero(value);
  }

  //Ejecutamos las dosificaciones en el tiempo establecido
  if(now.minute() % 1 == 0 && now.second() == 0){
    ejecutarDosificacionSegunPeso(number);
    delay(2500);   //Esperamos 2.5 segundos para evitar rebotes
  }
}

// ----------------------------- FUNCIONES -------------------------------

// ----------------------- FUNCION DEL MENÚ PRINCIPAL --------------
void navegarMenuPrincipal(int value) {
  // Navegación del menú principal
  if (value < Dmax && value > Dmin) {
    // MOVERSE HACÍA ABAJO //
    lcd.setCursor(18, selectedOption + 1);
    lcd.print(" ");  // Borrar la flecha de la opción anterior
    selectedOption++;
    if (selectedOption > 2) selectedOption = 0;  // Limitar opciones a 3
    delay(300);
  } 
  else if (value < Umax && value > Umin) {
    // MOVERSE HACÍA ARRIBA
    lcd.setCursor(18, selectedOption + 1);
    lcd.print(" ");  // Borrar la flecha de la opción anterior
    selectedOption--;
    if (selectedOption < 0) selectedOption = 2;  // Limitar opciones a 3
    delay(300);

  } else if (value < Mmax && value > Mmin) {
    // Seleccionar opción
    if (selectedOption == 0) {
      ejecutarDosificacion();
    } else if (selectedOption == 1) {
      lcd.clear();
      menuLevel = 1;      
    } else if (selectedOption == 2) {
      ejecutarDosificacionAutomatica();
    }
    delay(300);
  }

  // Mostrar opciones del menú principal
  lcd.setCursor(0, 1);
  lcd.print("1. Iniciar");
  lcd.setCursor(0, 2);
  lcd.print("2. Peso del pez");
  lcd.setCursor(0, 3);
  lcd.print("3. Dosificar");
  lcd.setCursor(18, selectedOption + 1);
  lcd.print(">");  // Mostrar indicador de selección
}

// --------------------- SELECCIONAMOS EL PESO DEL PEZ ----------------
void seleccionarNumero(int value) {
  // Seleccionar el número
  if (value < Dmax && value > Dmin) {
    // MOVERSE HACÍA ABAJO
    if (number > 2) {
      number--;
    } else {
      number = 50;  // Regresar a 50 si baja de 2
    }
    delay(300);

  } else if (value < Umax && value > Umin) {
    // MOVERSE HACÍA ARRIBA
    if (number < 50) {
      number++;
    } else {
      number = 2;  // Regresar a 2 si sube de 50
    }
    delay(300);

  } else if (value < Lmax && value > Lmin) {
    // Regresar al menú principal
    lcd.clear();
    menuLevel = 0;
    delay(300);

  } else if (value < Mmax && value > Mmin) {
    // Confirmar selección
    lcd.clear();
    menuLevel = 0;  // Regresar al menú principal
    delay(300);
  }

  // Mostrar el número seleccionado
  lcd.setCursor(0, 1);
  lcd.print("Peso en gramos pez:");
  lcd.setCursor(10, 2);
  lcd.print(number);
}

// -------------- CREAMOS LAS FUNCIONES PARA LAS DOSIFICACIONES --------
void ejecutarDosificacion1gr() {
  myServo.write(angApertura);
  delay(220); // Tiempo para dosificar 1 gramo de alimento.
  myServo.write(angCierre);
  delay(500);
}

void ejecutarDosificacion2gr(){
  myServo.write(angApertura);
  delay(250); // Tiempo para dosificar 2 gramos de alimento.
  myServo.write(angCierre);
  delay(500);
}

void ejecutarDosificacion3gr() {
  myServo.write(angApertura);
  delay(280); // Tiempo para dosificar 2 gramos de alimento.
  myServo.write(angCierre);
  delay(500);
}

void ejecutarDosificacion4gr() {
  ejecutarDosificacion2gr();
  delay(500);
  ejecutarDosificacion2gr();
  delay(500); 
}

void ejecutarDosificacion5gr(){
  myServo.write(angApertura);
  delay(300); // Tiempo para dosificar 2 gramos de alimento.
  myServo.write(angCierre);
  delay(500);
}

void ejecutarDosificacion6gr(){
  ejecutarDosificacion5gr();
  delay(500);
  ejecutarDosificacion1gr();
  delay(500);
}

void ejecutarDosificacion7gr(){
  myServo.write(angApertura);
  delay(320); 
  myServo.write(angCierre);
  delay(500); 
}

void ejecutarDosificacion8gr(){
  ejecutarDosificacion5gr();
  delay(500);
  ejecutarDosificacion3gr();
  delay(500);
}

void ejecutarDosificacion9gr(){
  myServo.write(angApertura);
  delay(340); 
  myServo.write(angCierre);
  delay(500);
}

void ejecutarDosificacion10gr(){
  ejecutarDosificacion9gr();
  delay(500);
  ejecutarDosificacion1gr();
  delay(500);
}

void ejecutarDosificacion19gr(){
  myServo.write(angApertura);
  delay(400);
  myServo.write(angCierre);
}

void ejecutarDosificacion39gr(){
  myServo.write(angApertura);
  delay(500);
  myServo.write(angCierre);
}

void ejecutarDosificacion57gr(){
  myServo.write(angApertura);
  delay(600);
  myServo.write(angCierre);
}

// --------- FUNCION PARA IMPLEMENTAR LAS DOSIFICACIONES SEGUN EL PESO --------
void ejecutarDosificacionSegunPeso(int peso) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Dosificando...");

  switch (peso) {
    case 2:
      ejecutarDosificacion2gr();
      break;
    case 3:
      ejecutarDosificacion4gr();
      break;
    case 4:
      ejecutarDosificacion5gr();
      break;
    case 5:
      ejecutarDosificacion6gr();
      break;
    case 6:
      ejecutarDosificacion7gr();
      break;
    case 7:
      ejecutarDosificacion8gr();
      break;
    case 8:
      ejecutarDosificacion10gr();
      break;
    case 9:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion1gr();
      break;
    case 10:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion2gr();
      break;
    case 11:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;      
    case 12:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion4gr();
      break;
    case 13:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion6gr();
      break;
    case 14:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion7gr();
      break;
    case 15:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion8gr();
      break;
    case 16:
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion9gr();
      break;
    case 17:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion1gr();
      delay(500);
      break;
    case 18:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;
    case 19:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion4gr();
      break;
    case 20:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion5gr();
      break;
    case 21:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion6gr();
      break;
    case 22:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion7gr();
      break;
    case 23:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion9gr();
      break;
    case 24:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      break;
    case 25:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion1gr();
      break;
    case 26:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion2gr();
      break;
    case 27:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;     
    case 28:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion5gr();
      break;
    case 29:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion6gr();
      break;
    case 30:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion7gr();
      break;
    case 31:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion8gr();
      break;
    case 32:
      ejecutarDosificacion19gr();
      delay(500);
      ejecutarDosificacion19gr();
    case 33:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion1gr();
      break;
    case 34:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion2gr();
      break;
    case 35:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;
    case 36:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion4gr();
      break;
    case 37:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion5gr();
      break;
    case 38:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion7gr();
      break;
    case 39:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion8gr();
      break;
    case 40:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion9gr();
      break;
    case 41:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      break;     
    case 42:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion1gr();
      break;
    case 43:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;
    case 44:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion4gr();
      break;
    case 45:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion5gr();
      break;
    case 46:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion6gr();
      break;
    case 47:
      ejecutarDosificacion39gr();
      delay(500);
      ejecutarDosificacion10gr();
      delay(500);
      ejecutarDosificacion7gr();
      break;
    case 48:
      ejecutarDosificacion57gr();
      delay(500);
      ejecutarDosificacion1gr();
      break;
    case 49:
      ejecutarDosificacion57gr();
      delay(500);
      ejecutarDosificacion2gr();
      break;
    case 50:
      ejecutarDosificacion57gr();
      delay(500);
      ejecutarDosificacion3gr();
      break;
    default:
      myServo.write(angApertura);
      delay(100);  // Caso por defecto
      myServo.write(angCierre);
      break;
  }
}

// ------------ CREAMOS UNA FUNCION PARA DOSIFICAR LA PRIMERA OPCION -----------
void ejecutarDosificacion() {
  // Para ejecutar la inicialización de la dosificación
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Dosificando...");

  delay(1000);
  myServo.write(angApertura);
  delay(6000);
  myServo.write(angCierre);
  lcd.clear();
}

// ---- CREAMOS UNA FUNCION PARA IR DOSIFICANDO DE MANERA AUTOMATICA ----------
void ejecutarDosificacionAutomatica() {
  // Ejecutar la dosificación automáticamente en los intervalos definidos
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Dosificando...");
  ejecutarDosificacionSegunPeso(number);
  lcd.clear();
}