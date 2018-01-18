#ifndef KEYMATRIX_H
#define KEYMATRIX_H

class KeyMatrix {
	public:
		KeyMatrix();
		void ProcessKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void StartKeyboard();
		void SendShutdownKey();

	private:
		void ScanKeyMatrix(CD4051 &cd4051_row, CD4051 &cd4051_column);
		void WriteKeys();
		void UpdateActivityMatrix();
		bool IsShiftMode();
};

#endif