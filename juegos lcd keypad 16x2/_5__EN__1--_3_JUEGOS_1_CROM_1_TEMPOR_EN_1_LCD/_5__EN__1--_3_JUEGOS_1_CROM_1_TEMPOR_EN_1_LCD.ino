/*
 * Walter Mauricio 
 * Abril 2018
 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7); // these are the pins used on the shield for this sketch  // Configura pines de salida para el LCD
//__________________________________________________________________________________________________
//__________________________________________________________________________________________________
int conta = 0;
int alarma = 13;           //Alarma
int ahoras = 0;            //Variable a mostrar por LCD de las horas
int aminutos = 0;          //Variable a mostrar por LCD de los minutos
int asegundos = 0;         //Variable a mostrar por LCD de los segundos
int segundostotal = 0;     //Tiempo total
int msg = 0;                //Barrera para el mensaje de bienvenida
//______________________________________________________
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons(){
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.0 comment the other threshold and use the one below:
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   

 return btnNONE;  // when all others fail, return this...
}
//_________________________________________________________________________________________________
byte ok = 0;
unsigned long interval = 100;  // the time we need to wait
unsigned long previousMillis = 0; // millis() returns an unsigned long.
//___________________________________________________________________________________________________  
unsigned long start, finished, elapsed; // variables used on more than 1 function need to be declared here
//____________________________________________________________________________________________________
// defino los nuevo caracter o imagenes
byte letra1[8] = { B01110, B01110, B01110, B00100, B11111, B00100, B01010, B01010 }; // fugitivo
byte letra2[8] = { B01110, B01110, B01110, B10101, B01110, B00100, B01010, B10001 }; 
byte letra3[8] = { B00100, B00100, B00100, B00100, B00100, B11111, B01110, B00100 }; //flecha
byte letra0[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000 }; // blanco
byte letra4[8] = { B01110, B01110, B01110, B11111, B10101, B01110, B01010, B11011 }; // atrapado

byte x = 0;
byte y = 1;
byte retardo = 200;
byte presos = 0;
byte libres = 7;
byte fallas = 0;
byte puntaje = 0;
byte fail = 3;
//______________________________________________________________________________________________________
// Steering wheel potentiometer
const int POTPIN = 0;
const int MAXPOT = 800; // no need to turn the wheel all the way to 1023 :)
const int RANDSEEDPIN = 0; // an analog pin that isn't connected to anything
const int MAXSTEPDURATION = 300; // Start slowly, each step is 1 millisec shorter.
const int MINSTEPDURATION = 150; // This is as fast as it gets

const int NGLYPHS = 6;
// the glyphs will be defined starting from 1 (not 0),
// to enable lcd.print() of null-terminated strings
byte glyphs[NGLYPHS][8] = {
  { B00000, B11111, B01110, B10101, B00000, B00000, B00000, B00000 }    // 1: car up
  ,{ B00000, B00000, B00000, B00000, B10101, B01110, B11111, B00000 }   // 2: car down
  ,{ B00000, B10101, B01110, B11111, B00000, B00000, B00000, B00000 }   // 3: truck up
  ,{ B00000, B00000, B00000, B00000, B01110, B11111, B01010, B00000 }   // 4: truck down
  ,{ B10101, B01110, B01110, B10101, B00000, B00000, B00000, B00000 }   // 5: crash up
  ,{ B00000, B00000, B00000, B10101, B01110, B01110, B10101, B00000 }   // 6: crash down
};

const int NCARPOSITIONS = 4;
int pasabot = 0;

// Each position is mapped to a column of 2 glyphs
// Used to make sense when I had a 5th position
// where car or crash was drawn as 2 glyphs
// (can't do that since 0 terminates strings),
// so it's kinda silly now, but it ain't broke :)
const char BLANK = 32;
char car2glyphs[NCARPOSITIONS][2] = {
  {1,BLANK},{2,BLANK},{BLANK,1},{BLANK,2}
};
char truck2glyphs[NCARPOSITIONS][2] = {
  {3,BLANK},{4,BLANK},{BLANK,3},{BLANK,4}
};
char crash2glyphs[NCARPOSITIONS][2] = {
  {5,BLANK},{6,BLANK},{BLANK,5},{BLANK,6}
};

const int ROADLEN = 15; // LCD width (not counting our car)
int road[ROADLEN]; // positions of other cars
char line_buff[2+ROADLEN]; // aux string for drawRoad()
int road_index;
int car_pos;
// Off-the-grid position means empty column, so MAXROADPOS
// determines the probability of a car in a column
// e.g. 3*NCARPOSITIONS gives p=1/3
const int MAXROADPOS = 3*NCARPOSITIONS;
int step_duration;

int crash; // true if crashed
unsigned int crashtime; // millis() when crashed
const int CRASHSOUNDDURATION = 250;

const char *INTRO1="    SPACE";
const char *INTRO2="  Drive MITY";
const int INTRODELAY = 2000;

int puntos = 0; 
//_______________________________________JUEGO DE SALTOS________________________________________________________
char BLOQUE;
char PERSONAJE;
int LINEA = 0;
int LINEA1 = 0;
int SCORE = -1;
int num = 110;
int tiemp = num;

byte saltoo = 0;
//_______________________________________________________________________________________________________________


void setup() {
  lcd.begin(16, 2);               // Configura el LCD con 16 columnas y 2 filas
  lcd.createChar(1, letra1);      // asigna los nuevos caracteres por numeros
  lcd.createChar(2, letra2);
  lcd.createChar(3, letra3);
  lcd.createChar(0, letra0);
  lcd.createChar(4, letra4);
} 

void loop() { // Elige juegos

 if((unsigned long)(millis() - previousMillis) >= interval){
  previousMillis = millis();
  ok = ok + 1;
 }
  if(ok == 0){
   lcd.setCursor(0, 0);  lcd.print("1.SELECT= Space."); 
   lcd.setCursor(0, 1);  lcd.print("2.LEFT  = Pres.."); }

  if(ok == 10){ 
   lcd.setCursor(0, 0);  lcd.print("3.DAWN  = Salt.."); 
   lcd.setCursor(0, 1);  lcd.print("4.RIGHT = Cron.."); }

   if(ok == 19){  lcd.clear(); }
   if(ok == 20){ lcd.setCursor(0, 0);  lcd.print("5.UP = Tempor..");  }
   
   if(ok == 30){ ok = 0; }
   
  int xy; // Declarar variables
  xy = analogRead (0); // Asignar 'X' para las entradas analógicas Arduino Shields (botones)

  if (xy < 10) {  lcd.clear(); Cronometro(); } // if el button es RIGHT
   
  else if(xy < 110){ lcd.clear(); Temporalizador(); }  // if el button es UP

  else if(xy < 265){ // if el button es DAWN
   lcd.clear(); saltos(); }
  
  else if(xy < 420){ // if el button es LEFT
   lcd.clear(); Presos(); }
  
  else if(xy < 650){ // if el button es SELECT
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("     Space");
    delay(2000); // para la supresión de rebotes
    lcd.clear(); Space();}
}
//_________________________________________________________________________
void Presos() {  //while(1) {
   lcd.setCursor(0, 0); lcd.print(" Juego de ");
   lcd.setCursor(0, 1); lcd.print("         Presos");  
   delay(250); lcd.clear(); delay(250);
   int xP;   xP = analogRead(0);
      if(xP < 1000){ Presoss(); }
      Presos(); //}
   }
   
void Presoss() { delay(250); //while(1) {
  lcd.setCursor(0, 0);   // Posicion del letrero en 0,0
  lcd.print(presos); 
  lcd.setCursor(2, 0);   // Posicion del letrero en 2,0
  lcd.print("F"); 
  lcd.setCursor(3, 0);   // Posicion del letrero en 3,0
  lcd.print(fallas); 
  lcd.setCursor(0, 1);   // Posicion del letrero en 0,1
  lcd.print(libres); 

  lcd.setCursor(x,y);  // imagen fugitivo 1
  lcd.write(byte(1)); 
  delay(retardo);

  lcd.setCursor(x,y);  // imagen fugitivo 2
  lcd.write(byte(2));
  delay(retardo);

  lcd.clear();        // borrar Pantalla

  lcd.setCursor(14,0); // Flecha en posicion 
  lcd.write(byte(3));
  
  lcd.setCursor(5, 0);   // Posicion del letrero
  lcd.print("Atrapelo"); 

  int xx; // Declarar variables
  xx = analogRead(0);
  
  if(xx < 1000){ // if (analogRead(A0)<= 10) 
    // Si es oprimido el interruptor A5
    lcd.setCursor(14,0);       // Se borra la flecha
    lcd.write(byte(0));
    lcd.setCursor(14,1);      // Baja la flecha
    lcd.write(byte(3));
   
    if(x == 14){            // Si coincide en la posicion 14 y es capturado
       lcd.setCursor(0, 0);
       lcd.print(" Atrapado  S."); // $
       lcd.setCursor(x,y);    // Coloca imagen de capturado
       lcd.write(byte(4));
       presos++;   
       libres--;  
    }
    else {                        // Cuando no es capturado
       lcd.setCursor(0, 0);
       lcd.print(" Se escapo N."); 
       lcd.setCursor(x,y);       // donde quedo el corredor, Imagen de que sigue libre 
       lcd.write(byte(2));
       fallas++;
    }
  delay(3000);                     // Espera 3 segundos
  }
  x=x+1;                         // avanza el corredor
  if(x>=16){ x=0; }            // al pasar regresa al inicio  
 
  retardo = random(1,150);       // Escoge un retardo aleatorio

 if(fallas == fail){  // Si coincide en la posicion 14 y es capturado
  lcd.clear();                 // borrar Pantalla
  lcd.setCursor(0, 0); lcd.print("Perdio el Juego");
  lcd.setCursor(0, 1); lcd.print("    Lastima ");  
  delay(3000); lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("NO PUDO ..."); 
  puntaje = presos - fallas;
  lcd.setCursor(3, 1);
  lcd.print("Puntaje:"); 
  lcd.setCursor(12, 1);
  lcd.print(puntaje); 
  delay (3000);
  presos = 0;
  libres = 7;
  fallas = 0;
  lcd.clear();        // borrar Pantalla
  Presos();
  }
  
 if(libres == 0){  // Si coincide en la posicion 14 y es capturado
  lcd.clear();             // borrar Pantalla
  lcd.setCursor(3, 0);

  for(int qq=0; qq<10; qq++){
    lcd.setCursor(0, 0); lcd.print(" Gano el Juego");
    lcd.setCursor(0, 1); lcd.print("    You Win ");  
    delay(300); lcd.clear(); delay(300); }
   
  if(fallas == 0){ lcd.print("EXCELENTE!!!!"); }  // Si no hubo fallas 
  else{ lcd.print("PUEDE MEJORAR"); }   // si termino pero con alguna falla
       
  puntaje=presos-fallas;
  lcd.setCursor(3, 1);
  lcd.print("Puntaje:"); 
  lcd.setCursor(12, 1);
  lcd.print(puntaje);               // Puntaje final 
  delay (7000);
  presos = 0;
  libres = 7;
  fallas = 0;
  lcd.clear();        // borrar Pantalla
  Presos();
  }   
  
  Presoss(); //}
}
//_____________________________________________________________________________________________________________
void Space() {
  crash = crashtime = road_index = 0;
  step_duration = MAXSTEPDURATION;
  line_buff[1+ROADLEN] = '\0'; // null terminate it
  randomSeed(analogRead(RANDSEEDPIN));
  
  for(int i=0; i<NGLYPHS; i++){ lcd.createChar(i+1,glyphs[i]); }
  for(int i=0; i<ROADLEN; i++){ road[i]=-1; }
  
  lcd.begin(16,2);
  //car_pos = map(analogRead(POTPIN),0,1024,0,NCARPOSITIONS);

  int xy;        // Declarar variables
  xy = analogRead(POTPIN);     // Asignar 'xy' para las entradas analógicas Arduino Shields (botones)

  if(xy < 10){ pasabot = 10; }     // if el button es RIGHT   
  else if(xy < 110){ pasabot = 11; }  // if el button es UP
  else if(xy < 265){ pasabot = 26; }  // if el button es DAWN
  else if(xy < 420){ pasabot = 42; }  // if el button es LEFT
  else if(xy < 650){ pasabot = 65; }  // if el button es SELECT
  car_pos = map(pasabot,0,1024,0,NCARPOSITIONS);
  
  for(int i=0; i<2; i++){
    
    if(crash){ line_buff[0]=crash2glyphs[car_pos][i]; } 
    else { line_buff[0]=car2glyphs[car_pos][i]; }
    
    for(int j=0; j<ROADLEN; j++){
      int pos = road[(j+road_index)%ROADLEN];
      line_buff[j+1] = pos>=0 && pos<NCARPOSITIONS ? truck2glyphs[pos][i] : BLANK;
    }
    lcd.setCursor(0,i);
    lcd.print(line_buff);
  }
  lcd.setCursor(1,0);
  lcd.print(INTRO1);
  lcd.setCursor(1,1);
  lcd.print(INTRO2);
  delay(INTRODELAY);
  
while(1){
  //_____________________________________--------------------------____________________________
  unsigned long now = millis()-INTRODELAY;
  if(!crash){
  //car_pos = map(analogRead(POTPIN),0,1024,0,NCARPOSITIONS);
  int xy;        // Declarar variables
  xy = analogRead(POTPIN);     // Asignar 'xy' para las entradas analógicas Arduino Shields (botones)

  if(xy < 10){ pasabot = 1023; }     // if el button es RIGHT   
  else if(xy < 110){ pasabot -= 256; }  // if el button es UP
  else if(xy < 265){ pasabot += 256; }  // if el button es DAWN
  else if(xy < 420){ pasabot += 256; }  // if el button es LEFT
  else if(xy < 650){ pasabot -= 256; }  // if el button es SELECT 
  
  if(pasabot < 0) pasabot = 0;
  if(pasabot > 1023) pasabot = 1023;
  car_pos = map(pasabot,0,1024,0,NCARPOSITIONS);
    
  crash = (car_pos==road[road_index]);
  }
  if(crash){
    if(!crashtime){ crashtime=now;
    for(int i=0; i<2; i++){
      if(crash){ line_buff[0]=crash2glyphs[car_pos][i]; }
      else { line_buff[0]=car2glyphs[car_pos][i]; }
      
    for(int j=0; j<ROADLEN; j++){  // puntos = puntos + 1;
      int pos = road[(j+road_index)%ROADLEN];
      line_buff[j+1] = pos>=0 && pos<NCARPOSITIONS ? truck2glyphs[pos][i] : BLANK;
      }
    lcd.setCursor(0,i);  lcd.print(line_buff);
  }
      // Game over text
      // (keep first 2 "crash" columns intact)
      lcd.clear();
    
      for(int P=0; P<=1; P++){
        lcd.setCursor(0,0);  lcd.print(" Ya caractej Ya");
        lcd.setCursor(3,1);  lcd.print(puntos); lcd.print(" PUNTOS");
        delay(1000); lcd.clear(); 
        } puntos = 0; 
      }
      
    lcd.setCursor(0,0);   lcd.print("Press tecl cualq");
    lcd.setCursor(1,1);   lcd.print("juega de nuevo");
    delay(10); // Wait a bit between writes
    int xa; // Declarar variables
    xa = analogRead (0);
    if (xa < 1000) { Space(); }   // Cualquier tecla lcd keypad
    } 
    
    else {
    int prev_pos = road[(road_index-1)%ROADLEN];
    int this_pos = random(MAXROADPOS);
    
    while(abs(this_pos-prev_pos)<2){ this_pos = random(MAXROADPOS); } // don't jam the road
    road[road_index] = this_pos;
    road_index = (road_index+1)%ROADLEN;
      
    for(int i=0; i<2; i++){ if(crash){ line_buff[0]=crash2glyphs[car_pos][i]; } 
        
    else { line_buff[0]=car2glyphs[car_pos][i]; }    
 
    for(int j=0; j<ROADLEN; j++){
      int pos = road[(j+road_index)%ROADLEN];
      line_buff[j+1] = pos>=0 && pos<NCARPOSITIONS ? truck2glyphs[pos][i]:
      BLANK;
      }
    lcd.setCursor(0,i); lcd.print(line_buff);
  }
    delay(step_duration); 
    puntos = puntos + 1;
    
    if(step_duration > MINSTEPDURATION){ step_duration--;  }// go faster
    if (puntos >= 22000){ lcd.clear();
      for(int i=0; i<=9; i++){
        lcd.setCursor(0, 0); lcd.print("  Escapaste de  "); 
        lcd.setCursor(0, 1); lcd.print("  De las naves"); 
        delay(500); lcd.clear(); delay(500); } lcd.clear();
        lcd.setCursor(0, 0); lcd.print("    ¿FIN? "); delay(2000); lcd.clear();
        lcd.setCursor(0, 0); lcd.print("     FIN ");  delay(1000); 
        }
  }
}
}
//________________________________________________________________________________________________________________________________________
void saltos() { //while(1){ 
  //lcd.begin(16, 2);
  LINEA = 0; LINEA1 = 0; 
  LINEA = !LINEA;    LINEA1 = !LINEA1;
  lcd.setCursor(0, 0); lcd.print(" JUEGO");
  lcd.setCursor(0, 1); lcd.print("      DE SALTOS");
  SCORE = 0;   delay(250); lcd.clear(); delay(250); 
  int xW;   xW = analogRead (0); 
  if(xW < 1000){ saltoss(); } 
  saltos(); // }
}

void saltoss() {  //while(1){ 
  int bloque_x = random(5, 15);
  juego(bloque_x); 
  saltoss(); //}
}

int juego(int bloque_x){
  //lcd.begin(16, 2);
  for(int personaje_x=0; personaje_x<16; personaje_x++){ 
    lcd.clear(); 
    int personaje_y = LINEA1;
     
    int xy;   xy = analogRead (0); 
    if(xy < 12){ saltoo = 1; }        // if el button es RIGHT
    else if(xy < 110){ saltoo = 0; }  // if el button es UP
    else if(xy < 266){ saltoo = 1; }  // if el button es DAWN
    else if(xy < 420){ saltoo = 0; }  // if el button es LEFT
    else if(xy < 650){ saltoo = !saltoo; }  // if el button es SELECT

    if(saltoo == 0){ personaje_y = 0; } 
    if(saltoo == 1){ personaje_y = 1; } // if(saltoo == 1){ personaje_y = !LINEA1; }  
     
    personaje(personaje_x, personaje_y, 150);
    bloque(bloque_x, 150);
    detectar_colision(bloque_x, personaje_x, personaje_y); 
    }
//___________________________________________--------------------
  SCORE += 1;
  if (SCORE == 3) { LINEA1 = !LINEA1;  }
  if (SCORE == 6) { LINEA1 = !LINEA1;  }
  if (SCORE == 9) { LINEA1 = !LINEA1;  }
  if (SCORE == 12) { LINEA1 = !LINEA1;  }
  if (SCORE == 15) { LINEA1 = !LINEA1;  }
  if (SCORE == 18) { LINEA1 = !LINEA1;  }
  if (SCORE == 21) { LINEA1 = !LINEA1;  }
  if (SCORE == 24) { LINEA1 = !LINEA1;  }
  if (SCORE == 27) { LINEA1 = !LINEA1;  }
  if (SCORE == 31) { tiemp = num; lcd.clear();
   lcd.setCursor(0, 0); lcd.print("Ganaste el Juego");

      for (int a = 0; a < 3; a++) {  lcd.noDisplay(); delay(450); lcd.display(); delay(550);  }     // for (int a = 0; a < 12; a++) {  lcd.noDisplay(); delay(225); lcd.display(); delay(250);  } 
     // for (int b = 0; b < 1; b++) {
        for (int positionCounter = 0; positionCounter < 13; positionCounter++) { lcd.scrollDisplayLeft(); delay(150); }
        for (int positionCounter = 0; positionCounter < 29; positionCounter++) { lcd.scrollDisplayRight(); delay(150); }
        for (int positionCounter = 0; positionCounter < 16; positionCounter++) { lcd.scrollDisplayLeft(); delay(150); } //}   // 8700 mseg eso tarda este bucle  

      lcd.setCursor(0, 0); lcd.print("Ganaste el Juego");
      lcd.setCursor(0, 1); lcd.print("     YOU WIN");
      delay(3000);  lcd.clear();
      while(1) { SCORE = 0;
      lcd.setCursor(0, 0); lcd.print("   Presiona");
      lcd.setCursor(0, 1); lcd.print(" cualquier TECLA");
       int xc;   xc = analogRead (0); 
      if (xc < 1000) { saltoss(); } 
      delay(100);}
  }
    LINEA = !LINEA; //  LINEA1 = !LINEA1;
    tiemp = tiemp - 3;  lcd.clear();
   lcd.setCursor(0, 0); lcd.print("      Nivel     "); 
 if (SCORE < 10) {  lcd.setCursor(8, 1); lcd.print(SCORE); }
 if (SCORE >= 10) { lcd.setCursor(7, 1); lcd.print(SCORE); }
  delay(300);
}

void personaje(int x, int y, int d){
  lcd.setCursor(x, y);
  lcd.print("x");
  delay(tiemp);
}

void bloque(int posicion_bloque, int d){
  lcd.setCursor(posicion_bloque, LINEA);
  lcd.print("O");
  delay(tiemp);
}

void detectar_colision(int bloque_x, int personaje_x, int personaje_y){
  if(bloque_x == personaje_x && personaje_y == LINEA){
    lcd.clear();
    lcd.setCursor(0,0);
    String score_string = String(SCORE);
    lcd.print("  GAME OVER:" + score_string);
    delay(500); lcd.clear();

    lcd.setCursor(0, 0); lcd.print("    Llego al   "); 
    delay(500);

    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("      Nivel     "); 
 
 if (SCORE < 10) {  lcd.setCursor(8, 1); lcd.print(SCORE); }
 if (SCORE >= 10) {  lcd.setCursor(7, 1); lcd.print(SCORE); }
  delay(300);
   
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Perdio el Juego");
  lcd.setCursor(0, 1); lcd.print("    Lastima");  
  SCORE = 0; delay(1000); 
  lcd.clear(); personaje_x=1; 
  tiemp = num;  LINEA = 1;  LINEA1 = !LINEA1;
  saltos();
  }
}
//_________________________________________________________________________________________________________
void Cronometro() { 
  lcd.setCursor(0,0);  lcd.print("   Cronometro   ");
  delay(1000); // wait 1 seconds
  
  Crom();
}
//___________________________________________________________________________________________________
void displayResult() {
  float h, m, s, ms;
  unsigned long over;

  elapsed = finished - start;
  
  h = int(elapsed / 3600000);
  over = elapsed % 3600000;
  m    = int(over / 60000);
  over = over % 60000;
  s    = int(over / 1000);
  ms   = over % 1000;

  lcd.setCursor(0,0);
  lcd.print("Tpo transcurrido: ");

  // display the results
  lcd.setCursor(0,1);
  lcd.print(h, 0); // display variable 'h' - the 0 after it is the number of algorithms after a comma (ex: lcd.print(h, 2); would print 0,00
  lcd.print("h "); // and the letter 'h' after it
  lcd.print(m, 0);
  lcd.print("m ");
  lcd.print(s, 0);
  lcd.print("s ");
  lcd.print(ms, 0);
  lcd.print("ms");

  if(ms >= 996){ lcd.clear(); }
}
//____________________________________________________________________________________________-
void Crom() { 
  int tempu = 0;
  int lol = 0;  
  int hol = 1;
   hol = 1;
  while(1){
    if(hol == 1 ){ 
 if((unsigned long)(millis() - previousMillis) >= interval){
  previousMillis = millis();
   lol = lol + 1; 
 }
  if(lol == 10){
    lcd.setCursor(0,0);  lcd.print("LFT - Inc/Reset");
    lcd.setCursor(0,1);  lcd.print("SEL - Para.tiempo");  }

  if(lol == 19){ lcd.clear(); } // clear the display
  if(lol == 20){ lcd.setCursor(0,0);  lcd.print("DWN - Reset/Inc"); 
                   lcd.setCursor(0,1);  lcd.print("RHT - Reset/Crom");
                   }
  if(lol == 30){ lol = 0;  } }
 
    int xq; // Declarar variables
    xq = analogRead (0); // Asignar 'X' para las entradas analógicas Arduino Shields (botones)
  
  if(xq < 10){ lcd.clear(); Crom(); }      // if el button es RIGHT
 
  else if(xq < 110){ tempu = 2; }  // if el button es UP

  else if(xq < 265){  hol = 0;  tempu = tempu + 1;  delay(250); } // if el button es DAWN
       
    if(tempu == 1){  hol = 0;
      start = millis(); // inivia el tiempo de inicio para calcular el tiempo transcurrido
      delay(250); lcd.clear(); tempu = tempu + 1; }
    
    if(tempu == 2){
     finished = millis(); // detiene el tiempo para calcular el tiempo transcurrido
     displayResult(); // Mostrar los resultados en la función
     }
      
    if(tempu == 3){ tempu = 0; }
  
  else if(xq < 420){  hol = 0;
    start = millis(); // inivia el tiempo de inicio para calcular el tiempo transcurrido
    delay(200); lcd.clear();
    lcd.print("Empezado Tpo...");
    }  // if el button es LEFT
  
  else if(xq < 650){  hol = 0;
    finished = millis(); // detiene el tiempo para calcular el tiempo transcurrido
    displayResult(); // Mostrar los resultados en la función
    }  // if el button es SELECT
  }
}

//________________________________________________________________________________________-----------------
void Temporalizador() {
   //lcd.begin(16, 2);         // Configuramos el numero de columnas y filas del LCD. 
   pinMode(alarma, OUTPUT);  //Pin de alarma --> Salida
   digitalWrite(alarma, LOW);
 
 for(int i=0; i<=2; i++){
  lcd.setCursor(2,0);
  lcd.print("Temporizador");
  lcd.setCursor(4,1);
  lcd.print("MITTY");
  delay(500);
  lcd.clear();
  delay(500); } //Temporalizador()
  Temport();
}

void Temport() { //while(1){ 
//_____________________________________________________________
 lcd_key = read_LCD_buttons();  // read the buttons

 switch(lcd_key) {              // depending on which button was pushed, we perform an action
 
   case btnRIGHT: {
     if(ahoras == 9){ asegundos = 0; aminutos = 0; }
    if(ahoras < 9){ 
         asegundos = asegundos + 1;
      if(asegundos == 60){ asegundos = 0; aminutos = aminutos + 1;}
      if(aminutos == 60) aminutos = 0;
       delay(250);
       lcd.setCursor(0,0);
       lcd.print("Elige el tiempo");  //Muestra mensaje y las HH:MM:SS que vayamos aumentando
       
       lcd.setCursor(4,1);    

      if (ahoras < 10) lcd.print("0");    // Si las horas son menor que 10, pone un "0" delante.
       lcd.print(ahoras);                 // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
       lcd.print(":");

      if (aminutos < 10) lcd.print("0");  // Si los minutos son menor que 10, pone un "0" delante.
       lcd.print(aminutos);               // Sin este codigo, se muestra asi: H:M:S  (H:1:S)
       lcd.print(":");
       
      if(asegundos < 10) lcd.print("0"); // Si los segundos son menor que 10, pone un "0" delante.
       lcd.print(asegundos);              // Sin este codigo, se muestra asi: H:M:S  (H:M:1)
      }
     break;
     }
   case btnLEFT: {
        ahoras = ahoras + 1 ;
     if(ahoras == 10){ ahoras = 0; }
       delay(250);
       lcd.setCursor(0,0);
       lcd.print("Elige el tiempo");  //Muestra mensaje y las HH:MM:SS que vayamos aumentando
       
       lcd.setCursor(4,1);    

      if(ahoras < 10) lcd.print("0");    // Si las horas son menor que 10, pone un "0" delante.
       lcd.print(ahoras);                 // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
       lcd.print(":");

      if(aminutos < 10) lcd.print("0");  // Si los minutos son menor que 10, pone un "0" delante.
       lcd.print(aminutos);               // Sin este codigo, se muestra asi: H:M:S  (H:1:S)

       lcd.print(":");
      if(asegundos < 10) lcd.print("0"); // Si los segundos son menor que 10, pone un "0" delante.
       lcd.print(asegundos);              // Sin este codigo, se muestra asi: H:M:S  (H:M:1)
        
     break;
     }
   case btnUP: {
    if(ahoras == 9){ asegundos = 0; aminutos = 0; }
    if(ahoras < 9){
        aminutos = aminutos + 1;
         
       if(aminutos == 60){ aminutos = 0; ahoras = ahoras + 1;}
       if(ahoras == 60) ahoras = 0;
         delay(250);
         
       lcd.setCursor(0,0);
       lcd.print("Elige el tiempo");  //Muestra mensaje y las HH:MM:SS que vayamos aumentando
       
       lcd.setCursor(4,1);    

      if(ahoras < 10) lcd.print("0");    // Si las horas son menor que 10, pone un "0" delante.
       lcd.print(ahoras);                 // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
       lcd.print(":");

      if(aminutos < 10) lcd.print("0");  // Si los minutos son menor que 10, pone un "0" delante.
       lcd.print(aminutos);               // Sin este codigo, se muestra asi: H:M:S  (H:1:S)

       lcd.print(":");
      if(asegundos < 10) lcd.print("0"); // Si los segundos son menor que 10, pone un "0" delante.
       lcd.print(asegundos);              // Sin este codigo, se muestra asi: H:M:S  (H:M:1)              // Sin este codigo, se muestra asi: H:M:S  (H:1:S)
         }
     break;
     }
   case btnDOWN: {         
         segundostotal = asegundos + (aminutos * 60) + (ahoras * 60 * 60);  //Convierte el tiempo elegido en segundos!!
      
      //-------------------------------------------------------------------------------------------------
      // UNA VEZ PULSADO EL BOTON DE ARRANQUE Y ACUMULADO EL TIEMPO, ENTRA EN EL SIGUIENTE WHILE
      // Y NO FINALIZA HASTA TERMINAR LA CUENTA.
      //-------------------------------------------------------------------------------------------------

      while(segundostotal > 0){
       delay(1);          //Descontamos en periodos de 1 segundo
           conta++;
           
       if(conta == 1000){ conta = 0;
        digitalWrite(alarma, HIGH); 
        segundostotal--;    
  
        ahoras = ((segundostotal / 60)/ 60);   //Convertimos los segundos totales en horas
        aminutos = (segundostotal / 60) % 60;  //Convertimos los segundos totales en minutos
        asegundos = segundostotal % 60;        //Convertimos los segundos totales en periodos de 60 segundos

        lcd.setCursor(0,0);
        lcd.print("Tiempo restante");        //Mostramos mensaje de tiempo restante
 
        lcd.setCursor(4,1);
        if(ahoras < 10) lcd.print("0");     // Si las horas son menor que 10, pone un "0" delante.
        lcd.print(ahoras);                   // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
        lcd.print(":");

        if(aminutos < 10) lcd.print("0");   // Si los minutos son menor que 10, pone un "0" delante.
        lcd.print(aminutos);                 // Sin este codigo, se muestra asi: H:M:S  (H:1:S)

        lcd.print(":");
        if (asegundos < 10) lcd.print("0");  // si el valor de segundo esta por debajo de 9 (unidad) antepone un cero
        lcd.print(asegundos);                // Sin este codigo, se muestra asi: H:M:S  (H:M:1)
     }
//___________________Tiempo cumplido______________________________________________
          if (segundostotal == 0){ digitalWrite(alarma, LOW);             //Si finaliza el tiempo
            for(int i=0; i<=2; i++){                       //Bucle infinito mostrando mensaje y haciendo parpadear un led              
                  lcd.clear();
                  lcd.setCursor(5,0);
                  lcd.print("Tiempo");
                  lcd.setCursor(3,1);
                  lcd.print("Finalizado");
                  delay(500);
                  lcd.clear();
                  delay(500);
             }     
          }
        }
     break;
     }
   case btnSELECT: {
     segundostotal = asegundos + (aminutos * 60) + (ahoras * 60 * 60);  //Convierte el tiempo elegido en segundos!!
      
      //-------------------------------------------------------------------------------------------------
      // UNA VEZ PULSADO EL BOTON DE ARRANQUE Y ACUMULADO EL TIEMPO, ENTRA EN EL SIGUIENTE WHILE
      // Y NO FINALIZA HASTA TERMINAR LA CUENTA.
      //-------------------------------------------------------------------------------------------------

      while(segundostotal > 0){
       delay(1);          //Descontamos en periodos de 1 segundo
           conta++;
           
       if(conta == 1000){ conta = 0;
        digitalWrite(alarma, HIGH); 
        segundostotal--;    
  
        ahoras = ((segundostotal / 60)/ 60);   //Convertimos los segundos totales en horas
        aminutos = (segundostotal / 60) % 60;  //Convertimos los segundos totales en minutos
        asegundos = segundostotal % 60;        //Convertimos los segundos totales en periodos de 60 segundos

        lcd.setCursor(0,0);
        lcd.print("Tiempo restante");        //Mostramos mensaje de tiempo restante
 
        lcd.setCursor(4,1);
        if (ahoras < 10) lcd.print("0");     // Si las horas son menor que 10, pone un "0" delante.
        lcd.print(ahoras);                   // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
        lcd.print(":");

        if (aminutos < 10) lcd.print("0");   // Si los minutos son menor que 10, pone un "0" delante.
        lcd.print(aminutos);                 // Sin este codigo, se muestra asi: H:M:S  (H:1:S)

        lcd.print(":");
        if (asegundos < 10) lcd.print("0");  // si el valor de segundo esta por debajo de 9 (unidad) antepone un cero
        lcd.print(asegundos);                // Sin este codigo, se muestra asi: H:M:S  (H:M:1)
     }
//___________________Tiempo cumplido______________________________________________
          if (segundostotal == 0){ digitalWrite(alarma, LOW);             //Si finaliza el tiempo
            for(int i=0; i<=2; i++){                       //Bucle infinito mostrando mensaje y haciendo parpadear un led              
                  lcd.clear();
                  lcd.setCursor(5,0);
                  lcd.print("Tiempo");
                  lcd.setCursor(3,1);
                  lcd.print("Finalizado");
                  delay(500);
                  lcd.clear();
                  delay(500);
             }     
          }
     }    
     break;
     }
     
     case btnNONE: {
      if(ahoras == 9){ asegundos = 0; aminutos = 0; }
       lcd.setCursor(0,0);
       lcd.print("Elige el tiempo");  //Muestra mensaje y las HH:MM:SS que vayamos aumentando
       
       lcd.setCursor(4,1);    

      if(ahoras < 10) lcd.print("0");    // Si las horas son menor que 10, pone un "0" delante.
       lcd.print(ahoras);                 // Sin este codigo, se muestra asi: H:M:S  (1:M:S)
       lcd.print(":");

      if(aminutos < 10) lcd.print("0");  // Si los minutos son menor que 10, pone un "0" delante.
       lcd.print(aminutos);               // Sin este codigo, se muestra asi: H:M:S  (H:1:S)
       lcd.print(":");
       
      if(asegundos < 10) lcd.print("0"); // Si los segundos son menor que 10, pone un "0" delante.
       lcd.print(asegundos);              // Sin este codigo, se muestra asi: H:M:S  (H:M:1)
      
      digitalWrite(alarma, LOW);
     break;
     }
   }
  Temport();  //}
//_____________________________________________________________
}

