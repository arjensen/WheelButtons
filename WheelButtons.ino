#include <Keyboard.h>
#include <Joystick.h>

#define BUTTON_FLAG_PADDLE_LEFT    0x0001
#define BUTTON_FLAG_PADDLE_RIGTH   0x0002
#define BUTTON_FLAG_TOP_MOST_LEFT  0x0004
#define BUTTON_FLAG_TOP_LEFT       0x0008
#define BUTTON_FLAG_TOP_MOST_RIGHT 0x0010
#define BUTTON_FLAG_TOP_RIGHT      0x0020
#define BUTTON_FLAG_JOY_LEFT       0x0040
#define BUTTON_FLAG_JOY_UP         0x0080
#define BUTTON_FLAG_JOY_RIGHT      0x0100
#define BUTTON_FLAG_JOY_DOWN       0x0200

// Alternative mode buttons
#define MODE_FLAG_0    BUTTON_FLAG_TOP_MOST_RIGHT
#define MODE_FLAG_1    BUTTON_FLAG_TOP_MOST_LEFT

// Button to pin Mapping 
#define PIN_PADDLE_LEFT     3
#define PIN_PADDLE_RIGTH    2
#define PIN_TOP_MOST_LEFT   8
#define PIN_TOP_LEFT        9
#define PIN_TOP_MOST_RIGHT 16
#define PIN_TOP_RIGHT      10
#define PIN_JOY_LEFT        4
#define PIN_JOY_UP          5
#define PIN_JOY_RIGHT       6
#define PIN_JOY_DOWN        7

#define BUTTON_CNT         10
#define START_PIN           2

// Last state of the button
int lastButtonFlags = 0;
int lastButtonFlagsMom = 0;
int modeFlags = 0;

// Create the Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_GAMEPAD, BUTTON_CNT, 0,
  false, false, false, false, false, false,
  false, false, false, false, false);

void setup() {
  // Initialize Button Pins
  pinMode(PIN_TOP_LEFT, INPUT_PULLUP);
  pinMode(PIN_TOP_MOST_LEFT, INPUT_PULLUP);
  pinMode(PIN_TOP_RIGHT, INPUT_PULLUP);
  pinMode(PIN_TOP_MOST_RIGHT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_LEFT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_RIGTH, INPUT_PULLUP);
  pinMode(PIN_JOY_LEFT, INPUT_PULLUP);
  pinMode(PIN_JOY_UP, INPUT_PULLUP);
  pinMode(PIN_JOY_RIGHT, INPUT_PULLUP);
  pinMode(PIN_JOY_DOWN, INPUT_PULLUP);  

  // Initialize Joystick Library
  Joystick.begin();
  Serial.begin(9600);
}

void SendKeyboardKeyCombo(int key1, int key2) {
  Keyboard.press(key1);
  delay(200);
  Keyboard.press(key2);
  delay(200);
  Keyboard.releaseAll();
}

int readButtons() {
  // Read buttonIndex values
  int currentButtonFlags = 0;

  if (!digitalRead(PIN_TOP_LEFT)) {
    currentButtonFlags |= BUTTON_FLAG_TOP_LEFT;
    //Serial.write("BUTTON_FLAG_TOP_LEFT\n");    
  }
  if (!digitalRead(PIN_TOP_MOST_LEFT)) {
    currentButtonFlags |= BUTTON_FLAG_TOP_MOST_LEFT;
    //Serial.write("BUTTON_FLAG_TOP_MOST_LEFT\n");    
  }
  if (!digitalRead(PIN_TOP_RIGHT)) {
    currentButtonFlags |= BUTTON_FLAG_TOP_RIGHT;
    //Serial.write("BUTTON_FLAG_PADDLE_RIGTH\n");
  }
  if (!digitalRead(PIN_TOP_MOST_RIGHT)) {
    currentButtonFlags |= BUTTON_FLAG_TOP_MOST_RIGHT;
    //Serial .write("BUTTON_FLAG_TOP_MOST_RIGHT\n");    
  }
  if (!digitalRead(PIN_PADDLE_LEFT)) {
    currentButtonFlags |= BUTTON_FLAG_PADDLE_LEFT;
    //Serial.write("BUTTON_FLAG_PADDLE_LEFT\n");
  }
  if (!digitalRead(PIN_PADDLE_RIGTH)) {
    currentButtonFlags |= BUTTON_FLAG_PADDLE_RIGTH;
    //Serial.write("BUTTON_FLAG_PADDLE_RIGTH\n");
  }
  if (!digitalRead(PIN_JOY_LEFT)) {
    currentButtonFlags |= BUTTON_FLAG_JOY_LEFT;
    //Serial.write("BUTTON_FLAG_JOY_LEFT\n");
  }
  if (!digitalRead(PIN_JOY_UP)) {
    currentButtonFlags |= BUTTON_FLAG_JOY_UP;
    //Serial.write("BUTTON_FLAG_JOY_UP\n");
  }
  if (!digitalRead(PIN_JOY_RIGHT)) {
    currentButtonFlags |= BUTTON_FLAG_JOY_RIGHT;
    //Serial.write("BUTTON_FLAG_JOY_RIGHT\n");    
  }
  if (!digitalRead(PIN_JOY_DOWN)) {
    currentButtonFlags |= BUTTON_FLAG_JOY_DOWN;
    //Serial.write("BUTTON_FLAG_JOY_DOWN\n");    
  } 

  return currentButtonFlags;
}

