const int buttonPin = 4;  
const int booPin = 12; 
const int offPin = 8;
const int lightingPin = 13;


int buttonState = 0;     // current state of the button
int wrapUpState = 0;     // current state of the wrapup
int lastButtonState = 0; // previous state of the button
unsigned long startPressed = 0;    // the moment the button was pressed
unsigned long endPressed = 0;      // the moment the button was released
unsigned long holdTime = 0;        // how long the button was hold
unsigned long idleTime = 0;        // how long the button was idle
int showRun = 0;        // how long the show is


void setup() {
  pinMode(buttonPin, INPUT); // initialize the step pad as INPUT
  pinMode(booPin, OUTPUT); // initialize the booBox pad signal as OUTPOT
  pinMode(offPin, INPUT); // Check for last 30 seconds of feature
  pinMode(lightingPin, OUTPUT); // iLighting pin to Pharos
  Serial.begin(9600);        // initialize serial communication for debugging
}

void loop() {
  buttonState = digitalRead(buttonPin); // read the button input
  wrapUpState = digitalRead(offPin); // check if the BooBox is giving us the kill signal
  //Serial.print("End Warning Trigger: ");
  //Serial.println(wrapUpState);
  updateState();
  delay(1000);
  //if (buttonState != lastButtonState) { // button state changed
  //   updateState();
  //}
  lastButtonState = buttonState;        // save state for next loop
}

void updateState() {
  // the pad has been just pressed -- keep the contact ON to the BooBox until we get the wrapUpState OK
  if (buttonState == HIGH) {
      startPressed = millis();
      showRun = showRun + 1;
      idleTime = startPressed - endPressed;
      digitalWrite(booPin, HIGH);
      digitalWrite(lightingPin, HIGH);
      Serial.print("Pad is ACTIVE: ");
      Serial.println(idleTime);
      Serial.print("Show runtime is: ");
      Serial.println(showRun);
            
      
      if (wrapUpState == HIGH) {
          Serial.println("ACTIVE PAD-- Wrapping Up, cutting the pad for a five seconds!");
          digitalWrite(booPin, LOW);
          digitalWrite(lightingPin, LOW);
          showRun = 0;
          delay(5000);
      }
      if (showRun >= 95) {
          Serial.println("ACTIVE PAD-- No show is longer than 95 seconds!");
          digitalWrite(booPin, LOW);
          digitalWrite(lightingPin, LOW);
          showRun = 0;
          delay(5000);
      }

  // the pad has been just released, await 10 seconds to cut
  } else {
      endPressed = millis();
      holdTime = endPressed - startPressed;
      Serial.print("Show runtime is: ");
      Serial.println(showRun);
      showRun = showRun + 1;
      if (wrapUpState == HIGH) {
          Serial.println("INACTIVE PAD-- Wrapping Up, cutting the pad for a five seconds!");
          digitalWrite(booPin, LOW);
          digitalWrite(lightingPin, LOW);
          showRun = 0;
          delay(5000);
      }
      
      if (wrapUpState == LOW) {
        if (holdTime >= 500 && holdTime < 15000) {
            Serial.print("Pad is INACTIVE for .5-15 second: "); 
            Serial.println(holdTime);
            digitalWrite(booPin, HIGH);
            digitalWrite(lightingPin, HIGH);
        }

        if (holdTime >= 15000) {
            Serial.print("Pad is INACTIVE for 15 second or more: "); 
            Serial.println(holdTime);
            digitalWrite(booPin, LOW);
            digitalWrite(lightingPin, LOW);
            showRun = 0;
        }
      }

  }
}
