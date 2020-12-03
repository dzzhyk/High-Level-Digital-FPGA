#include "hls_stream.h"
#include "filter.h"


// 是否在图像范围内
inline int bounds_ok(const int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}


// 卷积操作
inline int conv(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;

    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
            if (bounds_ok(y + i, x + j) == 1) {
                result += window[i + HALF_SIZE][j + HALF_SIZE] * (i + j);   // (i+j)巧妙地构造了上面的卷积核
            }

    return result;
}


// 卷积运算
void my_conv(hls::stream<int>& in, hls::stream<int>& out) {

    int line_buf[WIN_SIZE - 1][WIDTH];  // 2行WIDTH宽
    int window[WIN_SIZE][WIN_SIZE];     // 卷积核
    int up[WIN_SIZE];                   // 上移缓存

#pragma HLS ARRAY_PARTITION variable=line_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=window complete dim=0
#pragma HLS ARRAY_PARTITION variable=up complete

    // 已经读入的数量
    int read_count = 0;

    // 输入前HALF_SIZE行
    for (int y = 0; y < HALF_SIZE; y++)
        for (int x = 0; x < WIDTH; x++)
            line_buf[y][x] = in.read();

    // 左下角HALF_SIZE+1个
    for (int x = 0; x < HALF_SIZE + 1; x++)
        line_buf[HALF_SIZE][x] = in.read();

    // 初始化完成
    read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;

    // 把初始像素复制进入窗口
    for (int y = 0; y <= HALF_SIZE; y++)
        for (int x = 0; x <= HALF_SIZE; x++)
            window[y+1][x+1] = line_buf[y][x];

    // 开始卷积
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            // 执行卷积
            int val_out = conv(window, y, x);

            // 输出卷积结果
            out.write(val_out);

            // 窗口最右一列上移一位，up准备最右侧的缓存
            up[0] = line_buf[0][x+HALF_SIZE];
            for (int k = 1; k < WIN_SIZE - 1; k++)
                up[k] = line_buf[k - 1][x] = line_buf[k][x];

            // 读进输入值
            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in.read();
                read_count++;
            }

            // 在输入的同时更新缓存，同时完成最右侧一列的最后一个元素
            up[WIN_SIZE - 1] = line_buf[WIN_SIZE - 2][x] = val_in;

            // 把窗口除了最右一列往左移1位
            for (int m = 0; m < WIN_SIZE; m++)
                for (int n = 0; n < WIN_SIZE - 1; n++)
                    window[m][n] = window[m][n + 1];

            // 更新最右一列窗口值
            for (int i = 0; i < WIN_SIZE; i++)
                window[i][WIN_SIZE - 1] = up[i];
        }
    }
}

// 池化操作
void my_pool(hls::stream<int>& in, hls::stream<int>& out){

    int img[HEIGHT][WIDTH]; // 将输入流转换为二位矩阵
    for(int i=0; i<HEIGHT; i++){
        for(int j=0; j<WIDTH; j++){
            img[i][j] = in.read();
        }
    }

    int temp;
    int write_count = 0;

    for(int j=0, tj=0; j<POOL_OUT_HEIGHT; j++, tj+=POOL_STRIDE){
        for(int i=0, ti=0; i<POOL_OUT_WIDTH; i++, ti+=POOL_STRIDE){

            temp = img[tj][ti];

            // 最大池化
            for(int oj=0; oj < POOL_SIZE; oj++){
                for(int oi=0; oi < POOL_SIZE; oi++){
                    if (bounds_ok(tj+oj, ti+oi)) {
						if (temp < img[tj+oj][ti+oi]) {
							temp = img[tj+oj][ti+oi];
						}
                    }
                }
            }

            out.write(temp);
            write_count++;
        }
    }
}


// 融合卷积和池化操作
void filter_top(hls::stream<int>& in, hls::stream<int>& out){

    hls::stream<int> temp;

    my_conv(in, temp);
    my_pool(temp, out);
}
