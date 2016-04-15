// BauTek C64 PiCase Keyboard Controller
//

// Restore (I8) = 178 (backspace)
#define pin_row8 1
#define pin_colI 2
#define pin_shiftLock 9

#define pin_colZ 21
#define pin_colA0 18
#define pin_colA1 19
#define pin_colA2 20

#define pin_rowZ 10
#define pin_rowA0 14
#define pin_rowA1 15
#define pin_rowA2 16

#define pin_rgbR 3
#define pin_rgbG 5
#define pin_rgbB 6

#define battery_button 7
#define battery_sensor 8

const byte rows = 8;
const byte columns = 8;

boolean keysModified = false;
boolean debugEnabled = true;

//
// setup()
//
void setup() {
  if (debugEnabled == true)
    Serial.begin(115200);
  
  pinMode(pin_colZ, OUTPUT);
  pinMode(pin_colA0, OUTPUT);
  pinMode(pin_colA1, OUTPUT);
  pinMode(pin_colA2, OUTPUT);
  
  pinMode(pin_colZ, INPUT); 
  pinMode(pin_rowA0, OUTPUT);
  pinMode(pin_rowA1, OUTPUT);
  pinMode(pin_rowA2, OUTPUT);
  
  digitalWrite(pin_colZ, LOW);
  
  boolean shiftState = false;
  boolean altState = false;
  boolean ctrlState = false;
  boolean capsLockState = false;
  
  // Direct ASCII values of keys to send
  byte keyMapUnmodified[rows][columns] = {
    {49, 96, 128, 177, 32, 130, 113, 50},
    {51, 119, 97, 129, 122, 115, 101, 52},
    {53, 114, 100, 120, 99, 102, 116, 54},
    {55, 121, 103, 118, 98, 104, 117, 56},
    {57, 105, 106, 110, 109, 107, 111, 48},
    {43, 112, 108, 44, 46, 58, 64, 45},
    {36, 42, 59, 47, 133, 61, 94, 179},
    {212, 176, 215, 217, 194, 196, 198, 200}
  };
  
  boolean keyMapStatus[rows][columns] = {
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
    {false,false,false,false,false,false,false,false},
  };


} // End setup()

//
// loop()
//
void loop() {
  ScanKeys();

  if (debugEnabled == true)
    DebugKeys();
    
} // End loop()

//
// ScanKeys()
// Scans the keyboard matrix
//
void ScanKeys() {
    for (byte c = 0; c < columns; c++) {
    digitalWrite(pin_colZ, HIGH);
    MuxColumn(c);

    for (byte r = 0; r < rows; r++) {
      MuxRow(r);
      if (digitalRead(pin_rowZ) == HIGH) {
        keyMapStatus[r][c] = true;
      } else {
        keyMapStatus[r][c] = false;
      }
    }
  }
}

//
// DebugKeys()
// Dumps debug information to the serial console
//
DebugKeys() {
  for (byte c = 0; c < columns; c++) {
    Serial.print("Column: ");
    Serial.println(c);
    
    for (byte r = 0; r < columns; r++) {
      if (key_status[r][c] == true) {
        Serial.print("Row: ");
        Serial.println(r);
        Serial.println(keyMapUnmodified[r][c]);
      }
    }
}

//
// MuxColumn()
// Sets the column 4051 selector
//
void MuxColumn(int col) {
  digitalWrite(pin_colA0, bitRead(col, 0));
  digitalWrite(pin_colA1, bitRead(col, 1));
  digitalWrite(pin_colA2, bitRead(col, 2));
}

//
// MuxRow()
// Sets the row 4051 selector
//
void MuxRow(int row) {
  digitalWrite(pin_rowA0, bitRead(row, 0));
  digitalWrite(pin_rowA1, bitRead(row, 1));
  digitalWrite(pin_rowA2, bitRead(row, 2));
}

