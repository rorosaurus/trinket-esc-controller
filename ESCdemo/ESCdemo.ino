#include <Adafruit_SoftServo.h>

// This is our motor.
Adafruit_SoftServo myServo1;

#define SERVO1PIN 0
#define BUTTON_PIN 2
int led = 1; // blink 'digital' pin 1 - AKA the built in red LED

bool oldState = LOW;
int state = 0;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;            // any number is OK
  TIMSK |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)
  
  myServo1.attach(SERVO1PIN);
  myServo1.write(0);
  delay(30);

  // arm it
  for (int i=4; i<7; i++){
    myServo1.write(i);
  }
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) { // change ESC state (0-180)
//      0 dead
//      4-6 arm
//      30 lowest full speed
//      70 med
//      110 highest setting (no change beyond this)
//      180 technical max
      int stateNew = 0;
      if(state < 30) stateNew = 30;
      else if(state == 30) stateNew = 70;
      else if(state == 70) stateNew = 110;
      else if(state == 110) stateNew = 20;
      if (state < stateNew) {
        for (int i=state; i <=stateNew; i++){
          myServo1.write(i);
        }
      }
      else {
        for (int i=state; i >=stateNew; i--){
          myServo1.write(i);
        }
      }
  }
  }
  if(state%30 == 0) digitalWrite(led, HIGH);
  else digitalWrite(led, LOW);
  
  // Set the last button state to the old state.
  oldState = newState;
}

// We'll take advantage of the built in millis() timer that goes off
// to keep track of time, and refresh the servo every 20 milliseconds
// The SIGNAL(TIMER0_COMPA_vect) function is the interrupt that will be
// Called by the microcontroller every 2 milliseconds
volatile uint8_t counter = 0;
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, refresh the servos!
  if (counter >= 20) {
    counter = 0;
    myServo1.refresh();
  }
}
