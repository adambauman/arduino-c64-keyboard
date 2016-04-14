// BauTek C64 PiCase Keyboard Controller
//

void setup() {
const byte rows = 8;
const byte cols = 8;

boolean shiftPressed = false;
boolean altPressed = false;
boolean ctrlPressed = false;
boolean capsLockState = false;

// Set to direct ASCII values of keys to send
byte key_map_unmodified[rows][cols] = {
  {49, 96, 128, 177, 32, 130, 113, 50},
  {51, 119, 97, 129, 122, 115, 101, 52},
  {53, 114, 100, 120, 99, 102, 116, 54},
  {55, 121, 103, 118, 98, 104, 117, 56},
  {57, 105, 106, 110, 109, 107, 111, 48},
  {43, 112, 108, 44, 46, 58, 64, 45},
  {36, 42, 59, 47, 133, 61, 94, 179},
  {212, 176, 215, 217, 194, 196, 198, 200}
};

// Restore (I8) = 178 (backspace)

boolean key_status[rows][cols] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
};


}

void loop() {

}

