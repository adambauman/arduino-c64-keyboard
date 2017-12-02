#include "Common.h"
#include "KeyDefines.h"
#include "C64KeyMaps.h"

uint8_t g_c64_matrix_unmodified[8][8] = {
	{ KEYDEC_1,    KEYDEC_GRAV, KEYDEC_LCTR, KEYDEC_ESC,  KEYDEC_SPC,  KEYDEC_LALT, KEYDEC_Q,    KEYDEC_2 },
	{ KEYDEC_3,    KEYDEC_W,    KEYDEC_A,    KEYDEC_LSHI, KEYDEC_Z,    KEYDEC_S,    KEYDEC_E,    KEYDEC_4 },
	{ KEYDEC_7,    KEYDEC_Y,    KEYDEC_G,    KEYDEC_V,    KEYDEC_B,    KEYDEC_H,    KEYDEC_U,    KEYDEC_8 },
	{ KEYDEC_5,    KEYDEC_R,    KEYDEC_D,    KEYDEC_X,    KEYDEC_C,    KEYDEC_F,    KEYDEC_T,    KEYDEC_6 },
	{ KEYDEC_9,    KEYDEC_I,    KEYDEC_J,    KEYDEC_N,    KEYDEC_M,    KEYDEC_K,    KEYDEC_O,    KEYDEC_0 },
	{ KEYDEC_PLUS, KEYDEC_P,    KEYDEC_L,    KEYDEC_COMA, KEYDEC_PRCT, KEYDEC_COLN, KEYDEC_AT,   KEYDEC_MINU },
	{ KEYDEC_DOLR, KEYDEC_ASTR, KEYDEC_SCOL, KEYDEC_FSLA, KEYDEC_RSHI, KEYDEC_EQAL, KEYDEC_CART, KEYDEC_TAB },
	{ KEYDEC_DEL,  KEYDEC_ENT,  KEYDEC_RHT,  KEYDEC_DWN,  KEYDEC_F1,   KEYDEC_F3,   KEYDEC_F5,   KEYDEC_F7 }
};

uint8_t g_c64_matrix_shifted[8][8] = {
	{ KEYDEC_EXCL, KEYDEC_TILD, 0,           0,           0,           0,           KEYDEC_UC_Q, KEYDEC_DQUT },
	{ KEYDEC_HASH, KEYDEC_UC_W, 0,           0,           0,           0,           0,           KEYDEC_DOLR },
	{ 37,          0,           0,           0,           0,           0,           0,           KEYDEC_AMPR },
	{ KEYDEC_SQUT, 0,           0,           0,           0,           KEYDEC_H,    0,           KEYDEC_LPAR },
	{ KEYDEC_RPAR, 0,           0,           0,           0,           0,           0,           KEYDEC_CART },
	{ 0,          0,           0,           60,          KEYDEC_LSTH, KEYDEC_LBRA,  0,           KEYDEC_UNDR },
	{ 0,          0,           KEYDEC_BSLA, KEYDEC_QUES, 0,           0,            KEYDEC_PDN,  KEYDEC_HOM }
};

KeyMap C64KeyMap::GetC64KeyMap() {
	KeyMap key_map;
	key_map.column_count = 8;
	key_map.row_count = 8;
	key_map.restore_key = KEYDEC_BSP;
	key_map.shift_lock_key = KEYDEC_CLOK;
	key_map.left_shift_position_row = 1;
	key_map.left_shift_position_column = 3;
	key_map.right_shift_position_row = 6;
	key_map.right_shift_position_column = 4;

	this->SetUnmodifiedMap(key_map);
	this->SetShiftedMap(key_map);
};

void C64KeyMap::SetUnmodifiedMap(IN KeyMap &key_map) {
	for (uint8_t row = 0; key_map.row_count > row; row++) {
		for (uint8_t column = 0; key_map.column_count > column;
			key_map.matrix_unmodified[row][column] = g_c64_matrix_unmodified[row][column], column++);
	}
}

void C64KeyMap::SetShiftedMap(IN KeyMap &key_map) {
	for (uint8_t row = 0; key_map.row_count > row; row++) {
		for (uint8_t column = 0; key_map.column_count > column;
			key_map.matrix_shifted[row][column] = g_c64_matrix_shifted[row][column], column++);
	}
}