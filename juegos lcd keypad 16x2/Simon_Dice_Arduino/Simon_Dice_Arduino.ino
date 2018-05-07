/*
 * https://www.minitronica.com/juego-simon-dice-con-arduino/ 
 * Visitanos en www.minitronica.com
*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 
 
// Declaramos los pins de salida para los LED
/*
byte led_rojo = 1;         
byte led_azul = 4;
byte led_amarillo = 3;
byte led_verde = 2;
*/
// Declaramos el pin para el zumbador piezoelectrico
const byte zumbador = 12;  
 /* 
// Declaramos los pins de salida para los botones
byte boton_rojo = 1;      
byte boton_azul = 1;
byte boton_amarillo = 1; 
byte boton_verde = 1; 
*/

// Declaramos alguna variables
long sequence[20];             // Array que alberga la secuencia
int contador = 0;              // Contador
long input = 5;                // Indicador de boton pulsado
int wait = 500;                // Retraso segun la secuencia se incrementa
int puntuacion_maxima = 20;    // Puntuación máxima donde acaba el juego 
 
// Declaramos algunas variables para los efectos musicales
int length = 15;                  // Numero de notas de la melodia
char notes[] = "ccggaagffeeddc "; // Notas de la melodia (cada letra es una nota distinta)
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };  // Duracion de cada tono en un array
byte tempo = 100;  //Tempo de la melodia
 
   
void setup() {
  lcd.begin(16, 2); 
  pinMode(zumbador, OUTPUT);
  /*
 // Configuramos los pines de los leds y del zumbador como salidas
  pinMode(led_rojo, OUTPUT);      
  pinMode(led_verde, OUTPUT); 
  pinMode(led_amarillo, OUTPUT); 
  pinMode(led_azul, OUTPUT); 
  pinMode(zumbador, OUTPUT);
    
 // Configuramos los pines de los botones como entradas
  pinMode(boton_rojo, INPUT);    
  pinMode(boton_verde, INPUT);
  pinMode(boton_amarillo, INPUT);
  pinMode(boton_azul, INPUT);
    
 // Iniciamos las resistencias PullUp internas para los botones
  digitalWrite(boton_rojo, HIGH);
  digitalWrite(boton_verde, HIGH);
  digitalWrite(boton_amarillo, HIGH);
  digitalWrite(boton_azul, HIGH);
    */
  // Hacemos que suene la melodia al arrancar el Arduino    
   lcd.setCursor(0, 0);  lcd.print("     Simon"); 
   lcd.setCursor(0, 1);  lcd.print(" Dice lcd 16x2"); 
   delay(500);  lcd.clear();

   lcd.setCursor(0, 0);  lcd.print("     HECHO"); 
   lcd.setCursor(0, 1);  lcd.print("      Por"); 
   delay(500);  lcd.clear();

   lcd.setCursor(0, 0);  lcd.print("     Walter"); 
   lcd.setCursor(0, 1);  lcd.print("    Mauricio"); 
   delay(500);  lcd.clear();

  felicitacion();
  }
  
  
void loop() {
  mostrar_secuencia();  // Reproduce la sequencia
  leer_secuencia();     // Lee la sequencia
  delay(1000);          // Espera 1 segundo
}
  
  
 // Funcion para definir las notas y sus frecuencias
void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
 
  for(int i = 0; i < 8; i++){
     if(names[i] == note){ playtone(tones[i], duration); }
     }
}
 
 // Funcion para definir las notas segun la duración y el tono
void playtone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone *2) {
      digitalWrite(zumbador, HIGH);
      delayMicroseconds(tone);
      digitalWrite(zumbador, LOW);
      delayMicroseconds(tone);
    }
  }     
   
 // Funciones para encender los leds y reproducir el tono correspondiente
void flash_rojo() {
  //lcd.setCursor(3, 0); lcd.print(led_rojo);
  //lcd.setCursor(3, 1);  lcd.print("1");
  playtone(2273,wait);         
  lcd.clear();
}
  
void flash_azul() {
  //lcd.setCursor(12, 0); lcd.print(led_azul);
  //lcd.setCursor(12, 1);  lcd.print("4");
  playtone(1700,wait);            
  lcd.clear();
}
  
void flash_amarillo() {
  //lcd.setCursor(9, 0); lcd.print(led_amarillo);
  //lcd.setCursor(9, 1);  lcd.print("3");
  playtone(1275,wait);             
  lcd.clear();
} 
  
