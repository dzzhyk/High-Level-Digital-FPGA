#include "hls_stream.h"
#include "filter.h"

int mat[HEIGHT][WIDTH] = {
	{-5, -4, -3, -2, -1, 0,},
	{-4, -3, -2, -1, 0, 1,},
	{-3, -2, -1, 0, 1, 2,},
	{-2, -1, 0, 1, 2, 3,},
	{-1, 0, 1, 2, 3, 4,},
	{0, 1, 2, 3, 4, 5,},
};

hls::stream<int> in;
hls::stream<int> out;

int main()
{
	// 准备数据
	for(int i=0; i<HEIGHT; i++){
		for(int j=0; j<WIDTH; j++){
			in.write(mat[i][j]);
		}
	}

	filter_top(in, out);

    printf("结果：\n");
    printMat(out, POOL_OUT_HEIGHT, POOL_OUT_WIDTH);
}
