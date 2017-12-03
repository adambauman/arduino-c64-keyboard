#include <Arduino.h>
#include <Keyboard.h>
#include "KeyMatrix.h"
#include "KeyDefines.h"

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
	{ KEYDEC_1,    KEYDEC_GRAV, KEYDEC_LCTR, KEYDEC_ESC,  KEYDEC_SPC,  KEYDEC_LALT, KEYDEC_Q,    KEYDEC_2    },
	{ KEYDEC_3,    KEYDEC_W,    KEYDEC_A,    KEYDEC_LSHI, KEYDEC_Z,    KEYDEC_S,    KEYDEC_E,    KEYDEC_4    },
	{ KEYDEC_7,    KEYDEC_Y,    KEYDEC_G,    KEYDEC_V,    KEYDEC_B,    KEYDEC_H,    KEYDEC_U,    KEYDEC_8    },
	{ KEYDEC_5,    KEYDEC_R,    KEYDEC_D,    KEYDEC_X,    KEYDEC_C,    KEYDEC_F,    KEYDEC_T,    KEYDEC_6    },
	{ KEYDEC_9,    KEYDEC_I,    KEYDEC_J,    KEYDEC_N,    KEYDEC_M,    KEYDEC_K,    KEYDEC_O,    KEYDEC_0	 },
	{ KEYDEC_PLUS, KEYDEC_P,    KEYDEC_L,    KEYDEC_COMA, KEYDEC_PERD, KEYDEC_COLN, KEYDEC_AT,   KEYDEC_MINU },
	{ KEYDEC_LPND, KEYDEC_ASTR, KEYDEC_SCOL, KEYDEC_FSLA, KEYDEC_RSHI, KEYDEC_EQAL, KEYDEC_CART, KEYDEC_HOM  },
	{ KEYDEC_DEL,  KEYDEC_ENT,  KEYDEC_RHT,  KEYDEC_DWN,  KEYDEC_F1,   KEYDEC_F3,   KEYDEC_F5,   KEYDEC_F7   }
};

uint8_t g_c64_keymap_shifted[8][8] = {
	{ KEYDEC_EXCL, KEYDEC_TILD, 0,           0,           0,           0,            KEYDEC_UC_Q, KEYDEC_DQUT },
	{ KEYDEC_HASH, KEYDEC_UC_W, KEYDEC_UC_A, 0,           KEYDEC_UC_Z, KEYDEC_UC_S,  KEYDEC_UC_E, KEYDEC_DOLR },
	{ KEYDEC_SQUT, KEYDEC_UC_Y, KEYDEC_UC_G, KEYDEC_UC_V, KEYDEC_UC_B, KEYDEC_UC_H,  KEYDEC_UC_U, KEYDEC_LPAR },
	{ KEYDEC_PRCT, KEYDEC_UC_R, KEYDEC_UC_D, KEYDEC_UC_X, KEYDEC_UC_C, KEYDEC_UC_F,  KEYDEC_UC_T, KEYDEC_AMPR },
	{ KEYDEC_RPAR, KEYDEC_UC_I, KEYDEC_UC_J, KEYDEC_UC_N, KEYDEC_UC_M, KEYDEC_UC_K,  KEYDEC_UC_O, 0			  },
	{ 0,           KEYDEC_UC_P, KEYDEC_UC_L, KEYDEC_LSTH, KEYDEC_GRTH, KEYDEC_LBRA,  0,           KEYDEC_UNDR },
	{ 0,           0,           KEYDEC_BSLA, KEYDEC_QUES, 0,           0,            0,           KEYDEC_END  },
	{ KEYDEC_INS,  0,           KEYDEC_BSLA, KEYDEC_UP,   KEYDEC_F2,   KEYDEC_F4,    KEYDEC_F6,   KEYDEC_F8   },
};

uint8_t g_c64_keymap_character_mode[8][8] = {
	{ 0,   0,   0,   0,   0,   0,   0,   195 },
	{ 0,   195, 218, 0,   192, 191, 0,   193 },
	{ 0,   238, 179, 0,   0,   0,   0,   223 },
	{ 0,   194, 0,   217, 0,   0,   0,   0   },
	{ 0,   220, 221, 0,   0,   0,   221, 0   },
	{ 177, 95,  222, 0,   0,   0,   0,   0   },
	{ 205, 196, 0,   0,   0,   0,   0,   227 },
	{ 0,   0,   0,   0,   0,   0,   0,   0   },
};

uint8_t g_c64_keymap_shift_character_mode[8][8] = {
	{ 0,   0,   0,   0,   0,   0,   7,   0 },
	{ 0,   9,   6,   0,   4,   3,   0,   0 },
	{ 0,   0,   0,   0,   0,   0,   0,   0 },
	{ 0,   0,   0,   5,   0,   0,   0,   0 },
	{ 0,   0,   0,   0,   0,   0,   0,   0 },
	{ 197, 0,   0,   0,   0,   0,   0,   0 },
	{ 0,   0,   0,   0,   0,   0,   0,   0 },
	{ 0,   0,   0,   0,   0,   0,   0,   0 },
};

//KeyMap C64KeyMap::GetC64KeyMap() {
//	KeyMap key_map;
//	key_map.column_count = 8;
//	key_map.row_count = 8;
//	key_map.restore_key = KEYDEC_BSP;
//	key_map.shift_lock_key = KEYDEC_CLOK;
//	key_map.left_shift_position_row = 1;
//	key_map.left_shift_position_column = 3;
//	key_map.right_shift_position_row = 6;
//	key_map.right_shift_position_column = 4;
//
//	this->SetUnmodifiedMap(key_map);
//	this->SetShiftedMap(key_map);
//};

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

KeyMatrix::KeyMatrix() {
	//NOTE: Position of shift keys in matrix
	//TODO: assign dynamically after implementing keymap files
	this->m_left_shift_key.position_row = 1;
	this->m_left_shift_key.position_column = 3;
	this->m_right_shift_key.position_row = 6;
	this->m_right_shift_key.position_column = 4;
}

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
				Keyboard.press(this->GetKeyCode(column, row)); 
			}
			else if (!g_active_matrix_current[row][column] && g_active_matrix_last[row][column]) {
				Keyboard.release(this->GetKeyCode(column, row)); 
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

bool KeyMatrix::IsShiftKeyActive() {
	bool is_shift_active = false;
	bool left_shift_active = g_active_matrix_current[this->m_left_shift_key.position_column][this->m_left_shift_key.position_row];
	bool right_shift_active = g_active_matrix_current[this->m_right_shift_key.position_column][this->m_right_shift_key.position_row];
	if (left_shift_active || right_shift_active) { is_shift_active = true; }
	return(is_shift_active);
}

uint8_t KeyMatrix::GetKeyCode(uint8_t &column, uint8_t &row) {
	uint8_t requested_key_code = 0;
	//NOTE: Column and row are flipped here otherwise wonky keycodes come out
	if (true == this->IsShiftKeyActive()) {
#ifdef _DEBUG
		Serial.println("Codeset: Shifted");
#endif
		requested_key_code = g_c64_keymap_shifted[column][row];
		if (0 == requested_key_code) { requested_key_code = g_c64_keymap_unmodified[column][row]; }
	}
	else {
#ifdef _DEBUG
		Serial.println("Codeset: Unmodified");
#endif
		requested_key_code = g_c64_keymap_unmodified[column][row];
	}

	return(requested_key_code);
}