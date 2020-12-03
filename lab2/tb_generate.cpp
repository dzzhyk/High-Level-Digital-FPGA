#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
using namespace std;

#define HEIGHT 8
#define WIDTH 8
#define CHANNEL 16
#define TOTAL 100

// 生成测试样例
int main(){

    string base;

    for(int t=1; t<=TOTAL; t++){
        
        base = "./test_bench/data/" + to_string(t);
        
        freopen(base.c_str(), "w", stdout);
        
        for(int k=0; k < CHANNEL; k++){
            for(int i=0; i<HEIGHT; i++){
                for(int j=0; j<WIDTH-1; j++){
                    cout << rand()%256 << " ";        
                }
                cout << rand()%256 << endl;
            }
        }
        
        fclose(stdout);
    }

    return 0;
}