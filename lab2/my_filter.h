#ifndef MY_FILTER
#define MY_FILTER

#include "hls_stream.h"
#include <stdio.h>

#define WIN_SIZE 3
#define WIDTH 6
#define HEIGHT 6
#define HALF_SIZE (((WIN_SIZE) - 1) / 2)

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define POOL_OUT_HEIGHT ((HEIGHT - POOL_SIZE) / POOL_STRIDE + 1)
#define POOL_OUT_WIDTH ((WIDTH - POOL_SIZE) / POOL_STRIDE + 1)


void filter_top(hls::stream<int>& in, hls::stream<int>& out);

#endif
