#ifndef KEYMATRIX_H
#define KEYMATRIX_H

#include <Arduino.h>
#include <inttypes.h>

#ifndef CD4051_H
#include "CD4051.h"
#endif

#define KEY_MATRIX_ROWS 8
#define KEY_MATRIX_COLUMNS 8

class KeyMatrix {
    public:
        KeyMatrix();
        void ScanKeys(CD4051 &row_cd4051, CD4051 &column_cd4051);
	private:
		boolean m_keys_have_changed;
		boolean m_state_shift;
		boolean m_state_alt;
		boolean m_state_ctrl;
		boolean m_state_caps_lock;
		boolean m_last_state_restore;
		boolean m_last_state_shift_lock;
		boolean m_state_restore;
		boolean m_state_shift_lock;

		boolean m_state_map[KEY_MATRIX_ROWS][KEY_MATRIX_COLUMNS];
		boolean m_last_state_map[KEY_MATRIX_ROWS][KEY_MATRIX_COLUMNS];
};

#endif