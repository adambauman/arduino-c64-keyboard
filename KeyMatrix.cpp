#include <Arduino.h>
#include <Keyboard.h>
#include "KeyMatrix.h"

uint8_t g_column_count = 8;
uint8_t g_row_count = 8;

boolean g_active_matrix_current[8][8] = {
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false }
};

boolean g_active_matrix_last[8][8] = {
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false },
	{ false,false,false,false,false,false,false,false }
};

uint8_t g_c64_keymap_unmodified[8][8] = {
	{ 49,  96, 128, 177,  32, 130, 113,  50 },
	{ 51, 119,  97, 129, 122, 115, 101,  52 },
	{ 55, 121, 103, 118,  98, 104, 117,  56 },
	{ 53, 114, 100, 120,  99, 102, 116,  54 },
	{ 57, 105, 106, 110, 109, 107, 111,  48 },
	{ 43, 112, 108,  44,  46,  58,  64,  45 },
	{ 36,  42,  59,  47, 133,  61,  94, 179 },
	{ 212, 176, 215, 217, 194, 196, 198, 200 }
};

uint8_t g_c64_keymap_shifted[8][8] = {
	{ 33, 126,   0,   0,   0,   0,   0,  34 },
	{ 35,   0,   0,   0,   0,   0,   0,  36 },
	{ 37,   0,   0,   0,   0,   0,   0,  38 },
	{ 39,   0,   0,   0,   0, 104,   0,  40 },
	{ 41,   0,   0,   0,   0,   0,   0,  94 },
	{ 0,   0,   0,  60,  62,  91,   0,  95 },
	{ 0,   0,  92,  63,   0,   0, 214, 210 }
};

// Set if matrix is changed, stops loop from writing unnecessary key commands
boolean g_keys_pending_write = false;

// Tracks state of modifier keys (currently unused, running custom OS keymaps instead)
boolean g_shift_state = false;
boolean g_alt_state = false;
boolean g_ctrl_state = false;
boolean g_caps_lock_state = false;

// Tracks RESTORE and SHIFTLOCK parameters
boolean g_history_restore = false;
boolean g_history_shiftlock = false;
boolean g_status_restore = false;
boolean g_status_shiftlock = false;

KeyMatrix::KeyMatrix() {}

void KeyMatrix::ProcessKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column)
{
	this->ScanKeyMatrix(cd4051_row, cd4051_column);
	if (g_keys_pending_write) { this->WriteKeys(); }
	g_keys_pending_write = false;
	this->UpdateActivityMatrix();
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

	g_status_restore = false;
	//TODO: replace with PIN_ROW_8 variable
	if (!digitalRead(1)) { g_status_restore = true; }
	g_status_shiftlock = false;
	//TODO: replace with PIN_SHIFT_LOCK variable
	if (!digitalRead(9)) { g_status_shiftlock = true; }

	if (g_history_restore != g_status_restore || g_history_shiftlock != g_status_shiftlock) { g_keys_pending_write = true; }
}

void KeyMatrix::WriteKeys() {
	for (uint8_t column = 0; column < g_column_count; column++) {
		for (uint8_t row = 0; row < g_row_count; row++) {
			if (g_active_matrix_current[row][column] && !g_active_matrix_last[row][column]) {
				Keyboard.press(g_c64_keymap_unmodified[column][row]); // column and row need to reversed. Sure I'm derping on something here, but it works
			}
			else if (!g_active_matrix_current[row][column] && g_active_matrix_last[row][column]) {
				Keyboard.release(g_c64_keymap_unmodified[column][row]); // column and row need to reversed. Sure I'm derping on something here, but it works
			}
		}
	}

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
		Keyboard.write(193);
	}
}

void KeyMatrix::UpdateActivityMatrix() {
	for (uint8_t column = 0; column < g_column_count; column++) {
		for (uint8_t row = 0; row < g_row_count; g_active_matrix_last[row][column] = g_active_matrix_current[row][column], row++) {}
	}

	// Mirror the status of the special keys
	g_history_restore = g_status_restore;
}

void KeyMatrix::StartKeyboard() {
	Keyboard.begin();
}