// BauTek C64 PiCase Keyboard Controller
//

//#include <Keyboard.h>

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

boolean keysHaveChanged = false;
boolean debugEnabled = true;

boolean shiftState = false;
boolean altState = false;
boolean ctrlState = false;
boolean capsLockState = false;

boolean historyRestore = false;
boolean historyShiftlock = false;
boolean statusRestore = false;
boolean statusShiftlock = false;

unsigned long startTime = 0;
unsigned int debounceTime = 10;

unsigned int keyActiveCount = 0;
  
// Direct ASCII values of keys to send
byte keyMapUnmodified[rows][columns] = {
  { 49,  96, 128, 177,  32, 130, 113,  50},
  { 51, 119,  97, 129, 122, 115, 101,  52},
  { 55, 121, 103, 118,  98, 104, 117,  56},
  { 53, 114, 100, 120,  99, 102, 116,  54},
  { 57, 105, 106, 110, 109, 107, 111,  48},
  { 43, 112, 108,  44,  46,  58,  64,  45},
  { 36,  42,  59,  47, 133,  61,  94, 179},
  {212, 176, 215, 217, 194, 196, 198, 200}
};

// ASCII values of keys when SHIFT is down
byte keyMapShifted[rows][columns] = {
  { 33, 126,   0,   0,   0,   0,   0,  34},
  { 35,   0,   0,   0,   0,   0,   0,  36},
  { 37,   0,   0,   0,   0,   0,   0,  38},
  { 39,   0,   0,   0,   0, 104,   0,  40},
  { 41,   0,   0,   0,   0,   0,   0,  94},
  {  0,   0,   0,  60,  62,  91,   0,  95},
  {  0,   0,  92,  63,   0,   0, 214, 210}
};
  
boolean keyMapStatus[rows][columns] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}
};

boolean keyMapHistory[rows][columns] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}
};

//
// setup()
//
void setup() {
  if (debugEnabled == true)
    Serial.begin(115200);

  pinMode(pin_colI, OUTPUT);
  pinMode(pin_row8, INPUT_PULLUP);
  
  pinMode(pin_colZ, OUTPUT);
  pinMode(pin_colA0, OUTPUT);
  pinMode(pin_colA1, OUTPUT);
  pinMode(pin_colA2, OUTPUT);
  
  pinMode(pin_rowZ, INPUT_PULLUP); 
  pinMode(pin_rowA0, OUTPUT);
  pinMode(pin_rowA1, OUTPUT);
  pinMode(pin_rowA2, OUTPUT);

  // Row drops low when button shorted to column. Drop columns LOW so they're ready.
  digitalWrite(pin_colZ, LOW);
  digitalWrite(pin_colI, LOW);

  Keyboard.begin();
  
} // End setup()

//
// loop()
//
void loop() {
  if  ((millis() - startTime) > debounceTime) {
      ScanKeys();
      startTime = millis();
  }

  // Fire off the keys if key status have changed
  if (keysHaveChanged == true)
    WriteKeys();

  // Mirror the status keymap into the history keymap
  for (byte c = 0; c < columns; c++) {  
    for (byte r = 0; r < rows; r++) {
      keyMapHistory[r][c] = keyMapStatus[r][c];
    }
  }

  // Mirror the status of the special keys
  historyRestore = statusRestore;

  if (debugEnabled == true && keysHaveChanged == true)
    DebugKeys();

  // Reset tracking variables, go for another loop
  keysHaveChanged = false;
    
} // End loop()


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

//
// ScanKeys()
// Scans the keyboard matrix
//
void ScanKeys() {
    // Make sure columns are running LOW
    digitalWrite(pin_colI, LOW);
    digitalWrite(pin_colZ, LOW);
      
    // Start working through the rows and columns
    for (byte c = 0; c < columns; c++) {  
      MuxColumn(c);
  
      for (byte r = 0; r < rows; r++) {
        MuxRow(r);

        // Row reads TRUE if low
        if (digitalRead(pin_rowZ) == LOW) {
          keyMapStatus[r][c] = true;
        } else {
          keyMapStatus[r][c] = false;
        }

        if (keyMapStatus[r][c] != keyMapHistory[r][c])
          keysHaveChanged = true;

      } // endfor ROWS
    } // endfor COLUMNS

    // Read RESTORE status
    if (digitalRead(pin_row8) == LOW) {
      statusRestore = true;
    } else {
      statusRestore = false;
    }

    if (historyRestore != statusRestore)
      keysHaveChanged = true;
}

//
// WriteKeys()
// Writes the keys out through USBHID
void WriteKeys() {
  // Start with the key maps
  for (byte c = 0; c < columns; c++){
    for (byte r = 0; r < rows; r++) {
      if (keyMapStatus[r][c] == true && keyMapHistory[r][c] == false) {
        Keyboard.press(keyMapUnmodified[c][r]);
      } else if (keyMapStatus[r][c] == false && keyMapHistory[r][c] == true) {
        Keyboard.release(keyMapUnmodified[c][r]);
      }
    } // endfor ROWS
  } // endfor COLUMNS

  // Process the RESTORE key
  if (statusRestore == true && historyRestore == false) {
    Keyboard.press(178);
  } else if (statusRestore == false && historyRestore == true) {
    Keyboard.release(178);
  }
}

//
// DebugKeys()
// Dumps debug information to the serial console
//
void DebugKeys() {
  int shouldSend = 0;
  
  Serial.println("");
  for (byte c = 0; c < columns; c++) {  
    for (byte r = 0; r < rows; r++) {
      Serial.print(keyMapHistory[r][c]);
      Serial.print(", ");

      if (keyMapHistory[r][c] == true)
        shouldSend = keyMapUnmodified[r][c]; 
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.println("KeymapUnmodified: ");

  for (byte c = 0; c < columns; c++) {
    for (byte r = 0; r < rows; r++) {
      Serial.print(keyMapUnmodified[r][c]);
      Serial.print(", ");
    }
    Serial.println("");
  }
  
  Serial.print("Should send: ");
  Serial.println(shouldSend);
  Serial.println("");
}
