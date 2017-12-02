#ifndef C64_KEY_MAPS_H
#define C64_KEY_MAPS_H

typedef struct KeyMap {
	uint8_t matrix_unmodified[8][8];
	uint8_t matrix_shifted[8][8];

	uint8_t row_count;
	uint8_t column_count;
	uint8_t restore_key;
	uint8_t shift_lock_key;
	uint8_t left_shift_position_row;
	uint8_t left_shift_position_column;
	uint8_t right_shift_position_row;
	uint8_t right_shift_position_column;
};

class C64KeyMap {
	public:
		KeyMap GetC64KeyMap();

	private:
		void SetUnmodifiedMap(IN KeyMap &key_map);
		void SetShiftedMap(IN KeyMap &key_map);
};

#endif