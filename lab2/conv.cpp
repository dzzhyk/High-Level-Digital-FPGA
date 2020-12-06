#include "conv.h"
using namespace std;

/**
 * @author: dzzhyk
 * @since: 2020-12-05 10:36:41
 */

//#define MY_TEST

// �ж��Ƿ���ϱ߽�
inline bool bounds_ok (const short row, const short col){
	return (1 <= row && row <= 6 && 1 <= col && col <= 6);
}

// 2d�������
inline dout_t conv2D(din_t win[3][3], din_t wei[16*3*3], short channel)
{
	dout_t result = (dout_t)0;
	inner_conv2d_i:
	for(short i=0; i<3; ++i)
		inner_conv2d_j:
		for (short j=0; j<3; ++j)
			result += win[i][j] * wei[channel * 9 + i*WIN_SIZE + j];

    return result;
}


void my_engine(din_t img[16*8*8], din_t weight[16*3*3], dout_t out[3*3])
{

	// ��ʼ�����ڳػ���pool_buffer����
	hls::LineBuffer<POOL_BUFFER_SIZE, 1, dout_t> pool_buffer;
	pool_buffer_init:
	for(short i=0; i<POOL_BUFFER_SIZE; i++)
		pool_buffer(i, 0) = (dout_t)0;

	int pool_buffer_size;  // ��ǰԪ������

	// conv_total���ڼ�¼��ǰ�����˶��ٸ�������
	int conv_total;

	// ��ʼ������ͨ����line_buffer[16]
	hls::LineBuffer<2, WIDTH, din_t> linebuf[16];
	linebuf_init:
	for(short channel=0; channel < CHANNEL; channel++){
		linebuf_init_area:
		{
			linebuf_init_1:
			for(short i=0; i<2; i++){
				linebuf[channel](0, i) = img[channel * 64 + i];
				linebuf[channel](1, i) = img[channel * 64 + WIDTH + i];
			}
			linebuf_init_2:
			for(short i=2; i<WIDTH; i++){
				linebuf[channel](0, i) = (din_t)0;
				linebuf[channel](1, i) = img[channel * 64 + i];
			}
		}
	}

	// ��ʼ������ͨ����window[16]
	hls::Window<3, 3, din_t> window[16];
	window_init:
	for(short channel=0; channel < CHANNEL; channel++){
		window_init_1:
		for (short i=0; i<3; i++){
			window_init_1_1:
			for (short j=0; j<3; j++)
				if (i==0 || j==0){
					window[channel](i, j) = (din_t)0;
				}else{
					window[channel](i, j) = img[channel * 64 + (i-1)*WIDTH + j];
				}
		}
	}

	// ����ͨ����������
	din_t new_pixel[CHANNEL];
	new_pixel_init:
	for(short channel=0; channel < CHANNEL; channel++)
		new_pixel[channel] = (din_t)0;

	// read_count��¼��ǰ��ȡ�˶�������
	short read_count = 10;
	pool_buffer_size = 0;

	// Ŀǰ�Ѿ�д��out���ٸ�
	short pool_count = 0;

	conv_total = 0;

	main_conv_row:
	for (short row=0; row < HEIGHT; row++) {
		main_conv_col:
		for (short col=0; col < WIDTH; col++) {

			if (bounds_ok(row, col)) {
				// ������ͨ�����о�����������ҼӺ�����ͨ���Ľ��
				dout_t conv_out = (dout_t)0;

				do_conv2d_loop:
				for (short channel=0; channel < CHANNEL; channel++){
					conv_out += conv2D(window[channel].val, weight, channel);
				}
				conv_total++;

				// �����������뵽�ػ�bufferβ��
				pool_buffer.shift_up(0);
				pool_buffer(7, 0) = conv_out;
				pool_buffer_size++;

				// ������Խ��гػ����������Ҽ�¼����out
				if ((row&1)==0 && (col&1)==0 && pool_buffer_size == POOL_BUFFER_SIZE){

					dout_t result = pool_buffer(0, 0);
					result = pool_buffer(1, 0) > result? pool_buffer(1, 0) : result;
					result = pool_buffer(6, 0) > result? pool_buffer(6, 0) : result;
					result = pool_buffer(7, 0) > result? pool_buffer(7, 0) : result;
					out[pool_count] = result;

					pool_buffer_size -= POOL_SIZE;
					pool_count++;
				}else if(row != 1 && (row&1)==1 && (col&1)==0 && pool_buffer_size == POOL_BUFFER_SIZE){
					// �����������
					pool_buffer_size -= POOL_SIZE;
				}
			}


			// ��ȡ����ͨ����������
			if (read_count < HEIGHT * WIDTH){
				new_pixel_update:
				for(short channel=0; channel < CHANNEL; channel++){
					new_pixel[channel] = img[channel * 64 + read_count];
				}
				read_count++;
			}

			window_loop_area:
			{
				// ����ͨ����window��������1��
				window_shift_left:
				for(short channel=0; channel < CHANNEL; channel++){
					window[channel].shift_left();
				}

				// �������ͨ����window�����Ҳ�1��
				window_right:
				for(short channel=0; channel < CHANNEL; channel++){
					window_right_1:
					for (short i=0; i<2; i++){

						// ��linebuf��ȡ����Ҫ�����ֵgetval(i, col)
						// col = WIN_SIZE-1 ��ʾ���Ҳ�
						din_t temp = linebuf[channel](i,(col+2) % WIDTH);
						window[channel](i, 2) = temp;
					}
				}

				window_update_new:
				// ��ÿ��ͨ���������ؼ���ÿ��ͨ����window�����½�
				for(short channel=0; channel < CHANNEL; channel++)
					window[channel](2, 2) = new_pixel[channel];
			}

			linebuf_loop_area:
			{
				linebuf_shift_up:
				// ��������ͨ����linebuf
				for(short channel=0; channel < CHANNEL; channel++)
					linebuf[channel].shift_up( (col+2)%WIDTH );

				linebuf_update:
				for(short channel=0; channel < CHANNEL; channel++)
					linebuf[channel](1,(col+2) % WIDTH) = new_pixel[channel];
			}
		}
	}
}
