/** Arduino music spectrum analyzer - Making music visible!
 *  
 * author: Tijam Moradi
 * board: Arduino Uno / ATMega
 * version: 2.8
 * 
 * This code will use MSEQ7 to split a screenshot of incoming audio source.
 * It will analyze the 7 splitted band gains and map them to PWM output (RGB Leds)
 *  so that the colours respond to frequency. RED = bass, BLUE = mid, GREEN = high.
 *  Bass is optimized to run on 27Hz - 217Hz. 
 *  Treble treshold gives a nice party flavor to the lightshow! For even better epilepsia
 *  turn amplifier to -1 (you will lose some green lights tho)
 *  
 *  'MSGEQ7' is shortened to 'EQ'
 *  
 *  MOSFETs in this project require PWM to be <1KHz to avoid overheating. Pin
 *  9, 10 and 11 clocks have a frequency of 490Hz so there is no need to change that.
 *  However, it is changeable by writing settings to low-level memory.
 *  
 *  TODO:
 *  - Amplifier modifier could be made for all bands.
 *  - Amplifier and EQFilter (sensitivity) could have physical potentiometers
 *  Sensitivity is adjusted on MacBook auxilary port at max volume, but is usually the same on most devices.
 *  
 *  Spectrum:
 *  1 = 280Hz
 *  2 = 900Hz (Not stable)
 *  3 = 2,8KHz 
 *  4 = NOT STABLE
 *  5 = 90 - 180 Hz
 *  6 = 300 Hz
 *  7 = ON all the time - No response
 *  
 */

int ledPinB = 11;
int ledPinG = 9;
int ledPinR = 10;

// EQ control pins
int EQRead = 0;     // Input
int EQStrobe = 2;   // Output
int EQReset = 4;    // Output
int EQScreenshot[7];    // EQ splits the audio snipping to 7 pieces

// Program control parameters
int EQFilter = 780;     // Filters the unwanted noise from EQRead while no audio input, works as sensitivity modifier
int amplifier = 40;      // Amplifies bass signal by multiplier (-1 for flat maximum gain, 1 for no gain) 
int trebleTreshold = 50; // Value between 0...255. 255 for no cutting highs, 0 for cutting highs when any bass, 50 ~ 230Hz
boolean enhanceBass = true; // Cuts all other bands when low freq input is detected. Not suitable for melodic music
 

void setup() {
  // Debugging options
  Serial.begin(9600);

  // Pin config for MSGEQ7
  pinMode(EQRead, INPUT);
  pinMode(EQStrobe, OUTPUT);
  pinMode(EQReset, OUTPUT);

  // Set the start values for pins
  digitalWrite(EQReset, LOW);
  digitalWrite(EQStrobe, HIGH);

  // Setup 5 volt voltage fork for analog pin
  analogReference(DEFAULT); 
  
}

void loop() {
  // Ping EQ's reset pin to start strobe
  enableStrobe();

  // This loop gathers the screenshot and modifies it to proper format
  for (int i = 0; i < 7; i++) {
    digitalWrite(EQStrobe, LOW);  // Variables are global, thus can be accessed here
    delayMicroseconds(30);        // EQ Datasheet value to stabilize output
    
    EQScreenshot[i] = analogRead(EQRead); // Get the value
    
    // Limit range of screenshot values between filter and 1023
    EQScreenshot[i] = constrain(EQScreenshot[i], EQFilter, 1023);

    // Map value to match arduino output
    EQScreenshot[i] = map(EQScreenshot[i], EQFilter, 1023, 0, 255);
  
    // Strobe the EQ
    digitalWrite(EQStrobe, HIGH);
  }
  
  // Log values to serial for debugging
  logSerial(EQScreenshot);


  // Flash the RGB leds accordingly
  analogWrite(ledPinR, amplify(EQScreenshot[5], amplifier));  // Low's
  analogWrite(ledPinG, getHigh(EQScreenshot[3]));    // High's
  analogWrite(ledPinB, getMid(EQScreenshot[2]));    // Mid's

  // Dealys the visible time of bass due to human nature: Bass is easily heard but such 
  // short visible time is not easily registered. In short, visible time of significant bass is extended.
  if (enhanceBass && EQScreenshot[5] > trebleTreshold) {
    delay(20);
  }
}

/**
 * Function enables strobing. This should be called before screenshot.
 */
void enableStrobe() {
  digitalWrite(EQReset, HIGH);
  digitalWrite(EQReset, LOW);
}

/**
 * This function gives high only if low is OFF
 */
int getHigh(int sig) {
  return (enhanceBass && EQScreenshot[5] >= trebleTreshold) ? sig/8 : sig;
}

int getMid(int sig) {
  return (enhanceBass && EQScreenshot[5] >= trebleTreshold) ? sig/8 : sig;
}

/**
 * Amplifies given signal with amplifier parameter (multiplier). 
 * if multiplier = -1, max boost will be added.
 * After amplification it will constrain to [0...255]
 */
int amplify(int sig, int a) {
  boolean hasInput = (sig > 0);
  sig = sig*a;
  sig = constrain(sig, 0, 255);
  return (a == -1 && hasInput) ? 255 : sig;
}


/**
 * Writes debug info to serial
 */
void logSerial(int values[]) {
  for (int i = 0; i < 7; i++) {
    Serial.print(values[i]);
    Serial.print(" ");
  }
  Serial.println();
}

