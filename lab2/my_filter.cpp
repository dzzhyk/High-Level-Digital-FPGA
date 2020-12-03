#include "hls_stream.h"
#include "my_filter.h"


inline int bounds_ok(const int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}


inline int conv(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;

    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
            if (bounds_ok(y + i, x + j) == 1) {
                result += window[i + HALF_SIZE][j + HALF_SIZE] * (i + j);   // (i+j)宸у鍦版瀯閫犱簡涓婇潰鐨勫嵎绉�?
            }

    return result;
}


void my_conv(hls::stream<int>& in, hls::stream<int>& out) {

    int line_buf[WIN_SIZE - 1][WIDTH];
    int window[WIN_SIZE][WIN_SIZE];
    int up[WIN_SIZE];

    int read_count = 0;

    for (int y = 0; y < HALF_SIZE; y++)
        for (int x = 0; x < WIDTH; x++)
            line_buf[y][x] = in.read();

    for (int x = 0; x < HALF_SIZE + 1; x++)
        line_buf[HALF_SIZE][x] = in.read();

    read_count = WIDTH * HALF_SIZE + HALF_SIZE + 1;

    for (int y = 0; y <= HALF_SIZE; y++)
        for (int x = 0; x <= HALF_SIZE; x++)
            window[y+1][x+1] = line_buf[y][x];

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            int val_out = conv(window, y, x);

            out.write(val_out);

            up[0] = line_buf[0][x+HALF_SIZE];
            for (int k = 1; k < WIN_SIZE - 1; k++)
                up[k] = line_buf[k - 1][x] = line_buf[k][x];

            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in.read();
                read_count++;
            }

            up[WIN_SIZE - 1] = line_buf[WIN_SIZE - 2][x] = val_in;

            for (int m = 0; m < WIN_SIZE; m++)
                for (int n = 0; n < WIN_SIZE - 1; n++)
                    window[m][n] = window[m][n + 1];

            for (int i = 0; i < WIN_SIZE; i++)
                window[i][WIN_SIZE - 1] = up[i];
        }
    }
}


void my_pool(hls::stream<int>& in, hls::stream<int>& out){

    int img[HEIGHT][WIDTH];
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


void filter_top(hls::stream<int>& in, hls::stream<int>& out){

    hls::stream<int> temp;

    my_conv(in, temp);
    my_pool(temp, out);
}
