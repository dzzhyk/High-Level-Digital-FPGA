#include "hls_stream.h"

#define WIN_SIZE 3 // must be odd
#define HALF_SIZE (((WIN_SIZE) - 1) / 2)


// 是否在图像范围内
inline bool bounds_ok(int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}


// Defines the actual calculation for one output value.
inline int single_operation(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;

    win_i : for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
        win_j : for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
            if (bounds_ok(y + i, x + j)) {
                result += window[i + HALF_SIZE][j + HALF_SIZE] * (i + j);
            }

    return result;
}


// A buffered implementation of a 2D filter.
void my_filter_buffer(hls::stream<int>& in_stream, hls::stream<int>& out_stream) {

    int line_buf[WIN_SIZE - 1][WIDTH];
    int window[WIN_SIZE][WIN_SIZE];
    int right[WIN_SIZE];

    #pragma HLS ARRAY_PARTITION variable=line_buf complete dim=1
    #pragma HLS ARRAY_PARTITION variable=window complete dim=0
    #pragma HLS ARRAY_PARTITION variable=right complete

    // 把初始值加入line buffer中
    int read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;

    /**
     * 优化指令pragma HLS pipeline的作用是缩短C函数或C循环之内的指令触发间隔（initial interval，II）
     * 在不使用该指令的情况下，函数或循环默认的指令触发间隔II为N
     * 使用该指令后，编译器将将对II进行优化，默认将其缩短为1。用户也可以在优化指令中指定期望的II值
     * 
     * 指令语法 #pragma HLS pipeline II=<int> enable_flush rewind
     * II = <int> 设定期望的流水线程序触发间隔II；默认值为1
     * enable_flush （可选）如果流水线输入不有效，则清空流水线
     * rewind （可选）使多次循环执行形成流水线，多次循环之间不产生时间间隔
     **/

    buf_x1 : for (int x = WIDTH - HALF_SIZE - 1; x < WIDTH; x++)
    #pragma HLS PIPELINE
        line_buf[HALF_SIZE - 1][x] = in_stream.read();
    buf_y : for (int y = HALF_SIZE; y < WIN_SIZE - 1; y++)
        buf_x2 : for (int x = 0; x < WIDTH; x++)
        #pragma HLS PIPELINE
            line_buf[y][x] = in_stream.read();

    // 把初始值复制进入窗口
    win_y : for (int y = HALF_SIZE; y < WIN_SIZE; y++)
        win_x : for (int x = HALF_SIZE; x < WIN_SIZE; x++)
        #pragma HLS PIPELINE
            window[y][x] = line_buf[y - 1][x + WIDTH - WIN_SIZE];

    // 开始卷积
    for_y : for (int y = 0; y < HEIGHT; y++) {
        for_x : for (int x = 0; x < WIDTH; x++) {
        #pragma HLS PIPELINE

            // 计算输出值
            int val_out = single_operation(window, y, x);

            // 写出输出值
            out_stream.write(val_out);

            // 右移一位？
            right[0] = line_buf[0][x];
            for (int y = 1; y < WIN_SIZE - 1; y++)
                right[y] = line_buf[y - 1][x] = line_buf[y][x];

            // 读进输入值
            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in_stream.read();
                read_count++;
            }
            right[WIN_SIZE - 1] = line_buf[WIN_SIZE - 2][x] = val_in;

            // 把窗口往左移1位
            for (int y = 0; y < WIN_SIZE; y++)
                for (int x = 0; x < WIN_SIZE - 1; x++)
                    window[y][x] = window[y][x + 1];

            // 更新右侧窗口最大值
            for (int y = 0; y < WIN_SIZE; y++)
                window[y][WIN_SIZE - 1] = right[y];
        }
    }
}