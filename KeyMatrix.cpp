#include "KeyMatrix.h"
#include <Keyboard.h>

KeyMatrix::KeyMatrix(const bool is_usb_keyboard)
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

	for (uint8_t column = 0; KEY_MATRIX_COLUMNS > column; column++) {
		for (uint8_t row = 0; KEY_MATRIX_ROWS > row; row++,
			m_state_map[row][column] = false, m_last_state_map[row][column] = false) {}
	}
}

void KeyMatrix::ScanMatrix(CD4051 &row_cd4051, CD4051 &column_cd4051)
{
	this->m_keys_have_changed = false;
	for (uint8_t column = 0; KEY_MATRIX_COLUMNS > column; column++) {
		column_cd4051.Select(column);
		for (uint8_t row = 0; KEY_MATRIX_ROWS > row; row++) {
			row_cd4051.Select(row);
			this->m_state_map[row][column] = !row_cd4051.ReadCommonValue(); // true on LOW
			if (this->m_last_state_map[row][column] != this->m_state_map[row][column]) { this->m_keys_have_changed = true; }
		}
	}
}

void KeyMatrix::ScanSpecialKeys(CD4051 &row_cd4051, CD4051 &column_cd4051)
{
	this->m_state_restore = !digitalRead(Pins::row_8); // true on LOW
	this->m_state_shift_lock = !digitalRead(Pins::shift_lock); // true on LOW

	if (this->m_last_state_restore != this->m_state_restore) { this->m_keys_have_changed = true; }
	if (this->m_last_state_shift_lock != this->m_state_shift_lock) { this->m_keys_have_changed = true; }
}

void KeyMatrix::WriteMappedUSBKeys(const KeyMaps &key_maps)
{
	for (byte column = 0; KEY_MATRIX_COLUMNS > column; column++) {
		for (byte row = 0; KEY_MATRIX_ROWS > row; row++) {
			KeyToggleAction(row, column, key_maps);
		}
	}
}

void KeyMatrix::WriteC64SpecialUSBKeys(const KeyMaps &key_maps)
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

void KeyMatrix::KeyToggleAction(const uint8_t row, const uint8_t column, const KeyMaps &key_maps)
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

bool KeyMatrix::IsKeyboardShifted(const KeyMaps &key_maps)
{
	bool shift_pressed = false;
	bool left_shift_pressed = this->m_state_map[key_maps.left_shift_position_row][key_maps.left_shift_position_column];
	bool right_shift_pressed = this->m_state_map[key_maps.right_shift_position_row][key_maps.right_shift_position_column];
	if (left_shift_pressed || right_shift_pressed) { shift_pressed = true; }
	return(shift_pressed);
}
