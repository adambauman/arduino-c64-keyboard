#include "KeyMatrix.h"

KeyMatrix::KeyMatrix()
{
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

void KeyMatrix::ScanKeys(CD4051 &row_cd4051, CD4051 &column_cd4051)
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

	this->m_state_restore = !digitalRead(Pins::row_8); // true on LOW
	this->m_state_shift_lock = !digitalRead(Pins::shift_lock); // true on LOW

	if (this->m_last_state_restore != this->m_state_restore) { this->m_keys_have_changed = true; }
	if (this->m_last_state_shift_lock != this->m_state_shift_lock) { this->m_keys_have_changed = true; }
}