// Windshield Wiper - (c) 2016 William Butler

// Serial output interferes with the variable speed setting.  
// These can be turned back on if you want to see the 
// calculations in the Serial Monitor and don't care about 
// the actual performance of the servo.

// Servo library
#include <Servo.h>

// Declare the servo
Servo myServo;

// The two potentiometers
int const potPin0 = A0;
int const potPin1 = A1;

// The full span of the servo
int const span = 180;     // This can be made smaller to 
                          // reduce the angle of the servo's 
                          // rotation

int const spanPace = 500; // This number works well with my 
                          // servo to give it a visually 
                          // satisfying turn-around

// Values read from the pots
int pot0Val;
int pot1Val;

// Translated values of the pots
int pot0Selection;
int pot1Speed;

int lastAngle;      // Where we left off the last time 
                    // through the loop
int targetAngle;    // Where we're going
int pace;           // How fast we're going to get there 
                    // (milliseconds)
int increment;      // How big our steps will be (degree 
                    // per servo write)
long lastMillis;    // The last time we did anything 
                    // (milliseconds)
int spinDirection;  // Which direction the servo spins
          // (affects angle calculations)


void setup() {
  // PWM pin for controlling the servo
  myServo.attach(9);

  // Intializations
  lastAngle = 0;
  targetAngle = 1;    // If I set this to 0, the motor 
                      // made noises as if it were moving.  
                      // At 1, the motor was off.
  pace = 20;          // An initial amount of time between 
                      // issuing instructions
  increment = 1;      // A slow start of degrees
  lastMillis = 0;     
  spinDirection = 1;  // Positive or negative, this will 
                      // control whether we are adding the 
                      // increment or subtracting it.

  myServo.write(targetAngle);  // Always start in the same 
                               // position

//  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();  // Get the 
                                           // current "time"

  // If enough time has passed (time being more milliseconds 
  // have passed than our current pace requires)
  // then read the pots and do something
  if (currentMillis - lastMillis > pace) {
    //Serial.print("Last/Curr/Pace: ");
    //Serial.print(currentMillis);
    //Serial.print(" / ");
    //Serial.print(lastMillis);
    //Serial.print(" / ");
    //Serial.print(pace);
    lastMillis = currentMillis;            // Save for the 
                                           // next loop
    pot0Val = analogRead(potPin0);          
    pot1Val = analogRead(potPin1);
    pot0Selection = map(pot0Val, 0, 1023, 0, 4); // This maps 
                                     // the pot to 5 values 
                                     // when I only want 4 
                                     // settings: 
                                     // On/Variable/Low/High
                                     // If we map to 4 values, 
                                     // the last setting is 
                                     // only active if the pot 
                                     // is turned all 
                                     // the way to 1023.
    pot1Speed = map(pot1Val, 0, 1023, 200, 50); // Map the 
                                     // speed backwards from 
                                     // 50 millis to 200 millis.  
                                     // Turning the pot all the
                                     // way up feels better if 
                                     // we call that setting   
                                     // "fastest" speed
    //Serial.print("\tpot0Val/Selection: ");
    //Serial.print(pot0Val);
    //Serial.print(" / ");
    //Serial.print(pot0Selection);
    //Serial.print("\t pot1Val/Speed: ");
    //Serial.print(pot1Val);
    //Serial.print(" / ");
    //Serial.print(pot1Speed);
    //Serial.print("\t lastAngle: ");
    //Serial.print(lastAngle);

    // Off.  Zero has a meaning in the calculations below
    if (pot0Selection == 0) {
      increment = 0;
      pace = 0;
    }

    // Variable speed. Both pace and increment will change.
    if (pot0Selection == 1){
      pace = pot1Speed;                     // The speed value 
                               // we mapped from the pot's value 
                                     // value 
      increment = map(pace, 50, 200, 4, 1); // At slower speeds,
                               // we increment the angle slowly.
                               // At higher speeds, we increment 
                               // the angle quickly
    }

    // Low speed.  A fixed setting.
    if (pot0Selection == 2) {
      increment = 3;
      pace = 50;
    }

    // High speed. A fixed setting. We take into account that 
    // we have values from 0-4 and that 4 is only used when the 
    // pot is at it's highest setting.
    if (pot0Selection > 3) {
      increment = span;        // High speed will use the entire 
                               // span for calculating the new
                               // target angle
      pace = spanPace;         // Most number of millis we will 
                               // wait for the servo to 
                               // reach it's full speed target.
    }

    //Serial.print("\t Increment: ");
    //Serial.print(increment);
    //Serial.print("\t Pace: ");
    //Serial.print(pace);

    // If pace is 0, we don't want to move the servo at all.  
    // If it is any other number, we have business to attend 
    if (pace > 0){
      // Calculate the new angle by using the last angle we set. 
      // spinDirection will make this either an addition or 
      // subtraction calculation
      targetAngle = lastAngle + (spinDirection * increment);

      // Any advance beyond the span limit will just be set to 
      // that limit.
      // Turn around once we've reached the span limi
      if (targetAngle >= span) {
        targetAngle = span;
        spinDirection = -1;   // Turn around and go back
      }

      // If we go below 0, set the target to zero.
      // Change direction when we've reached 0
      if (targetAngle <= 0){
        targetAngle = 0;
        spinDirection = 1;    // Turn around and go back
      }

      // Remember for the next time through
      lastAngle = targetAngle;
      
      //Serial.print("\t targetAngle: ");
      //Serial.print(targetAngle);
      //Serial.print("\t Direction: ");
      //Serial.print(spinDirection);

      // Finally!  We ge to set the target angle.
      myServo.write(targetAngle);
    }
    //Serial.println(" ");
  }
}
