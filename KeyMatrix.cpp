#include "Common.h"
#include "KeyMatrix.h"
//#include <Keyboard.h>

KeyMatrix::KeyMatrix(IN const bool is_usb_keyboard)
{
	if (is_usb_keyboard) { Keyboard.begin(); }

	this->m_keys_have_changed = false;
	this->m_state_shift = false;
	this->m_state_alt = false;
	this->m_state_ctrl = false;
	this->m_state_caps_lock = false;
	this->m_last_state_restore = false;
	this->m_last_state_shift_lock = false;
	this->m_state_restore = false;
	this->m_state_shift_lock = false;

	//TODO: (Adam) Gotta be a way to make status matrix dynamic and track better
	for (uint8_t column = 0; COLUMN_COUNT > column; column++) {
		for (uint8_t row = 0; ROW_COUNT > row; row++,
			m_state_map[row][column] = false, m_last_state_map[row][column] = false) {}
	}
}

void KeyMatrix::ProcessKeyboardMatrix(
	IN CD4051 &row_cd4051,
	IN CD4051 &column_cd4051,
	IN const uint8_t &pin_row_8,
	IN const uint8_t &pin_shift_lock,
	IN const KeyMaps &key_maps,
	IN const bool &debug_enabled)
{
	//TODO: (Adam) make keys_have_changed a result response from scanning instead of a member
	this->m_keys_have_changed = false;
	ScanMatrix(row_cd4051, column_cd4051, key_maps);
	ScanSpecialKeys(pin_row_8, pin_shift_lock);

	//TODO: (Adam) obliterate debug_enabled
#ifdef _DEBUG
	if (this->m_keys_have_changed) { DebugKeyboardMatrix(key_maps); }
#endif

	if (m_keys_have_changed) {
		WriteMappedUSBKeys(key_maps);
		WriteC64SpecialUSBKeys(key_maps);
	}

	UpdateLastStates();
}

void KeyMatrix::ScanMatrix(IN CD4051 &row_cd4051, IN CD4051 &column_cd4051, IN const KeyMaps &key_maps)
{
	this->m_keys_have_changed = false;
	for (uint8_t column = 0; key_maps.column_count > column; column++) {
#ifdef _DEBUG
		Serial.print("Column: "); Serial.println(column);
#endif
		column_cd4051.Select(column);
		for (uint8_t row = 0; key_maps.row_count > row; row++) {
			row_cd4051.Select(row);
			this->m_state_map[row][column] = !row_cd4051.ReadCommonValue(); // true on LOW
#ifdef _DEBUG
			Serial.print("Row: "); Serial.println(row);
			Serial.print("CommonValue: "); Serial.println(row_cd4051.ReadCommonValue());
			if (this->m_state_map[row][column]) { Serial.print("ScanMatrix() detects change in "); Serial.print(row); Serial.print(":"), Serial.println(column); }
#endif
			if (this->m_last_state_map[row][column] != this->m_state_map[row][column]) { this->m_keys_have_changed = true; }
		}
	}
}

void KeyMatrix::ScanSpecialKeys(IN const uint8_t &pin_row_8, IN const uint8_t &pin_shift_lock)
{
	//TODO: (Adam) Make pin referencing safer/more dynamic
	this->m_state_restore = !digitalRead(pin_row_8); // true on LOW
	this->m_state_shift_lock = !digitalRead(pin_shift_lock); // true on LOW

	if (this->m_last_state_restore != this->m_state_restore) { this->m_keys_have_changed = true; }
	if (this->m_last_state_shift_lock != this->m_state_shift_lock) { this->m_keys_have_changed = true; }
}

void KeyMatrix::WriteMappedUSBKeys(IN const KeyMaps &key_maps)
{
	for (byte column = 0; key_maps.column_count > column; column++) {
		for (byte row = 0; key_maps.row_count > row; row++) {
			KeyToggleAction(row, column, key_maps);
		}
	}
}

void KeyMatrix::WriteC64SpecialUSBKeys(IN const KeyMaps &key_maps)
{
	if (this->m_state_restore && !this->m_last_state_restore) {
		Keyboard.press(key_maps.restore_key);
	}
	else if (this->m_state_restore && this->m_last_state_restore) {
		Keyboard.release(key_maps.restore_key);
	}

	// Shift Lock runs Caps Lock, USB output is a bit different
	if (this->m_state_shift_lock && !m_last_state_shift_lock) {
		Keyboard.write(KEYDEC_CLOK);
	}
	else if (!this->m_state_shift_lock && this->m_last_state_shift_lock) {
		Keyboard.write(KEYDEC_CLOK);
	}
}

void KeyMatrix::KeyToggleAction(IN const uint8_t row, IN const uint8_t column, IN const KeyMaps &key_maps)
{
	if (this->m_state_map[row][column] && !this->m_last_state_map[row][column]) {
		if (IsKeyboardShifted(key_maps)) {
			//TODO: figure out why row and column need to be reversed here
			Keyboard.press(key_maps.shifted[column][row]);
		}
		else {
			Keyboard.press(key_maps.unmodified[column][row]);
		}
	}
	else if (this->m_state_map[row][column] && this->m_last_state_map[row][column]) {
		if (IsKeyboardShifted(key_maps)) {
			//TODO: figure out why row and column need to be reversed here
			Keyboard.release(key_maps.shifted[column][row]);
		}
		else {
			Keyboard.release(key_maps.unmodified[column][row]);
		}
	}
}

bool KeyMatrix::IsKeyboardShifted(IN const KeyMaps &key_maps)
{
	bool shift_pressed = false;
	bool left_shift_pressed = this->m_state_map[key_maps.left_shift_position_row][key_maps.left_shift_position_column];
	bool right_shift_pressed = this->m_state_map[key_maps.right_shift_position_row][key_maps.right_shift_position_column];
	if (left_shift_pressed || right_shift_pressed) { shift_pressed = true; }
	return(shift_pressed);
}

void KeyMatrix::UpdateLastStates()
{
	this->m_last_state_restore = this->m_state_restore;
	//TODO: (Adam) dynamic row/column size for different maps
	for (byte column = 0; COLUMN_COUNT < column; column++) {
		for (byte row = 0; ROW_COUNT > row; row++, this->m_last_state_map[row][column] = this->m_state_map[row][column]) {}
	}
}

void KeyMatrix::DebugKeyboardMatrix(IN const KeyMaps &key_maps)
{
	  uint8_t selected_key = 0;
	  
	  Serial.println("");
	  for (uint8_t column = 0; COLUMN_COUNT > column; column++) {  
	    for (uint8_t row = 0; ROW_COUNT > row; row++) {
	      Serial.print(this->m_last_state_map[row][column]);
	      Serial.print(", ");
	
	      if (this->m_last_state_map[row][column])
	        selected_key = key_maps.unmodified[row][column]; 
	    }
	    Serial.println("");
	  }
	
	  Serial.println("");
	  Serial.println("KeymapUnmodified: ");
	
	  for (byte column = 0; COLUMN_COUNT > column; column++) {
	    for (byte row = 0; ROW_COUNT > row; row++) {
	      Serial.print(key_maps.unmodified[row][column]);
	      Serial.print(", ");
	    }
	    Serial.println("");
	  }
	  
	  Serial.print("Should send: ");
	  Serial.println(selected_key);
	  Serial.println("");
}