#ifndef KEYMATRIX_H
#define KEYMATRIX_H

#ifndef CD4051_H
#include "CD4051.h"
#endif

struct ShiftKey {
	uint8_t position_row;
	uint8_t position_column;
};

class KeyMatrix {
	public:
		KeyMatrix();
		void ProcessKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void StartKeyboard();

	private:
		void ScanKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void WriteKeys();
		void UpdateActivityMatrix();
		//uint8_t GetKeyCode(uint8_t &column, uint8_t &row);
		bool IsShiftKeyActive();
		
	private:
		ShiftKey m_left_shift_key;
		ShiftKey m_right_shift_key;
		
};

#endif