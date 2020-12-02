#include "filter.h"
int in[HEIGHT][HEIGHT] = {
		{0, 0, 0, 0},
		{0, 3, 3, 0},
		{0, 3, 3, 0},
		{0, 0, 0, 0},
};

int out[HEIGHT][WIDTH];

int main()
{
	int i, j;
	my_filter_v1(in, out);
	printf("float out[%d][%d]= { \n", HEIGHT, WIDTH);
    for (i=0; i<HEIGHT; i++)
    {
    	printf("{");
    	for (j=0;j<WIDTH;j++)
    	{
    		printf("%d,",out[i][j]);
    	}
    	printf("},\n");
    }
    printf("}\n");
}
