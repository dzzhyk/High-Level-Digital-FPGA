#ifndef CONV_H_
#define CONV_H_

#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "ap_int.h"
#include "hls_video.h"
#include <hls_stream.h>


#define INF 0x3f3f3f3f
#define WIN_SIZE 3
#define WIDTH 8
#define HEIGHT 8
#define CHANNEL 16

#define CONV_HEIGHT 6
#define CONV_WIDTH 6

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define POOL_OUT_HEIGHT ((CONV_HEIGHT - POOL_SIZE) / POOL_STRIDE + 1)
#define POOL_OUT_WIDTH ((CONV_WIDTH - POOL_SIZE) / POOL_STRIDE + 1)
#define POOL_BUFFER_SIZE (CONV_WIDTH + POOL_SIZE)

typedef ap_int<8> din_t;
typedef ap_int<24> dout_t;

void my_engine(din_t img[16*8*8], din_t weight[16*3*3], dout_t out[3*3]);


#endif
