#ifndef C64KEYMAP_H
#define C64KEYMAP_H

#define KEYMODE_NORMAL  0
#define KEYMODE_SHIFT	1
#define KEYMODE_ALT		2
#define KEYMODE_CTRL	3

class C64KeyMap {
	public:
		uint8_t GetKeyCode(uint8_t &column, uint8_t &row, uint8_t keymode);
	
};

#endif