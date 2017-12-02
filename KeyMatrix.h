#ifndef KEYMATRIX_H
#define KEYMATRIX_H
#include "CodeLabels.h"
//#include <inttypes.h>

#ifndef CD4051_H
#include "CD4051.h"
#endif

//TODO: (Adam) Do we need to include key maps, or is just dummy struct enough?
#ifndef C64_KEY_MAPS_H
#include "C64KeyMaps.h"
#endif


class KeyMatrix {
    public:
        KeyMatrix(IN const bool is_usb_keyboard);
		void ProcessKeyboardMatrix(
			IN CD4051 &row_cd4051, 
			IN CD4051 &column_cd4051,
			IN const uint8_t &pin_row_8,
			IN const uint8_t &pin_shift_lock,
			IN const KeyMaps &key_maps,
			IN const bool &debug_enabled
		);
		void DebugKeyboardMatrix(IN const KeyMaps &key_maps);
	private:
		void ScanMatrix(IN CD4051 &row_cd4051, IN CD4051 &column_cd4051, IN const KeyMaps &key_maps);
		void ScanSpecialKeys(IN const uint8_t &pin_row_8, IN const uint8_t &pin_shift_lock);
		void WriteMappedUSBKeys(IN const KeyMaps &key_maps);
		void WriteC64SpecialUSBKeys(IN const KeyMaps &key_maps);
		void KeyToggleAction(IN const uint8_t row, IN const uint8_t column, IN const KeyMaps &key_maps);
		bool IsKeyboardShifted(IN const KeyMaps &key_maps);
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
		boolean m_state_map[ROW_COUNT][COLUMN_COUNT];
		boolean m_last_state_map[ROW_COUNT][COLUMN_COUNT];
};

#endif