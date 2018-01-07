#include <Keyboard.h>
#include <Joystick.h>

#define BUTTON_FLAG_TOP_LEFT      0x01
#define BUTTON_FLAG_TOP_RIGHT     0x02
#define BUTTON_FLAG_PADDLE_LEFT   0x04
#define BUTTON_FLAG_PADDLE_RIGTH  0x08
#define BUTTON_FLAG_JOY_LEFT      0x10
#define BUTTON_FLAG_JOY_UP        0x20
#define BUTTON_FLAG_JOY_RIGHT     0x40
#define BUTTON_FLAG_JOY_DOWN      0x80

// Alternative mode buttons
#define MODE_FLAG_0    BUTTON_FLAG_TOP_RIGHT
#define MODE_FLAG_1    BUTTON_FLAG_TOP_LEFT

// Button to pin Mapping 
#define PIN_TOP_LEFT      2
#define PIN_TOP_RIGHT     9
#define PIN_PADDLE_LEFT   3
#define PIN_PADDLE_RIGTH  4
#define PIN_JOY_LEFT      5
#define PIN_JOY_UP        6
#define PIN_JOY_RIGHT     7
#define PIN_JOY_DOWN      8

#define BUTTON_CNT        8
#define START_PIN         2


// Constant that maps the phyical pin to the joystick button.


// Create the Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_GAMEPAD, BUTTON_CNT, 0,
  false, false, false, false, false, false,
  false, false, false, false, false);


void setup() {
  // Initialize Button Pins
  for (int pin = START_PIN; pin < (START_PIN+BUTTON_CNT); pin++)
  {
    pinMode(pin, INPUT_PULLUP);
  }

  // Initialize Joystick Library
  Joystick.begin();
}

// Last state of the button
int lastButtonFlags = 0;
int lastButtonFlagsMom = 0;
int ModeFlags = 0;

void loop() {
  // Read pin values
  int currentButtonFlags = 0;
  for (int pin = START_PIN; pin < (START_PIN+BUTTON_CNT); pin++) {
    
    int val = !digitalRead(pin);
    if (val) {
      switch (pin) {
        case PIN_TOP_LEFT:
          currentButtonFlags |= BUTTON_FLAG_TOP_LEFT;
          break;
        case PIN_TOP_RIGHT:
          currentButtonFlags |= BUTTON_FLAG_TOP_RIGHT;
          break;
        case PIN_PADDLE_LEFT:
          currentButtonFlags |= BUTTON_FLAG_PADDLE_LEFT;
          break;
        case PIN_PADDLE_RIGTH:
          currentButtonFlags |= BUTTON_FLAG_PADDLE_RIGTH;
          break;
        case PIN_JOY_LEFT:
          currentButtonFlags |= BUTTON_FLAG_JOY_LEFT;
          break;
        case PIN_JOY_UP:
          currentButtonFlags |= BUTTON_FLAG_JOY_UP;
          break;
        case PIN_JOY_DOWN:
          currentButtonFlags |= BUTTON_FLAG_JOY_DOWN;
          break;
        case PIN_JOY_RIGHT:
          currentButtonFlags |= BUTTON_FLAG_JOY_RIGHT;
          break;
        default:
          break;
      }
    }
  }

  //Filter out very short pesses
  if (lastButtonFlagsMom != currentButtonFlags)
  {    
    lastButtonFlagsMom = currentButtonFlags;
    delay(5);
    return;
  }

  ModeFlags = currentButtonFlags & (MODE_FLAG_0 | MODE_FLAG_1);
    
  // Set joystick button values
  for (int bitPos = 0; bitPos < BUTTON_CNT; bitPos++) {    
    
    int flagMask = 0x01 << bitPos;
    int currentFlag = currentButtonFlags & flagMask;
    if (currentFlag != (lastButtonFlags & flagMask)) {

      if (ModeFlags & MODE_FLAG_0) {
        switch (currentFlag)
        {
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
          case BUTTON_FLAG_TOP_LEFT:
            SendKeyboardKeyCombo(KEY_LEFT_CTRL,' ');
            continue;
          default:
            break;
        }        
      }
      else if (ModeFlags & MODE_FLAG_1) {
        switch (currentFlag)
        {
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
      
      Joystick.setButton(bitPos, currentFlag);      
    }
  }

  if (lastButtonFlags == currentButtonFlags)
  {
    delay(5);
  }
  else
  {
    lastButtonFlags = currentButtonFlags;
    delay(50);
  }
}

void SendKeyboardKeyCombo(int key1, int key2)
{
  Keyboard.press(key1);
  delay(200);
  Keyboard.press(key2);
  delay(200);
  Keyboard.releaseAll();
}
