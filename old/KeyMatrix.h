#ifndef KEYMATRIX_H
#define KEYMATRIX_H
#include "CodeLabels.h"

#define KEY_MATRIX_ROW_COUNT	8
#define KEY_MATRIX_COLUMN_COUNT	8

class KeyMatrix {
    public:
        KeyMatrix(IN const bool is_usb_keyboard);
		void ProcessKeyboardMatrix(
			IN CD4051 &row_cd4051, 
			IN CD4051 &column_cd4051,
			IN const uint8_t &pin_row_8,
			IN const uint8_t &pin_shift_lock,
			IN const KeyMap &key_map,
			IN const bool &debug_enabled
		);
		void DebugKeyboardMatrix(IN const KeyMap &key_map);
	private:
		void ScanMatrix(IN CD4051 &row_cd4051, IN CD4051 &column_cd4051, IN const KeyMap &key_map);
		void ScanSpecialKeys(IN const uint8_t &pin_row_8, IN const uint8_t &pin_shift_lock);
		void WriteMappedUSBKeys(IN const KeyMap &key_map);
		void WriteC64SpecialUSBKeys(IN const KeyMap &key_map);
		void KeyToggleAction(IN const uint8_t row, IN const uint8_t column, IN const KeyMap &key_map);
		bool IsKeyboardShifted(IN const KeyMap &key_map);
		void UpdateLastStates();
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

		//TODO: (Adam) Figure out a way to make these dynamic or track better
		boolean m_state_map[KEY_MATRIX_ROW_COUNT][KEY_MATRIX_COLUMN_COUNT];
		boolean m_last_state_map[KEY_MATRIX_ROW_COUNT][KEY_MATRIX_COLUMN_COUNT];
};

#endif