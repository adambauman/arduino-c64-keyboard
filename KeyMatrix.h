#ifndef KEYMATRIX_H
#define KEYMATRIX_H

#include <Arduino.h>
#include <inttypes.h>

#ifndef CD4051_H
#include "CD4051.h"
#endif

class KeyMatrix {
    public:
        KeyMatrix(const bool is_usb_keyboard);
        void ScanMatrix(CD4051 &row_cd4051, CD4051 &column_cd4051);
		void ScanSpecialKeys(CD4051 &row_cd4051, CD4051 &column_cd4051);
		void WriteMappedUSBKeys(const KeyMaps &key_maps);
		void WriteC64SpecialUSBKeys(const KeyMaps &key_maps);
	private:
		void KeyToggleAction(const uint8_t row, const uint8_t column, const KeyMaps &key_maps);
		bool IsKeyboardShifted(const KeyMaps &key_maps);
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