void flash_verde() {
  //lcd.setCursor(6, 0); lcd.print(led_verde);
  //lcd.setCursor(6, 1);  lcd.print("2");
  playtone(1136,wait);             
  lcd.clear();
} 
   
// Funcion para mostrar que botón se tenia que pulsar en caso de error del jugador
void mostrar_boton_correcto(long led) {
    switch(led) {
        case 0:
          lcd.setCursor(3, 1);  lcd.print("1"); //delay(100);
          flash_rojo();
          break;
        case 1:
          lcd.setCursor(6, 1);  lcd.print("2"); //delay(100);
          flash_verde();
          break;
        case 2:
          lcd.setCursor(9, 1);  lcd.print("3"); //delay(100);
          flash_amarillo();
          break;
        case 3:
          lcd.setCursor(12, 1);  lcd.print("4");
          flash_azul();
           //delay(100);
          break;
      }
      delay(50);
} 
  
// Función que reproduce la canción al arrancar el arduino y para el juego cuando se llega a la puntuacion maxima
void felicitacion() {
  lcd.setCursor(0, 0); lcd.print(" OK SIGUIENTE");
  delay(300); lcd.clear(); 
  lcd.setCursor(0, 0); lcd.print("   1  2  3  4");

  for(int i = 0; i < length; i++){
   if(notes[i] == ' '){ delay(beats[i] * tempo); } // rest
   else { playNote(notes[i], beats[i] * tempo); }
   
   delay(tempo / 2); 
  }
   
  delay(1000);   
  lcd.clear();
  resetcontador();                    
  }
    
 // Reseteamos contadores
void resetcontador() {
  contador = 0;
  wait = 500;
}
   
  
// Funcion para crear y reproducir los patrones
void mostrar_secuencia() { // Con esto hacemos que la función Random sea aun mas aleatoria
  randomSeed(analogRead(10));  
  sequence[contador] = random(4);       
  
  for(int i = 0; i < contador; i++){ mostrar_boton_correcto(sequence[i]); }
    
  wait = 500 - (contador * 15);        
  contador++;                          
  }
  
 
// Funcion para leer los botones que pulsa el jugador
void leer_secuencia() {
   for(int i=1; i < contador; i++){              
      while(input==5){ botons(); }
    
  if(sequence[i-1] == input) {              
        mostrar_boton_correcto(input);                           
        if(i == puntuacion_maxima){ 
          lcd.setCursor(0, 0); lcd.print("  Felicidades!");
          lcd.setCursor(0, 1); lcd.print("    YOU WIN!!");
          delay(500);
          felicitacion(); }
      }
  else {
    lcd.setCursor(3, 0); lcd.print("PERDISTE");
    lcd.setCursor(3, 1); lcd.print("NI MODO!!");
    playtone(4545,1500);                  
    delay(500);
    lcd.clear();
    mostrar_boton_correcto(sequence[i-1]);                 
    mostrar_boton_correcto(sequence[i-1]);                 
    mostrar_boton_correcto(sequence[i-1]);
    delay(1000);
    felicitacion();
    resetcontador();                          
    } 
    input = 5;                                   
    }
}  
/*
void botons() {
     int xy;        // Declarar variables
     xy = analogRead(0);     // Asignar 'xy' para las entradas analógicas Arduino Shields (botones)

    if(xy < 10){ boton_azul = 0; }      // if el button es RIGHT
    else if(xy < 110){ boton_amarillo = 0; }  // if el button es UP
    else if(xy < 265){ boton_amarillo = 0; }  // if el button es DAWN
    else if(xy < 420){ boton_verde = 0; }  // if el button es LEFT //
    else if(xy < 650){ boton_rojo = 0; }  // if el button es SELECT //
    else {
      boton_rojo = 1;      
      boton_azul = 1;
      boton_amarillo = 1; 
      boton_verde = 1;  
      }        
}
*/
void botons() {
     int xy;        // Declarar variables
     xy = analogRead(0);     // Asignar 'xy' para las entradas analógicas Arduino Shields (botones)

    if(xy < 10){ input = 3; }      // if el button es RIGHT
    else if(xy < 110){ input = 2; }  // if el button es UP
    else if(xy < 265){ input = 2; }  // if el button es DAWN
    else if(xy < 420){ input = 1; }  // if el button es LEFT //
    else if(xy < 650){ input = 0; }  // if el button es SELECT //
    else {  }        
}
