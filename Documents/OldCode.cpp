// BauTek C64 PiCase Keyboard Controller
//
// Utilizes an ATmega32U4-based microcontroller and two CD4051 muxers to run a Commodore64
// keyboard as a USBHID device. 
//
// See included wiring diagrams for hardware setup. The C64 keyboard requires one minor modification, you must
// de-solder the leads on the SHIFTLOCK key and run one to the microcontroller ground, the other to pin 9.
//
#include "key_maps.h"
#include "key_states.h"
#include "led_control.h"
#include<Keyboard.h>

// RESTORE lives outside the muxed matrix, sends BACKSPACE
#define pin_row8 1
#define pin_colI 2

// Modified SHIFTLOCK (originally closes SHIFT key in hardware, now runs CAPSLOCK)
#define pin_shiftLock 9

// Pins connected to the column CD4051
#define pin_colZ 21
#define pin_colA0 18
#define pin_colA1 19
#define pin_colA2 20

// Pins connected to the row CD4051
#define pin_rowZ 10
#define pin_rowA0 14
#define pin_rowA1 15
#define pin_rowA2 16

// RGB LED pins (optional, set RGBenabled = TRUE to enable custom LED function)
#define pin_rgbR 3
#define pin_rgbG 5
#define pin_rgbB 6

// Battery level check and sensor pins (optional, set batteryEnabled = TRUE to enable function)
#define battery_button 7
#define battery_sensor 8

KeyStates keyStates;
KeyMaps keymaps;
LEDControl ledControl(pin_rgbR, pin_rgbG, pin_rgbB);

// SETUP PARAMETERS
// Set TRUE to enable serial debugging
boolean debugEnabled = true;

// Set TRUE to enable the RGB LED
boolean RGBenabled = true;

// Set TRUE to enable battery level reading
boolean batteryEnabled = true;
// END SETUP PARAMETERS


// Size of the keyboard matrix. C64 is technically 9x9 but the RESTORE key lives on its own and is handled seperatly
const byte rows = 8;
const byte columns = 8;

// Set if matrix is changed, stops loop from writing unnecessary key commands
boolean keysHaveChanged = false;

// Status of special keyboard mapping modes
// 0 = normal, 1 = wasd navigation, 2 = c64 symbol writer
int keymapMode = 0;

// Debounce setup, on an ATmega32U4 @ 16MHz 10ms works pretty well
unsigned long startTime = 0;
unsigned int debounceTime = 10;
  
void LEDCycle(int cycleCount);



//
// setup()
//
void setup() {
  if (debugEnabled == true)
    Serial.begin(115200);

  LEDCycle(3);

  // All key reading pins use the internal pullup resistors
  pinMode(pin_shiftLock, INPUT_PULLUP);

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

  // Row drops low when button closed to column. Drop columns LOW so they're ready.
  digitalWrite(pin_colZ, LOW);
  digitalWrite(pin_colI, LOW);

  // Check Shift Lock button to see if it was left on when powering up
  if (digitalRead(pin_shiftLock) == LOW)
    keyStates.shiftlock_active = true;  

  Keyboard.begin(); 
  
} // End setup()


//
// loop()
//
void loop() {
  // Simple debounce routine
  if  ((millis() - startTime) > debounceTime) {
      ScanKeys();
      startTime = millis();
  }

  // Fire off the keys if key status have changed
  if (keysHaveChanged == true)
    WriteKeys();

  // Mirror the status key map into the history key map
  for (byte c = 0; c < columns; c++) {  
    for (byte r = 0; r < rows; r++) {
      keyStates.key_state_history[r][c] = keyStates.key_state[r][c];
    }
  }

  // Mirror the status of the special keys
  keyStates.restore_state_history = keyStates.restore_state;
  keyStates.shiftlock_state_history = keyStates.shiftlock_state;

  if (debugEnabled == true && keysHaveChanged == true)
    DebugKeys();

  // Reset tracking variables, go for another loop
  keysHaveChanged = false;
    
} // End loop()


//
// MuxColumn(int col)
// Sets the column CD4051 selector
//
// int col = column number to select
//
void MuxColumn(int col) {
  digitalWrite(pin_colA0, bitRead(col, 0));
  digitalWrite(pin_colA1, bitRead(col, 1));
  digitalWrite(pin_colA2, bitRead(col, 2));
}


//
// MuxRow(int row)
// Sets the row CD4051 selector
//
// int row = row number to select
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
    //digitalWrite(pin_colI, LOW);
    //digitalWrite(pin_colZ, LOW);
      
    // Start working through the rows and columns and fill the status matrix
    for (byte c = 0; c < columns; c++) {  
      MuxColumn(c);
  
      for (byte r = 0; r < rows; r++) {
        MuxRow(r);

        // Row reads TRUE if low
        if (digitalRead(pin_rowZ) == LOW) {
          keyStates.key_state[r][c] = true;
        } else {
          keyStates.key_state[r][c] = false;
        }

        // Compare the current key matrix to the matrix on the last loop, determine if keys have changed
        if (keyStates.key_state[r][c] != keyStates.key_state_history[r][c])
          keysHaveChanged = true;

      } // endfor ROWS
    } // endfor COLUMNS

    // Read RESTORE status
    if (digitalRead(pin_row8) == LOW) {
      keyStates.restore_state = true;
    } else {
      keyStates.restore_state = false;
    }

    // Read SHIFTLOCK status
    if (digitalRead(pin_shiftLock) == LOW) {
      keyStates.shiftlock_state = true;
    } else {
      keyStates.shiftlock_state = false;
    }

    // Compare RESTORE and SHIFTLOCK status, determine if keys have changed.
    if (keyStates.restore_state_history != keyStates.restore_state) {
      keysHaveChanged = true; 
    }

    if (keyStates.shiftlock_state_history != keyStates.shiftlock_state) {
      keysHaveChanged = true;
    }
}

