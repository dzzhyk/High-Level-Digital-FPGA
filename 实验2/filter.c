#include "filter.h"

#define HALF_SIZE (((WIN_SIZE) - 1) / 2)

inline int bounds_ok(int y, int x)
{
	if(0 <= y && y < HEIGHT && 0 <= x && x < WIDTH){
		return 1;
	}else{
		return 0;
	}
}

// Defines the actual calculation for one output value.
inline int single_operation(int window[3][3], int y, int x)
{
	int result = 0;
	win_i : for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
		win_j : for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
			if (bounds_ok(y + i, x + j) == 1)
				result += window[i + HALF_SIZE][j + HALF_SIZE] * (i + j);
	return result;
}


// A simple implementation of a 2D filter.
void my_filter_v1(const int data_in[HEIGHT][WIDTH], int data_out[HEIGHT][WIDTH]) {

	int window[WIN_SIZE][WIN_SIZE];

	#pragma HLS ARRAY_PARTITION variable=window complete

	for_y : for (int y = 0; y < HEIGHT; y++) {
		for_x : for (int x = 0; x < WIDTH; x++) {

			#pragma HLS PIPELINE

			// Load window
			load_i : for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
				load_j : for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
					if (bounds_ok(y + i, x + j) == 1)
						window[i + HALF_SIZE][j + HALF_SIZE] = data_in[y + i][x + j];

			// Calculate output value
			int val_out = single_operation(window, y, x);
			data_out[y][x] = val_out;
		}
	}
}
