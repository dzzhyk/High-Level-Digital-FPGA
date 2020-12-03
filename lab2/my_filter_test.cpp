#include <iostream>
#include <iomanip>
#include "my_filter.h"

using namespace std;

int mat[HEIGHT][WIDTH] = {
	{-5, -4, -3, -2, -1, 0,},
	{-4, -3, -2, -1, 0, 1,},
	{-3, -2, -1, 0, 1, 2,},
	{-2, -1, 0, 1, 2, 3,},
	{-1, 0, 1, 2, 3, 4,},
	{0, 1, 2, 3, 4, 5,},
};


int main() {

	hls::stream<int> in;
	hls::stream<int> out;

	for(int i=0; i<HEIGHT; i++){
		for(int j=0; j<WIDTH; j++){
			in.write(mat[i][j]);
		}
	}

	filter_top(in, out);

    printf("ans: \n");
    my_printMat(out, POOL_OUT_HEIGHT, POOL_OUT_WIDTH);
}
