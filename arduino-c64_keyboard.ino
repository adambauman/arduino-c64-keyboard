// BauTek C64 PiCase Keyboard Controller
//

#include <Keypad.h>

void setup() {
const byte rows = 8;
const byte cols = 8;

char keys[rows][cols] = {
  {'1','leftArrow','ctrl','runstop','space','commodore','q','2'},
  {'3','w','a','leftShift','z','s','e','4'},
  {'5','r','d','x','c','f','t','6'},
  {'7','y','g','v','b','h','u','8'},
  {'9','i','j','n','m','k','o','0'},
  {'+','p','l',',','.',':','@','-'},
  {'$','*',';','/','rightShift','=','upArrow','home'},
  {'del','return','crsrHor','crsrVert','f1,'f3','f5,'f7'}
};

byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

}

void loop() {

}

