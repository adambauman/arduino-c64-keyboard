#include <Arduino.h>
#include <Keyboard.h>
#include "KeyMatrix.h"
#include "C64KeyMap.h"

//TODO: convert globals to member variables as possible
const uint8_t g_column_count = 8;
const uint8_t g_row_count = 8;
boolean g_keys_pending_write = false;
boolean g_shift_state = false;
boolean g_alt_state = false;
boolean g_ctrl_state = false;
boolean g_caps_lock_state = false;
boolean g_history_restore = false;
boolean g_history_shiftlock = false;
boolean g_status_restore = false;
boolean g_status_shiftlock = false;
boolean g_active_matrix_current[g_row_count][g_column_count] = {
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false }
};
boolean g_active_matrix_last[g_row_count][g_column_count] = {
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false }
};

C64KeyMap g_c64_key_map;

KeyMatrix::KeyMatrix() {}

void KeyMatrix::ProcessKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column)
{
	this->ScanKeyMatrix(cd4051_row, cd4051_column);
	if (g_keys_pending_write) { this->WriteKeys(); }
	this->UpdateActivityMatrix();
	g_keys_pending_write = false;
}

void KeyMatrix::ScanKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column)
{
	for (uint8_t column = 0; column < g_column_count; column++) {
		cd4051_column.Select(column);
		for (uint8_t row = 0; row < g_row_count; row++) {
			cd4051_row.Select(row);
			g_active_matrix_current[row][column] = false;
			//TODO: replace with PIN_CD4051_ROW_COMMON read common function
			if (!digitalRead(10)) { g_active_matrix_current[row][column] = true; } //Key active if LOW
			if (g_active_matrix_current[row][column] != g_active_matrix_last[row][column]) { g_keys_pending_write = true; }
		}
	}

	// Special handling for off-matrix restore and shiftlock keys
	g_status_restore = false;
	//TODO: replace with PIN_ROW_8 variable
	if (!digitalRead(1)) { g_status_restore = true; }
	g_status_shiftlock = false;
	//TODO: replace with PIN_SHIFT_LOCK variable
	if (!digitalRead(9)) { g_status_shiftlock = true; }

	if (g_history_restore != g_status_restore || g_history_shiftlock != g_status_shiftlock) { g_keys_pending_write = true; }
}

void KeyMatrix::WriteKeys() {
	uint8_t keymode = KEYMODE_NORMAL;
	if (this->IsShiftKeyActive()) { keymode = KEYMODE_SHIFT; }
	for (uint8_t column = 0; column < g_column_count; column++) {
		for (uint8_t row = 0; row < g_row_count; row++) {
			if (g_active_matrix_current[row][column] && !g_active_matrix_last[row][column]) {
				Keyboard.press(g_c64_key_map.GetKeyCode(column, row, keymode)); 
			}
			else if (!g_active_matrix_current[row][column] && g_active_matrix_last[row][column]) {
				Keyboard.release(g_c64_key_map.GetKeyCode(column, row, keymode)); 
			}
		}
	}

	// Special handling for off-matrix restore and shiftlock keys
	if (g_status_restore && !g_history_restore) {
		Keyboard.press(178);
	}
	else if (!g_status_restore && g_history_restore) {
		Keyboard.release(178);
	}

	if (g_status_shiftlock && !g_history_shiftlock) {
		Keyboard.write(193);
	}
	else if (!g_status_shiftlock && g_history_shiftlock) {
		Keyboard.release(193);
	}
}

void KeyMatrix::UpdateActivityMatrix() {
	for (uint8_t column = 0; column < g_column_count; column++) {
		for (uint8_t row = 0; row < g_row_count; g_active_matrix_last[row][column] = g_active_matrix_current[row][column], row++) {}
	}

	g_history_restore = g_status_restore;
	g_history_shiftlock = g_status_shiftlock;
}

void KeyMatrix::StartKeyboard() {
	Keyboard.begin();
}

bool KeyMatrix::IsShiftKeyActive() {
	bool is_shift_active = false;
	bool left_shift_active = g_active_matrix_current[g_c64_key_map.m_left_shift.column][g_c64_key_map.m_left_shift.row];
	bool right_shift_active = g_active_matrix_current[g_c64_key_map.m_right_shift.column][g_c64_key_map.m_right_shift.row];
	if (left_shift_active || right_shift_active) { is_shift_active = true; }
	return(is_shift_active);
}