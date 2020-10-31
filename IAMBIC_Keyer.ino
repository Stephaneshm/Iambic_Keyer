
/**  Pinout ATtiny85
 
       arduino pin :     :  SOIC   :     : arduino pin
                         +---\_/---+
                     RST | 1     8 | VCC
  SPEED      4       PB3 | 2     7 | PB2     2     DAH
  TX         3       PB4 | 3     6 | PB1     1     DIT
                     GND | 4     5 | PB0     0     HP
                         +---------+                         

 *  FUSE L:0x02
 *  FUSE H:0xD5
 *  FUSE E:0xFF
 *  FUSE LB:0xFF      **/
 

// ===============================================================================================================================================
//                                                                   INCLUDE
// ===============================================================================================================================================
#include <Bounce2.h>                          // see Library for details ( https://github.com/thomasfredericks/Bounce2/wiki )
// ===============================================================================================================================================
//                                                                   DEFINE 
// ===============================================================================================================================================
//#define DEBUG 
#define Speed 3                               // define Speed PIN
#define TX    4                               // define TX PIN
#define HP    0                               // Define HP Pin ( can be cut off with inter inside )
#define Dit   1                               // Paddle DIT PIN
#define Dah   2                               // Paddle DAH PIN

// ===============================================================================================================================================
//                                                                   VARIABLE
// ===============================================================================================================================================
int CharSpace = 125;                          // Minimun Value 

#ifdef DEBUG                                  // If debug Send Serial Data for debugging ( use HP PIN for RX )
   #include <SoftwareSerial.h>
   SoftwareSerial mySerial(5, 0);             // RX on pin 5 ( RST) - not use here just for definition , HP on PB4
   unsigned long previousMillis=0 ;           // Use for Check SPEED
   unsigned long interval = 30000L;    
   #define Version  "1.0a"       
#endif


// ===============================================================================================================================================
//                                                                   OBJECT 
// ===============================================================================================================================================
Bounce debouncerDit = Bounce();               // for Debouncing DIT Paddle
Bounce debouncerDah = Bounce();               // For Debouncing DAH Paddle 

// ===============================================================================================================================================
//                                                                   READSPEED
// ===============================================================================================================================================
void ReadSpeed()
{
 int SpeedValue=analogRead(Speed);            // Read Speed From PB3 ( Value 0-1024 )
 CharSpace=(CharSpace+SpeedValue)/6;          // 1024+125=1149/6-> MAX  0+125/6 ->MIN
 #ifdef DEBUG                                 // If debug Print SpeedValue ans CharSpace
   mySerial.print("SpeedValue : ");
   mySerial.println(SpeedValue);
   mySerial.print("CharSpace : ");
   mySerial.println(CharSpace);
 #endif
}

// ===============================================================================================================================================
//                                                                   SENDTX
// ===============================================================================================================================================
void SendTX(int Timing){                      // 1=dit  3-Dah
    #ifndef DEBUG                             // If NOT Debug Active HP Otherwise It's SERIAL DEBUG
      digitalWrite(HP,HIGH);
    #endif
    digitalWrite(TX,HIGH);                    // Send HIGH to TX
    delay(Timing * CharSpace);                // Duration of DIT or DAH 
    #ifndef DEBUG                             // If NOT Debug Cut off HP Otherwise It's SERIAL DEBUG  
      digitalWrite(HP,LOW);
    #endif
    digitalWrite(TX,LOW);                     // Switch to LOW TX
    delay (CharSpace);                        // duration of Silence
}

// ===============================================================================================================================================
//                                                                   SETUP  
// ===============================================================================================================================================
void setup() {
  analogReference(DEFAULT);                   // define AnalogRef ( see arduino DOC )
  pinMode(Speed,INPUT);                       // Define Speed PIN Mode
  pinMode(Dit,INPUT_PULLUP);                  // define DIT PIN Mode
  pinMode(Dah,INPUT_PULLUP);                  // Define DAH Pin Mode
  pinMode(TX, OUTPUT);                        // Define TX PIN Mode
  digitalWrite(TX, LOW);                      // Put LOW to TX Pin
  #ifdef DEBUG                                // If DEBUG Active Serial
    mySerial.begin(9600);   
    mySerial.println("-------------- START --------------------");
    mySerial.print(" Version "); mySerial.print(Version); mySerial.println(" - Date : 2020-10-31 by SHM  ");          // Yes it's HALOWEEN ! 
    mySerial.println();          
    delay(1000);
  #else                                       // If NOT define HP PINMode
    pinMode(HP, OUTPUT); 
    digitalWrite(HP, LOW);
    delay(1000);
    digitalWrite(HP, HIGH);                   // With BIP duration 1 sec for Starting NORMAL Mode
    delay(2000);
    digitalWrite(HP, LOW);
  #endif
  debouncerDit.attach(Dit);                   // Attach PIN to Object
  debouncerDit.interval(5);                   // interval in ms
  debouncerDah.attach(Dah);                   // Attach PIN to Object
  debouncerDah.interval(5);                   // interval in ms
  delay(3000);                                // Delay for Stability of Voltage Comparaison
  ReadSpeed();                                // need a RST to change SPEED if not debug MODE
 }


// ===============================================================================================================================================
//                                                                   LOOP
// ===============================================================================================================================================
void loop() {

  #ifdef DEBUG                                 // If DEBUG p^rint the SPEED Each Interval   
     if( millis() - previousMillis >= interval) {
          previousMillis = millis();   
          ReadSpeed(); 
          }
  #endif
  debouncerDit.update();                      // Update the DIT Pin
  debouncerDah.update();                      // Update the DAH Pin
  if(debouncerDit.fell())                     // If PAddle DIT Active
     { 
      SendTX(1);                              // Send DIT
      #ifdef DEBUG                    
        mySerial.println("DIT");
       #endif
     }
   if(debouncerDah.fell())                    // If PAddle DAH Active
     {
      SendTX(3);                              // Send DAH
      #ifdef DEBUG 
        mySerial.println("DAH");
      #endif
     }
}
