#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

#define INF 0x3f3f3f3f

#define WIN_SIZE 3
#define WIDTH 8
#define HEIGHT 8
#define CHANNEL 16
#define HALF_SIZE (((WIN_SIZE) - 1) / 2)

// stride = 1
#define CONV_HEIGHT (HEIGHT-HALF_SIZE*2)
#define CONV_WIDTH (WIDTH-HALF_SIZE*2)

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define POOL_OUT_HEIGHT ((CONV_HEIGHT - POOL_SIZE) / POOL_STRIDE + 1)
#define POOL_OUT_WIDTH ((CONV_WIDTH - POOL_SIZE) / POOL_STRIDE + 1)


// 是否在图像范围内
int bounds_ok(int y, int x)
{
    return (0 <= y && y < HEIGHT && 0 <= x && x < WIDTH);
}


// 定义了一个卷积核
// 注意：卷积核比较简单，所有通道都相同的
const int kernel[WIN_SIZE][WIN_SIZE] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
};

/** 卷积操作
 * 应该是用来检测45度斜对角特征
 **/
int conv(int window[WIN_SIZE][WIN_SIZE], int y, int x)
{
    int result = 0;
    for (int i = -HALF_SIZE; i <= HALF_SIZE; i++)
        for (int j = -HALF_SIZE; j <= HALF_SIZE; j++)
            if (bounds_ok(y + i, x + j) == 1) {
                result += window[i + HALF_SIZE][j + HALF_SIZE] * kernel[i + HALF_SIZE][j + HALF_SIZE];
            }
    
    return result;
}


// 单channel卷积运算，带有Line Buffer
void my_conv(const int in[], int out[]) {

    int line_buf[WIN_SIZE - 1][WIDTH];          // 2行WIDTH宽
    int window[WIN_SIZE][WIN_SIZE];             // 当前卷积核位置
    int up[WIN_SIZE];                           // 上移缓存

    memset(window, 0, sizeof(window));
    memset(line_buf, 0, sizeof(line_buf));
    memset(up, 0, sizeof(up));

    // 已经读入的数量
    int read_count = 0;

    // 先把前WIN_SIZE个读进来
    for(int x=0; x<WIN_SIZE; x++){
        window[0][x] = in[x];
    }

    // 输入右上角window右侧的所有
    for(int x = WIN_SIZE; x < WIDTH; x++)
        line_buf[0][x] = in[x];

    // 输入前HALF_SIZE行
    for (int y = 1; y <= HALF_SIZE; y++)
        for (int x = 0; x < WIDTH; x++)
            line_buf[y][x] = in[y*WIDTH+x];

    // 左下角WIN_SIZE个
    for (int x = 0; x < WIN_SIZE; x++)
        line_buf[HALF_SIZE][x] = in[(WIN_SIZE-1)*WIDTH + x];

    // 初始化完成
    read_count = WIDTH * (WIN_SIZE-1) + WIN_SIZE;

    // 把初始像素复制进入窗口
    for (int y = 1; y < WIN_SIZE; y++)
        for (int x = 0; x < WIN_SIZE; x++)
            window[y][x] = line_buf[y-1][x];

    // 记录输出位置
    int write_count = 0;

    // 开始卷积
    for (int y = HALF_SIZE; y < HALF_SIZE+CONV_HEIGHT; y++) {
        for (int x = HALF_SIZE; x < HALF_SIZE+CONV_WIDTH; x++) {

            // 执行卷积
            int val_out = conv(window, y, x);

            // 输出卷积结果
            out[write_count] = val_out;
            write_count++;

            // 准备最右侧的缓存up
            up[0] = line_buf[0][x+HALF_SIZE];
            for (int k = 1; k < WIN_SIZE - 1; k++)
                up[k] = line_buf[k - 1][x] = line_buf[k][x];

            // 读进输入值
            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in[read_count];
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

// 单层池化操作
void my_pool(const int in[], int out[]){

    int img[HEIGHT][WIDTH]; // 将输入流转换为二位矩阵
    for(int i=0; i<HEIGHT; i++){
        for(int j=0; j<WIDTH; j++){
            img[i][j] = in[i*WIDTH + j];
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
                        temp = max(temp, img[tj+oj][ti+oi]);
                    }
                }
            }

            // 综合了多个channel的和
            out[write_count] = max(temp, out[write_count]);
            write_count++;
        }
    }
}


// 将流式输出转换为二维矩阵输出
void printMat(const int in[], int h, int w){
    for(int i=0; i<h; i++){
        for(int j=0; j<w-1; j++){
            cout << in[i*w+j] << " ";
        }
        cout << in[i*w+(w-1)] << endl;
    }
}


// 融合卷积和池化操作
// 输入是图像大小
// 输出是池化后大小
void combine(const int in[], int out[]){

    int temp[CONV_HEIGHT * CONV_WIDTH];

    for(int i=0; i<CHANNEL; i++){

        cout << "************ Channel " << i + 1 << " **************" << endl;

        memset(temp, 0, sizeof(temp));
    
        // 获取当前channel的矩阵
        int a[HEIGHT * WIDTH];
        memset(a, 0, sizeof(a));
        for(int j=0; j<(HEIGHT*WIDTH); j++){
            a[j] = in[i*(WIDTH * HEIGHT) + j];
        }

        printf("Input Matrix:\n");
        printMat(a, HEIGHT, WIDTH);

        my_conv(a, temp);

        printf("Conv2D Output:\n");
        printMat(temp, CONV_HEIGHT, CONV_WIDTH);

        my_pool(temp, out);

        printf("Max pooling Output:\n");
        printMat(out, POOL_OUT_HEIGHT, POOL_OUT_WIDTH);
    
    }

}



int main(){

    // 准备数据，从文件输入
    freopen("./test_bench/data/1", "r", stdin);

    // 创建一个输出矩阵
    // freopen("./test_bench/answer/1", "w", stdout);
    int out[POOL_OUT_HEIGHT * POOL_OUT_WIDTH];
    fill(out, out + POOL_OUT_HEIGHT * POOL_OUT_WIDTH, -INF);
    
    // 把in转化为单行矩阵
    int a[HEIGHT * WIDTH * CHANNEL];
    memset(a, 0, sizeof(a));
    for(int k=0; k<CHANNEL; k++)
        for(int i=0; i<HEIGHT; i++)
            for(int j=0; j<WIDTH; j++)
                cin >> a[(k * HEIGHT * WIDTH) + i*WIDTH + j];
        

    // 输出
    // for(int k=0; k<CHANNEL; k++){
    //     for(int i=0; i<HEIGHT; i++){
    //         for(int j=0; j<WIDTH-1; j++){
    //             cout << a[(k * HEIGHT * WIDTH) + i*WIDTH + j] << " ";
    //         }
    //         cout << a[(k * HEIGHT * WIDTH) + i*WIDTH + WIDTH - 1] << endl;
    //     }
    // }
        

    // 合并操作
    combine(a, out);


    fclose(stdin);
    return 0;
}
