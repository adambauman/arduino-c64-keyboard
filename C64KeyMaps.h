#ifndef C64_KEY_MAPS_H
#define C64_KEY_MAPS_H
#include "KeyDefines.h"

struct KeyMaps {
	const uint8_t row_count = 8;
	const uint8_t column_count = 8;

	// Matrix of un-modified keycodes
	const uint8_t unmodified[8][8] = {
	{ KEYDEC_1,    KEYDEC_GRAV, KEYDEC_LCTR, KEYDEC_ESC,  KEYDEC_SPC,  KEYDEC_LALT, KEYDEC_Q,    KEYDEC_2 },
	{ KEYDEC_3,    KEYDEC_W,    KEYDEC_A,    KEYDEC_LSHI, KEYDEC_Z,    KEYDEC_S,    KEYDEC_E,    KEYDEC_4 },
	{ KEYDEC_7,    KEYDEC_Y,    KEYDEC_G,    KEYDEC_V,    KEYDEC_B,    KEYDEC_H,    KEYDEC_U,    KEYDEC_8 },
	{ KEYDEC_5,    KEYDEC_R,    KEYDEC_D,    KEYDEC_X,    KEYDEC_C,    KEYDEC_F,    KEYDEC_T,    KEYDEC_6 },
	{ KEYDEC_9,    KEYDEC_I,    KEYDEC_J,    KEYDEC_N,    KEYDEC_M,    KEYDEC_K,    KEYDEC_O,    KEYDEC_0 },
	{ KEYDEC_PLUS, KEYDEC_P,    KEYDEC_L,    KEYDEC_COMA, KEYDEC_PRCT, KEYDEC_COLN, KEYDEC_AT,   KEYDEC_MINU },
	{ KEYDEC_DOLR, KEYDEC_ASTR, KEYDEC_SCOL, KEYDEC_FSLA, KEYDEC_RSHI, KEYDEC_EQAL, KEYDEC_CART, KEYDEC_TAB },
	{ KEYDEC_DEL,  KEYDEC_ENT,  KEYDEC_RHT,  KEYDEC_DWN,  KEYDEC_F1,   KEYDEC_F3,   KEYDEC_F5,   KEYDEC_F7 }
	};

	// Matrix of SHIFT modified keycodes
	const uint8_t shifted[8][8] = {
	{ KEYDEC_EXCL, KEYDEC_TILD, 0,           0,           0,           0,           KEYDEC_UC_Q, KEYDEC_DQUT },
	{ KEYDEC_HASH, KEYDEC_UC_W, 0,           0,           0,           0,           0,           KEYDEC_DOLR },
	{ 37,          0,           0,           0,           0,           0,           0,           KEYDEC_AMPR },
	{ KEYDEC_SQUT, 0,           0,           0,           0,           KEYDEC_H,    0,           KEYDEC_LPAR },
	{ KEYDEC_RPAR, 0,           0,           0,           0,           0,           0,           KEYDEC_CART },
	{  0,          0,           0,           60,          KEYDEC_LSTH, KEYDEC_LBRA, 0,           KEYDEC_UNDR },
	{  0,          0,           KEYDEC_BSLA, KEYDEC_QUES, 0,           0,           KEYDEC_PDN,  KEYDEC_HOM}
	};

	const uint8_t restore_key = KEYDEC_BSP;

	const uint8_t left_shift_position_row = 1;
	const uint8_t left_shift_position_column = 3;
  
	const uint8_t right_shift_position_row = 6;
	const uint8_t right_shift_position_column = 4;
};

#endif