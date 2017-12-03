#include <Arduino.h>
#include "KeyDefines.h"
#include "C64KeyMap.h"

uint8_t g_c64_keymap_unmodified[8][8] = {
	{ KEYDEC_1,    KEYDEC_GRAV, KEYDEC_LCTR, KEYDEC_ESC,  KEYDEC_SPC,  KEYDEC_LALT, KEYDEC_Q,    KEYDEC_2 },
	{ KEYDEC_3,    KEYDEC_W,    KEYDEC_A,    KEYDEC_LSHI, KEYDEC_Z,    KEYDEC_S,    KEYDEC_E,    KEYDEC_4 },
	{ KEYDEC_7,    KEYDEC_Y,    KEYDEC_G,    KEYDEC_V,    KEYDEC_B,    KEYDEC_H,    KEYDEC_U,    KEYDEC_8 },
	{ KEYDEC_5,    KEYDEC_R,    KEYDEC_D,    KEYDEC_X,    KEYDEC_C,    KEYDEC_F,    KEYDEC_T,    KEYDEC_6 },
	{ KEYDEC_9,    KEYDEC_I,    KEYDEC_J,    KEYDEC_N,    KEYDEC_M,    KEYDEC_K,    KEYDEC_O,    KEYDEC_0 },
	{ KEYDEC_PLUS, KEYDEC_P,    KEYDEC_L,    KEYDEC_COMA, KEYDEC_PERD, KEYDEC_COLN, KEYDEC_AT,   KEYDEC_MINU },
	{ KEYDEC_LPND, KEYDEC_ASTR, KEYDEC_SCOL, KEYDEC_FSLA, KEYDEC_RSHI, KEYDEC_EQAL, KEYDEC_CART, KEYDEC_HOM },
	{ KEYDEC_DEL,  KEYDEC_ENT,  KEYDEC_RHT,  KEYDEC_DWN,  KEYDEC_F1,   KEYDEC_F3,   KEYDEC_F5,   KEYDEC_F7 }
};

uint8_t g_c64_keymap_shifted[8][8] = {
	{ KEYDEC_EXCL, KEYDEC_TILD, 0,           0,           0,           0,            KEYDEC_UC_Q, KEYDEC_DQUT },
	{ KEYDEC_HASH, KEYDEC_UC_W, KEYDEC_UC_A, 0,           KEYDEC_UC_Z, KEYDEC_UC_S,  KEYDEC_UC_E, KEYDEC_DOLR },
	{ KEYDEC_SQUT, KEYDEC_UC_Y, KEYDEC_UC_G, KEYDEC_UC_V, KEYDEC_UC_B, KEYDEC_UC_H,  KEYDEC_UC_U, KEYDEC_LPAR },
	{ KEYDEC_PRCT, KEYDEC_UC_R, KEYDEC_UC_D, KEYDEC_UC_X, KEYDEC_UC_C, KEYDEC_UC_F,  KEYDEC_UC_T, KEYDEC_AMPR },
	{ KEYDEC_RPAR, KEYDEC_UC_I, KEYDEC_UC_J, KEYDEC_UC_N, KEYDEC_UC_M, KEYDEC_UC_K,  KEYDEC_UC_O, 0 },
	{ 0,           KEYDEC_UC_P, KEYDEC_UC_L, KEYDEC_LSTH, KEYDEC_GRTH, KEYDEC_LBRA,  0,           KEYDEC_UNDR },
	{ 0,           0,           KEYDEC_BSLA, KEYDEC_QUES, 0,           0,            0,           KEYDEC_END },
	{ KEYDEC_INS,  0,           KEYDEC_BSLA, KEYDEC_UP,   KEYDEC_F2,   KEYDEC_F4,    KEYDEC_F6,   KEYDEC_F8 },
};

//TODO: Flesh out character modes
uint8_t g_c64_keymap_character_mode[8][8] = {
	{ 0,   0,   0,   0,   0,   0,   0,   195 },
	{ 0,   195, 218, 0,   192, 191, 0,   193 },
	{ 0,   238, 179, 0,   0,   0,   0,   223 },
	{ 0,   194, 0,   217, 0,   0,   0,   0 },
	{ 0,   220, 221, 0,   0,   0,   221, 0 },
	{ 177, 95,  222, 0,   0,   0,   0,   0 },
	{ 205, 196, 0,   0,   0,   0,   0,   227 },
	{ 0,   0,   0,   0,   0,   0,   0,   0 },
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

C64KeyMap::C64KeyMap() {
	this->m_left_shift.row = 1;
	this->m_left_shift.column = 3;

	this->m_right_shift.row = 6;
	this->m_right_shift.column = 4;
}

uint8_t C64KeyMap::GetKeyCode(uint8_t &column, uint8_t &row, uint8_t keymode)
{
	uint8_t requested_key_code = 0;
#ifdef _DEBUG
	Serial.print("Codeset: "); Serial.println(keymode);
#endif
	//NOTE: Column and row are flipped here otherwise wonky keycodes come out
	switch (keymode) {
	case(KEYMODE_NORMAL):
		requested_key_code = g_c64_keymap_unmodified[column][row];
		break;
	case(KEYMODE_SHIFT):
		requested_key_code = g_c64_keymap_shifted[column][row];
		if (0 == requested_key_code) { requested_key_code = g_c64_keymap_unmodified[column][row]; }
	}

#ifdef _DEBUG
	Serial.print("Keycode Found: "); Serial.println(requested_key_code);
#endif
	return(requested_key_code);
}
