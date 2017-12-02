#ifndef KEYMATRIX_H
#define KEYMATRIX_H

#ifndef CD4051_H
#include "CD4051.h"
#endif

class KeyMatrix {
	public:
		KeyMatrix();
		void ProcessKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void StartKeyboard();

	private:
		void ScanKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void WriteKeys();
		void UpdateActivityMatrix();
};

#endif