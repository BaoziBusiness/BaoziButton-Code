

/**************************************************************************************
 * 
 * 
 *                       T H E
 *                      
 *               B A O Z I   B U T T O N
 *   
 * 
 *                  v2.0  12.12.2018
 *                    by Ch. Münch
 
                  Licence: CC BY-SA 4.0
 * 
 * 
 * 
 *  Inspired by: Tasten Interrupt mit Entprellung
 *           of: Matthias Busse Version 1.0 vom 26.10.2014
 *         from: http://shelvin.de/eine-taste-per-interrupt-einlesen-und-entprellen/
 * 
 * 
 ***************************************************************************************/



 
//int LED=13;
int LED2=9;
int mp3pin=7;
volatile unsigned long alteZeit=0, entprellZeit=20, tilttime=0, stresstime=0, actiontime=0, randomtime=0;
volatile unsigned long stresspenaltytime=0, tiltpenaltytime=0;
volatile unsigned long counter=0, tilt=0, stress=0;
volatile bool tiltpenalty=false, stresspenalty=false;

bool debug = false;



void setup() {
  
  if (debug) Serial.begin(9600);
  if (debug) { Serial.println("Greetings from the Baozi Button!"); Serial.println(); }

  // digital pins
  //pinMode(LED, OUTPUT);    // LED Pin
  pinMode(LED2, OUTPUT);    // LED Pin
  pinMode(mp3pin, OUTPUT);    // mp3 Pin

  // mp3 normally HIGH
  digitalWrite(mp3pin, HIGH);
  
  // LED normally on
  //digitalWrite(LED, HIGH);
  // but fade in the beginning
  fadebuttonled_in(450);
  digitalWrite(LED2, HIGH);
    
  // interrupt pin
  pinMode(2, INPUT);       // Pin 2 ist INT0
  digitalWrite(2, HIGH);   // interner Pull up Widerstand auf 5V
  // Pin 2 (INT 0) geht auf 0V (LOW) dann interruptRoutine aufrufen
  attachInterrupt(0, CatchInterrupt, LOW);

  // start rand
  randomSeed(analogRead(0));
  
  // init times
  randomtime = random(50000);
  actiontime = millis();
  tilttime = millis();
  tiltpenaltytime = 10000;
  stresstime = millis();
  stresspenaltytime = 15000;
  
  
} /* setup */




void loop() {

/*
while(1) {
digitalWrite(mp3pin, HIGH);
delayMicroseconds(100);
digitalWrite(mp3pin, LOW);
delayMicroseconds(100);
}
*/
  
  // blinkfade every 10 to 60 sec.
  if (millis()-actiontime >= randomtime) {
    if (debug) { Serial.println("Action time"); }
    actiontime = millis();
    randomtime = random(10000, 60000);
    fadebuttonled(350);
    fadebuttonled(500);
  }


// perform tilt penalty
if (tiltpenalty) {
  fadebuttonled_out(400);
  if (debug) { Serial.println("Tilt penalty"); }
  delay(10000);  // tiltpenaltytime
  tiltpenalty = false;
  tilt=0;
  tilttime = millis();
  actiontime = millis();
  fadebuttonled_in(3400);
  attachInterrupt(0, CatchInterrupt, LOW);
  if (debug) { Serial.println("Recovered from tilt"); }
  }



  
// perform stress penalty
if (stresspenalty) {
  fadebuttonled_out(400);
  if (debug) { Serial.println("Stress penalty"); }
  delay(15000);  // stresspenaltytime
  stresspenalty = false;
  stress=0;
  stresstime = millis();
  actiontime = millis();
  fadebuttonled_in(3400);
  attachInterrupt(0, CatchInterrupt, LOW);
  if (debug) { Serial.println("Recovered from stress"); }
  }


  
}  /* main loop */




void CatchInterrupt() {

  // only act if debounce time is met
  if((millis() - alteZeit) > entprellZeit) { 
  
    counter++;

    // mp3 triggern, min. 50ms
    digitalWrite(mp3pin, LOW);
    delayMicroseconds(10000);
    delayMicroseconds(10000);
    delayMicroseconds(10000);
    delayMicroseconds(10000);
    delayMicroseconds(10000);
    delayMicroseconds(10000);
    digitalWrite(mp3pin, HIGH);

    // LEDs schalten
    //digitalWrite(LED, LOW);
    fadebuttonled(450);
    //digitalWrite(LED, HIGH); 
    digitalWrite(LED2, HIGH);


    // check: reset tilt counter?
    if (millis() - tilttime > 15000) {
      tilt=0;  
      if (debug) { Serial.println("Tilt reset"); }
      }
    // special tilt counter
    if (millis() - tilttime < 1000) {
      tilt++;
      if (debug) { Serial.print("tilt: "); Serial.println(tilt); }
      if (tilt >= 5) {
        detachInterrupt(0);
        tiltpenalty = true;
        if (debug) { Serial.println("TILT!"); }  
        return;
        }
      }


    // check: reset stress counter?
    if (millis() - stresstime > 10000) {
    stress=0;  
    if (debug) { Serial.println("Stress reset"); }
    }
    // normal stress counter 
    if (millis() - stresstime < 30000) {
      stress++;
      if (debug) { Serial.print("stress: "); Serial.println(stress); }
      if (stress >= 10) {
        detachInterrupt(0);
        stresspenalty = true;
        if (debug) { Serial.println("STRESS"); }
        return;
      }
    }
  
      
      
    // letzte Schaltzeit merken      
    alteZeit = millis(); 
    tilttime = millis(); 
    stresstime = millis(); 

    
    
  }
  
} /* ISR */






// make the button led fade soft out and in
void fadebuttonled(int d) {
//int d = 450;  // fade time
// fade button led
    for (int i=255; i>=5; i--) {
      analogWrite(LED2, i);  
      delayMicroseconds(d);
      }
   for (int i=5; i<=255; i++) {
      analogWrite(LED2, i);  
      delayMicroseconds(d*3);
      }
 }



// make button led fade in
void fadebuttonled_in(int d) {
  for (int i=0; i<=255; i++) {
      analogWrite(LED2, i);  
      delayMicroseconds(d*3);
      }
  }



// make button led fade out
void fadebuttonled_out(int d) {
  for (int i=255; i>=0; i--) {
      analogWrite(LED2, i);  
      delayMicroseconds(d*3);
      }
  }

