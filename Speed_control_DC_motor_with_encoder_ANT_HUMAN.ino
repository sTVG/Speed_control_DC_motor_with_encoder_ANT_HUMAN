//This code was produced by sTVG at www.themakerspace.co.za program designed for Brushless DC motor with encoder 24v = 240RPM

// period of pulse accumulation and serial output, milliseconds
#define MainPeriod 100 // time between readings of the encoder

/*
 * push button to start
 * spin up for 15 seconds to 240 rpm
 * hold for 120 seconds 135 seconds total
 * spin down for 15 seconds 150 seconds total
*/
#define delayStart    200     // must be greater than 0, probably safe to be greater than 100, bust be less than spinupTime
#define spinupTime    10000   // must be greater than delay start and less than spinTime
#define spinTime      25000  // must be greater than spinupTime and less than spindownTime
#define spindownTime  40000  // must be greater than spinTime

long previousMillis1 = 0; // will store last time of the cycle end

volatile unsigned long duration=0; // accumulates pulse width
volatile unsigned int pulsecount=0;
volatile unsigned long previousMicros=0;

//const int speedoPin=2; //Green wire for encoder - 4 pulses per revolution of motor 114 pulses per rev of gearbox. using interrupt pin 0 = GPIO2
const int spinSpeedPin=3; //Blue wire for speed control using PWM
const int spinDirectionPin=4; // Yellow wire for Direction control
const int brakePin=5; //White wire for brake or E-stop
const int buttonPin=8; //Start
const int ledPin=13; //LED output pin
//emergency stop - hard wired to ground

int cycleState = 0;
int RPM;
int spinSpeed;
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int counter;
long milli2;

long countAtStop = 0;     // will store last time of the cycle end


  unsigned long speedee ;
  int map1 ;
  int map2 ;
  int pwm1 ;
  int pwm2 ;


void setup()
{
  
    pinMode(brakePin, OUTPUT);
    pinMode(spinSpeedPin, OUTPUT);
    pinMode(spinDirectionPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    Serial.begin(19200); 
    attachInterrupt(0, myinthandler, RISING);
    digitalWrite(spinSpeedPin, HIGH);
    digitalWrite(ledPin, LOW);

}

void loop()
{


 buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH && cycleState == 0) {
      // if the current state is HIGH then the button
      // wend from off to on:
      lastButtonState = !buttonState;
      motorCycle();
     // flash LED
    }
    
     if (buttonState == LOW)
     {
      digitalWrite(spinSpeedPin,HIGH);
      cycleState = 0;
      lastButtonState = !buttonState;
      countAtStop = millis() ;
      digitalWrite(ledPin,LOW);
    }
    
}


void motorCycle()
{
  digitalWrite(brakePin,HIGH); // LOW = Brake, High = no brake
  digitalWrite(spinDirectionPin, LOW); // LOW = CW, HIGH = CCW

 unsigned long milliCounter = millis();
  if (milliCounter - previousMillis1 >= MainPeriod) 
  {
    previousMillis1 = milliCounter;   
    // need to bufferize to avoid glitches
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount; //how many pulses per MainPeriod (100 millis)
    float secs = MainPeriod*0.001;
    float  revs = secs*2 ; // measured pulses per gearbox revolution to calculate RPM every 100 millis
    unsigned long rpm = _pulsecount/revs;
    duration = 0; // clear counters
    pulsecount = 0;
    float Freq = 1e6 / float(_duration);
    Freq *= _pulsecount; // calculate F
      
    Serial.print(secs);
    Serial.print(" secs ,");
    Serial.print(milliCounter);
    Serial.print(" counter ,");
    Serial.print(_pulsecount);
    Serial.print(" ");
    Serial.print("pulses");
    Serial.print(" , ");
    Serial.print(countAtStop);
    Serial.print(" prevTime2 , ");
    Serial.print(milli2);
    Serial.print(" milli2 , ");
    Serial.print(buttonState);
    Serial.print(" buttonState , ");
    Serial.print(cycleState);
    Serial.print(" cycle , ");
    Serial.print(rpm);
    Serial.println("rpm");
    
  RPM = rpm;
  }

milli2 = milliCounter - countAtStop;

digitalWrite(ledPin, HIGH); 
    
if(milli2 < spindownTime )
{  
  if( spinTime <= milli2 && milli2 < spindownTime)
   {
      map1=spinTime;
      map2=spindownTime-spinTime;
      pwm1=0;
      pwm2=255;
      speedee = map(milli2,map1,map2,pwm1,pwm2);
      analogWrite(spinSpeedPin,speedee);
      digitalWrite(ledPin, HIGH); //could add flashing function matching deceleration
    }

if( spinupTime <= milli2 && milli2  < spinTime)
   {
      map1=spinupTime;
      map2=spinTime-spinupTime;
      pwm1=0;
      pwm2=0;
   speedee = map(milli2,map1,map2,pwm1,pwm2);
    analogWrite(spinSpeedPin,0);
    digitalWrite(ledPin, HIGH); //could add flashing function matching speed
   }
   
  if ( delayStart <= milli2 && milli2 < spinupTime) 
  { 
      map1=delayStart;
      map2=spinupTime-delayStart;
      pwm1=255;
      pwm2=0;
    speedee = map(milli2,map1,map2,pwm1,pwm2);
    analogWrite(spinSpeedPin,speedee);
    digitalWrite(ledPin, HIGH); //could add flashing function matching acceleration
  }
  
   
   if ( milli2 < delayStart)
  {
    analogWrite(spinSpeedPin,255);
    digitalWrite(ledPin, HIGH); //need to make this flash
  }
  
 }

  if(milli2 >= spindownTime )
  {
  //runs at the end of every cycle
  digitalWrite(ledPin, LOW);
  digitalWrite(spinSpeedPin,HIGH);
  countAtStop = milliCounter;  
  cycleState++;

  }

}


void myinthandler() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;
}