void loop() {
  int currentButtonFlags = readButtons();  

  //Filter out very short pesses
  if (lastButtonFlagsMom != currentButtonFlags) {
    lastButtonFlagsMom = currentButtonFlags;
    delay(5);
    return;
  }

  modeFlags = currentButtonFlags & (MODE_FLAG_0 | MODE_FLAG_1);
    
  // Set joystick button values
  for (int buttonNr = 0; buttonNr < BUTTON_CNT; buttonNr++) {    
    
    int flagMask = 0x0001 << buttonNr;
    int currentFlag = currentButtonFlags & flagMask;
    if (currentFlag != (lastButtonFlags & flagMask)) {

      if (modeFlags & MODE_FLAG_0) {
        switch (currentFlag) {
          case BUTTON_FLAG_JOY_LEFT:
            Keyboard.write(KEY_LEFT_ARROW);            
            continue;
          case BUTTON_FLAG_JOY_UP:
            Keyboard.write(KEY_UP_ARROW);
            continue;
          case BUTTON_FLAG_JOY_RIGHT:
            Keyboard.write(KEY_RIGHT_ARROW);
            continue;
          case BUTTON_FLAG_JOY_DOWN:
            Keyboard.write(KEY_DOWN_ARROW);
            continue;
          case BUTTON_FLAG_TOP_MOST_LEFT:
            SendKeyboardKeyCombo(KEY_LEFT_CTRL,' ');
            continue;
          default:
            break;
        }        
      }
      else if (modeFlags & MODE_FLAG_1) {
        switch (currentFlag) {
          case BUTTON_FLAG_JOY_LEFT:
            // Teleport to pit and open setup screen
            SendKeyboardKeyCombo(KEY_LEFT_CTRL,'v');
            continue;
          case BUTTON_FLAG_JOY_UP:
            // Running start
            SendKeyboardKeyCombo(KEY_LEFT_CTRL,'y');
            continue;
          case BUTTON_FLAG_JOY_RIGHT:
            // Session restart
            if (BUTTON_FLAG_PADDLE_LEFT == (currentButtonFlags & BUTTON_FLAG_PADDLE_LEFT)) {
              SendKeyboardKeyCombo(KEY_LEFT_CTRL,'n');           
            }
            continue;
          case BUTTON_FLAG_JOY_DOWN:
            // Exit the game
            if (BUTTON_FLAG_PADDLE_LEFT == (currentButtonFlags & BUTTON_FLAG_PADDLE_LEFT)) {
              SendKeyboardKeyCombo(KEY_LEFT_CTRL,'e');
            }
            continue;
          default:
            break;
        }        
      }
      
      Serial.write(48+buttonNr);
      Serial.write("\t");
      if (currentFlag) { 
        Joystick.setButton(buttonNr, 1);
        Serial.write("ON\n");      
      }
      else { 
        Joystick.setButton(buttonNr, 0);
        Serial.write("OFF\n");      
      }
    }
  }

  if (lastButtonFlags == currentButtonFlags) {
    delay(5);
  }
  else {
    lastButtonFlags = currentButtonFlags;
    delay(50);
  }
}