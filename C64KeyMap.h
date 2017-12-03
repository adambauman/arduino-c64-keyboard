#ifndef C64KEYMAP_H
#define C64KEYMAP_H

#define KEYMODE_NORMAL  0
#define KEYMODE_SHIFT	1
#define KEYMODE_ALT		2
#define KEYMODE_CTRL	3

struct ShiftKey {
	uint8_t row;
	uint8_t column;
};

class C64KeyMap {
	public:
		C64KeyMap();
		uint8_t GetKeyCode(uint8_t &column, uint8_t &row, uint8_t keymode);
	
	public:
		ShiftKey m_left_shift;
		ShiftKey m_right_shift;
};

#endif