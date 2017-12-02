

/*
 * Traffic light system. v.1.2
 * By Tijam Moradi
 * 
 * System has three leds; green, yellow and red. Default emitting 
 * is red. If sensor (or button) notices activity (HIGH), light will turn green.
 * Light will remain green while sensor is occasionally triggered upon treshold.
 * If treshold is triggered on 4 green cycles, red light wil lbe forced. 
 * 
 * If you want to extend this program, ýou can start by adding sensors to 
 * see if many cars are in queue. Also switch light to red / green can be animated with 
 * specific functions. For further extension there could be more lights and parallel 
 * queue tracking.
 */

/* init values */
// pin configuration
int sensor = 2;
int green = 3;
int yellow = 4;
int red = 5;

// counters
int switchState = 0;
int sensorTriggerCount = 0;
int greenCycles = 0;

int treshold = 3500;  // editable milliseconds for green light

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(sensor, INPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  switchState = digitalRead(sensor);  // read sensor
  if (switchState == LOW) {
    // sensor is not triggered
    sensorTriggerCount = 0; // reset counter
    greenCycles = 0;  // reset counter
    digitalWrite(red, HIGH);  // turn on red light
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    
  } else {
    //  sensor is triggered
    greenCycles += 1; // start counting cycles
    digitalWrite(red, LOW); // turn red light off
    if (sensorTriggerCount < 1) { // only scroll through yellow light on Cycle if 
      digitalWrite(yellow, HIGH); // sensor was triggered during the green light.
      delay(1000);                // This avoids yellow light to flash during queue.
      sensorTriggerCount += 1;
    }
    
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);  // turn on green light
    delay(treshold);
    if (greenCycles == 4){
      forceRedLight(10000);   // Force red light if green has been on for 4 cycles
    }
  }
  
}

/**
 * This method will turn light to red for desired time (milliSeconds).
 * After execution system will return to called location (void loop()).
 */
void forceRedLight(int milliSeconds) {
  digitalWrite(red, HIGH);
  digitalWrite(yellow, HIGH);
  digitalWrite(green, LOW);
  delay(1000);
  digitalWrite(yellow, LOW);
  delay(milliSeconds);
}

