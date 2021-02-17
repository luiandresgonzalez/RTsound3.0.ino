
/* REACTION TIME (with 2 leds) v3.0
    Luis Andrés Gonzalez, January 2016
    A reaction time game that interfaces with Processing to record reaction time in a CSV file. 
    
    This game works as following:
        1.  wait for a button press. led 3 fades in and out indicating a stand by status. 
        2.  once this happens, led 2 blinks once indicating the game begins. "9090" is printed in the serial 
            is a code that sends a signal to the PC to play a sound).
        3.  after a random lapse, led 1 lights up, and a clock starts to measure the time until you press 
            the button again. In this lapse of time, nothing is printed.
        4.  if you press the button after the light is turned on, it shows the reaction time in seconds through 
            the serial (which the computer stores in a CSV file).
        5.  if you press before the led lights up, it blinks three times and the game ends. ("1010" is 
            a code that signals the PC to play an error sound). it then should return to an idle state and return to step 1.
        6.  if you don't press the button after a time, the led blinks twice and the game ends. returns to step 1. 
            (code "7777" is sent through the serial comm).
        7.  once the game ends, it only begins again if you press the button again. 
        
   Reaction time original version from http://www.instructables.com/id/Arduino-Reaction-Time-Tester/?ALLSTEPS
   Send data to processing via the Serial Port original from By Elaine Laguerta http://url/of/online/tutorial.cc
   Special thanks to user BulldogLowell from Arduino Forums for helping in the improvement of this code. 
*/
#define TIMEOUT_MILLISECONDS  5000UL  // not too tough; make smaller for a bigger challenge

const char* const stateText[] = { "Stand By", "Active Game", "Early Press", "Good Press", "Late Press"};

enum GameState {
  STAND_BY,
  ACTIVE_GAME,
  EARLY_PRESS,
  GOOD_PRESS,
  LATE_PRESS,
};


const byte buttonPin = 6;  // pin where the button will be connected
const byte ledPin1 = 3 ;   // Left LED
const byte ledPin2 = 8 ;   // Middle LED
const byte ledPin3 = 11;   // Right LED

GameState gameState = STAND_BY;
GameState lastState;

// declare some variables:
boolean lastButton = LOW;
boolean pressed = LOW;
unsigned long lastMillis = 0;
unsigned long elapsedMillis;


long endTime;
long startTime;
int randomMillis;
long beginTime;
float elapsedTime;

float totalTime;
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void setup() {
  // Setup button and LEDs:
  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // Begin serial communication
  Serial.begin(9600);
 
}

void loop() {
  endTime = millis ();
  // read the pushbutton input pin in the current loop:
  pressed = checkButton();

  if (gameState == STAND_BY) {
    Fade();
    Light(ledPin1,"off");
    if (pressed) {
      startTime = millis();
      Light(ledPin1, "off");
      randomMillis = random(2, 8) * 1000; // generates a random number of milliseconds between 2 and 5
      Blink(1);

      gameState = ACTIVE_GAME;
      Light(ledPin3,"off");
      Serial.println("9090"); // signal code for start sound
      beginTime = millis() - lastMillis;
     

    }
  }

  else if (gameState == ACTIVE_GAME) {
    elapsedMillis = millis() - lastMillis;

    if (elapsedMillis >= randomMillis) {
      if (elapsedMillis < randomMillis + TIMEOUT_MILLISECONDS) {
        Light(ledPin1, "on");
       


      }
      else if (elapsedMillis > randomMillis + TIMEOUT_MILLISECONDS) {
        Serial.println("7777");
        delay(1000);
        Light(ledPin1, "off");
        gameState = LATE_PRESS;
      }

    }
    else if (pressed)
    {
      if (digitalRead(ledPin1))
      {
        gameState = GOOD_PRESS;

      }
      else
      {
        gameState = EARLY_PRESS;
      }
    }


  }
  else if (gameState == EARLY_PRESS) {

    Serial.println("1010"); // signal code for early response
    Blink(3);
    gameState = STAND_BY;
    delay(1000);

  }
  else if (gameState == GOOD_PRESS) {

    gameState = STAND_BY;
   
    long reactionTime = (endTime - startTime - randomMillis);
    Serial.println(reactionTime);
   
   
  }
  else if (gameState == LATE_PRESS) {
    gameState = STAND_BY;
    delay(1000);
  }


}




bool checkButton()
{
  static byte lastButtonState = 1;
  byte buttonState = digitalRead(buttonPin);
  if (buttonState == 0 && lastButtonState == 1 && millis() - lastMillis > 5UL) // <<<<<<< adjust 75 down as far as possible
  {
    lastMillis = millis();
    return true;
  }
  lastButtonState = buttonState;
  return false;
}

void Light(int lednumber, String onOff) {
  if (onOff == "on") {
    digitalWrite(lednumber, HIGH);

  } else if (onOff == "off") {
    digitalWrite(lednumber, LOW);
  }
}
void Blink(int repetitions) {
  for (int i = 0; i < repetitions; i++) {
    digitalWrite(ledPin2, HIGH);
    delay(100);
    digitalWrite(ledPin2, LOW);
    delay(100);
  }
}

void Fade() {
  analogWrite(ledPin3, brightness);
  brightness = brightness + fadeAmount;
  if (brightness == 0 || brightness == 255) {
    fadeAmount = -fadeAmount ;
  }
  delay(30);
}