//
// WriteKeys()
// Writes the keys out through USBHID
//
void WriteKeys() {
  // Start with the key maps
  for (byte c = 0; c < columns; c++){
    for (byte r = 0; r < rows; r++) {
      if (keyStates.key_state[r][c] == true && keyStates.key_state_history[r][c] == false) {
        // column and row need to reversed?
        Keyboard.press(keymaps.unmodified[c][r]);
      } else if (keyStates.key_state[r][c] == false && keyStates.key_state_history[r][c] == true) {
        // column and row need to reversed?
        Keyboard.release(keymaps.unmodified[c][r]);
      }
    } // endfor ROWS
  } // endfor COLUMNS

  // Process the RESTORE key
  if (keyStates.restore_state == true && keyStates.restore_state_history == false) {
    Keyboard.press(178);
  } else if (keyStates.restore_state == false && keyStates.restore_state_history == true) {
    Keyboard.release(178);
  }

  // Process the SHIFTLOCK key, use .write() since the key physically locks in place
  if (keyStates.shiftlock_state == true && keyStates.shiftlock_state_history == false && keyStates.shiftlock_active == false) {
    Keyboard.write(193);
    keyStates.shiftlock_active = true;
  } else if (keyStates.shiftlock_state == false && keyStates.shiftlock_state_history == true && keyStates.shiftlock_active == true) {
    Keyboard.write(193);
    keyStates.shiftlock_active = false;
  }
}


// 
// SetLED(String requestedStatus)
// Sets the color the RGB LED (if enabled)
//
// int requestedStatus = name of the status we want to indicate
//
void SetLED(int requestedStatus) {
  // Only run if RGB LED is enabled in the setup parameters
  if (RGBenabled) {
    switch (requestedStatus) {
      case 0: // Off
        analogWrite(pin_rgbR, 0);
        analogWrite(pin_rgbG, 0);
        analogWrite(pin_rgbB, 0);
        break;
      case 1: // Normal (red)
        analogWrite(pin_rgbR, 20);
        analogWrite(pin_rgbG, 0);
        analogWrite(pin_rgbB, 0);
        break;
      case 2: // Capslock enabled (blue)
        analogWrite(pin_rgbR, 0);
        analogWrite(pin_rgbG, 0);
        analogWrite(pin_rgbB, 20);
        break;
      case 3: // Battery good (green)
        analogWrite(pin_rgbR, 0);
        analogWrite(pin_rgbG, 20);
        analogWrite(pin_rgbB, 0);
        break;
      case 4: // Battery warning (yellow)
        analogWrite(pin_rgbR, 10);
        analogWrite(pin_rgbG, 10);
        analogWrite(pin_rgbB, 0);
        break;
      case 5: // Battery danger (orange-red)
        analogWrite(pin_rgbR, 10);
        analogWrite(pin_rgbG, 1);
        analogWrite(pin_rgbB, 1);
        break;
      default:
        break;
    }
  }
}


//
// DebugKeys()
// Dumps debug information to the serial console
//
void DebugKeys() {
  int shouldSend = 0;

  Serial.println("======STARTEVENT=======");
  
  Serial.println("");
  for (byte c = 0; c < columns; c++) {  
    for (byte r = 0; r < rows; r++) {
      Serial.print(keyStates.key_state_history[r][c]);
      Serial.print(", ");

      if (keyStates.key_state_history[r][c] == true)
        shouldSend = keymaps.unmodified[r][c]; 
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.println("KeymapUnmodified: ");

  for (byte c = 0; c < columns; c++) {
    for (byte r = 0; r < rows; r++) {
      Serial.print(keymaps.unmodified[r][c]);
      Serial.print(", ");
    }
    Serial.println("");
  }

  Serial.println("");
  Serial.print("Should send: ");
  Serial.println(shouldSend);
  Serial.println("");

  Serial.print("shiftlockEnabled: ");
  Serial.println(keyStates.shiftlock_active);
  Serial.print("statusShiftlock: ");
  Serial.println(keyStates.shiftlock_state);
  Serial.print("historyShiftlock: ");
  Serial.println(keyStates.shiftlock_state_history);

  Serial.println("=======ENDEVENT========");
}

//
// LEDCycle()
// Cycles through the C64 logo colors, then settles on past state.
//
void LEDCycle(int cycleCount = 1) {
  int cycleTimer = 150;
  int cycleSequence[] = {1, 5, 4 ,3, 2};

  int a, b;
  for (a=0; a < cycleCount; a++) {
    for (b=0; b < 5; b++) {
      SetLED(cycleSequence[b]);
      delay(cycleTimer);
    }
  }

  SetLED(0);
  delay(300);
  SetLED(1);
}


//
// LEDCycle()
// Cycles through the C64 logo colors, then settles on past state.
//
void LEDCycle(int cycleCount = 1) {
  int cycleTimer = 150;
  int cycleSequence[] = {1, 5, 4 ,3, 2};

  int a, b;
  for (a=0; a < cycleCount; a++) {
    for (b=0; b < 5; b++) {
      SetLED(cycleSequence[b]);
      delay(cycleTimer);
    }
  }

  SetLED(0);
  delay(300);
  SetLED(1);
}