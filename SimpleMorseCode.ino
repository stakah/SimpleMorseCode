/*
  Based on the code from:
  http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
*/

// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 100;           // interval at which to blink (milliseconds)

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

//                       0        1        2        3        4        5        6        7        8        9
//                       a        b        c        d        e        f        g        h        i        j
//                       k        l        m        n        o        p        q        r        s        t
//                       u        v        w        x        y        z
String morseCode[36] = { "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.",
                         ".-"   , "-..." , "-.-." , "-.."  , "."    , "..-." , "--."  , "...." , ".."   , ".---" ,
                         "-.-"  , ".-.." , "--"   , "-."   , "---"  , ".--." , "--.-" , ".-."  , "..."  , "-"    ,
                         "..-"  , "...-" , ".--"  , "-..-" , "-.--" , "--.." };
String ascii2Code =  "0123456789abcdefghijklmnopqrstuvwxyz";
String code;
int duration = 0;
bool sendingChar = false;
bool isPause = true;
bool isBetweenChar = false;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  sendMorseCode();
}

void serialEvent() {
  while (!stringComplete && Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  if (stringComplete) {
    Serial.println("Received:" + inputString);
  }

}

void sendMorseCode() {
  if (!stringComplete) return;
  
  if (inputString.length() < 1) {
    //Serial.println("All chars sent.");
    stringComplete = false;
    return;
  }
  if (!sendingChar) {
    String aux = inputString.substring(0,1);
    inputString = inputString.substring(1);
    aux.toLowerCase();
    sendingChar = true;
    char c = aux.charAt(0);
    //Serial.print("Sending char:" + aux);
    getCode(c);
  } else {
    // sendingChar
    sendCode();
  }
}

void getCode(char c) {
  for (int i=0; i<ascii2Code.length(); i++) {
    if (ascii2Code[i] == c) {
      code = morseCode[i];
      isPause = false;
      isBetweenChar = false;
      //Serial.println(" code:" + code);
      return;
    }
  }
  code = "";
  isPause = true;
  duration = 7;
  //Serial.println("\n Sending pause");
}

void sendCode() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis < interval) return;
  
  // save the last time you blinked the LED
  previousMillis = currentMillis;
  
  if (isBetweenChar) {
    digitalWrite(ledPin, LOW);
    isBetweenChar = false;
    sendingChar = false;
  }

  if (isPause) {
    digitalWrite(ledPin, LOW);
    duration --;
    if (duration == 0) {
      isPause = false;
      sendingChar = false;
    }
  }
  
  if (sendingChar) {
    if (duration > 0) {
      //debug();
      if (!isPause) {
        digitalWrite(ledPin, HIGH);
        tone(8, 440);
      }
      duration --;
    } else {
      digitalWrite(ledPin, LOW);
      noTone(8);
      if (code.length() > 0) {
        char dd = code.charAt(0);
        code = code.substring(1);
        if (dd == '.') duration = 1;
        else duration = 3;
      } else {
        isBetweenChar = true;
      }
    }
  }
}

void debug() {
  if (stringComplete)
    Serial.print("stringComplete: TRUE");
  else
    Serial.print("stringComplete: FALSE");

  if (sendingChar)
    Serial.print(", sendingChar: TRUE");
  else
    Serial.print(", sendingChar: FALSE");

 if (isPause)
   Serial.print(", isPause: TRUE");
 else
   Serial.print(", isPause: FALSE");

 if (isBetweenChar)
   Serial.print(", isBetweenChar: TRUE");
 else
   Serial.print(", isBetweenChar: FALSE");

 Serial.print(", code:" + code);
 
 Serial.println("");
}
