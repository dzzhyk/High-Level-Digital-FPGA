#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>

using namespace std;

#define TOTAL 100
#define INF 0x3f3f3f3f
#define WIN_SIZE 3
#define WIDTH 8
#define HEIGHT 8
#define HALF_SIZE (((WIN_SIZE) - 1) / 2)
#define CHANNEL 1

#define CONV_HEIGHT (HEIGHT-HALF_SIZE*2)
#define CONV_WIDTH (WIDTH-HALF_SIZE*2)

#define POOL_SIZE 2
#define POOL_STRIDE 2
#define POOL_OUT_HEIGHT ((CONV_HEIGHT - POOL_SIZE) / POOL_STRIDE + 1)
#define POOL_OUT_WIDTH ((CONV_WIDTH - POOL_SIZE) / POOL_STRIDE + 1)


// 是否在图像范围内
int bounds_ok(const int y, int x)
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
 *  这里如果是3*3，则使用的卷积核如下
 * -2  -1  0
 * -1  0   1
 *  0  1   2
 *
 * 应该是用来检测45度角的
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



// 卷积运算
void my_conv(const int in[], int out[]) {

    int line_buf[WIN_SIZE - 1][WIDTH];  // 2行WIDTH宽
    int window[WIN_SIZE][WIN_SIZE];     // 卷积核
    int up[WIN_SIZE];                   // 上移缓存

    memset(window, 0, sizeof(window));
    memset(line_buf, 0, sizeof(line_buf));
    memset(up, 0, sizeof(up));

    // 已经读入的数量
    int read_count = 0;

    // 初始化
    for(int i=0; i<2; i++)
        line_buf[0][i] = in[i];
    for(int i = 2; i < WIDTH; i++)
        line_buf[1][i] = in[i];
    for(int i=0; i<2; i++){
        line_buf[1][i] = in[WIDTH + i];
    }

    // 初始化完成
    read_count = WIDTH + HALF_SIZE + 1;

    // 把初始像素复制进入窗口
    for (int y = 0; y <= HALF_SIZE; y++)
        for (int x = 0; x <= HALF_SIZE; x++)
            window[y+1][x+1] = line_buf[y][x];

    // 记录输出位置
    int write_count = 0;

    // 开始卷积
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            // 执行卷积
            int val_out = conv(window, y, x);

            if (1 <= y && y <= CONV_HEIGHT && 1 <= x && x <= CONV_WIDTH) {
                out[write_count] = val_out;
                write_count++;
            }

            // 读进输入值
            int val_in = 0;
            if (read_count < HEIGHT * WIDTH)
            {
                val_in = in[read_count];
                read_count++;
            }
            // 准备最右侧的缓存up
            up[0] = line_buf[0][(x+HALF_SIZE+1) % WIDTH];
            up[1] = line_buf[0][(x+HALF_SIZE+1) % WIDTH] = line_buf[1][(x+HALF_SIZE+1) % WIDTH];
            up[2] = line_buf[1][(x+HALF_SIZE+1) % WIDTH] = val_in;

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
void my_pool(const int in[], int out[]){

    int img[CONV_HEIGHT][CONV_WIDTH]; // 将输入流转换为二位矩阵
    for(int i=0; i<CONV_HEIGHT; i++){
        for(int j=0; j<CONV_WIDTH; j++){
            img[i][j] = in[i*CONV_WIDTH + j];
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

            out[write_count] = max(out[write_count], temp);
            write_count++;
        }
    }
}


// 将流式输出转换为二位矩阵输出
void printMat(const int in[], int h, int w){
    for(int i=0; i<h; i++){
        for(int j=0; j<w-1; j++){
            cout << in[i*w+j] << " ";
        }
        cout << in[i*w+w-1] << endl;
    }
}

// 融合卷积和池化操作
// 输入是图像大小
// 输出是池化后大小
void combine(const int in[], int out[]) {

    int temp[CONV_HEIGHT * CONV_WIDTH];

    for (int i = 0; i < CHANNEL; i++) {
        memset(temp, 0, sizeof(temp));

        // 获取当前channel的矩阵
        int a[HEIGHT * WIDTH];
        memset(a, 0, sizeof(a));
        for (int j = 0; j < (HEIGHT * WIDTH); j++) {
            a[j] = in[i * (WIDTH * HEIGHT) + j];
        }

       printf("Input Matrix:\n");
       printMat(a, HEIGHT, WIDTH);

        my_conv(a, temp);

       printf("Conv2D Output:\n");
       printMat(temp, CONV_HEIGHT, CONV_WIDTH);

        my_pool(temp, out);


    }

   printf("Max pooling Output:\n");
    printMat(out, POOL_OUT_HEIGHT, POOL_OUT_WIDTH);
}

int mat[16][8][8] = {

	{{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},

	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},
    {{41,35, 190, 132, 225, 108, 214, 174},
	{82, 144, 73, 241, 241, 187, 233, 235},
	{179, 166, 219, 60, 135, 12, 62, 153},
	{36, 94, 13, 28, 6, 183, 71, 222},
	{179, 18, 77, 200, 67, 187, 139, 166},
	{31, 3, 90, 125, 9, 56, 37, 31},
	{93, 212, 203, 252, 150, 245, 69, 59},
	{19, 13, 137, 10, 28, 219, 174, 50}},
	{{41,35, 190, 132, 225, 108, 214, 174},
    {82, 144, 73, 241, 241, 187, 233, 235},
    {179, 166, 219, 60, 135, 12, 62, 153},
    {36, 94, 13, 28, 6, 183, 71, 222},
    {179, 18, 77, 200, 67, 187, 139, 166},
    {31, 3, 90, 125, 9, 56, 37, 31},
    {93, 212, 203, 252, 150, 245, 69, 59},
    {19, 13, 137, 10, 28, 219, 174, 50}},

};

int main(){

    string base_data;
    string base_answer;
    int cnt = 1;
    for(int t=1; t<=TOTAL; t++){
            
        // base_data = "./test_bench/data/" + to_string(t);
        // base_answer = "./test_bench/answer/" + to_string(t);

        // 准备数据，从文件输入
        // freopen(base_data.c_str(), "r", stdin);
        // freopen(base_answer.c_str(), "w", stdout);


        int out[POOL_OUT_HEIGHT * POOL_OUT_WIDTH];
        fill(out, out + POOL_OUT_HEIGHT * POOL_OUT_WIDTH, -INF);

        // 把in转化为单行矩阵
        int a[HEIGHT * WIDTH * CHANNEL];
        memset(a, 0, sizeof(a));
        for(int k=0; k<CHANNEL; k++)
            for(int i=0; i<HEIGHT; i++)
                for(int j=0; j<WIDTH; j++)
                    // cin >> a[(k * HEIGHT * WIDTH) + i*WIDTH + j];
                    a[(k * HEIGHT * WIDTH) + i*WIDTH + j] = mat[k][i][j];


        combine(a, out);

        fclose(stdout);
        fclose(stdin);
    }
    return 0;
}