#include "xc.h"

#define N 8
#define D (1<<N)

int32_t extern g900;
int32_t extern g1100;

int32_t extern coef900[4][6];

int32_t extern coef1100[4][6];

static int32_t res900[4][3] = {{0, 0, 0},{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static int32_t res1100[4][3] = {{0, 0, 0},{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

int32_t stage900(int32_t x, int i){
    res900[i][0] = (x*D - coef900[i][4]*res900[i][1] - coef900[i][5]*res900[i][2]) >> N;
    int32_t y = g900*(coef900[i][0]*res900[i][1] + coef900[i][2]*res900[i][2]) >> 2*N;
    res900[i][2] = res900[i][1];
    res900[i][1] = res900[i][0];
    return y;
}

int32_t filtre900(int16_t m){
    int32_t y = m;
    int i;
    for (i=0;i<4;i++){
        y = stage900(y,i);
    }
    return y;
}


int32_t stage1100(int32_t x, int i){
    res1100[i][0] = (x*D - coef1100[i][4]*res1100[i][1] - coef1100[i][5]*res1100[i][2]) >> N;
    int32_t y = g1100*(coef1100[i][0]*res1100[i][1] + coef1100[i][2]*res1100[i][2]) >> 2*N;
    res1100[i][2] = res1100[i][1];
    res1100[i][1] = res1100[i][0];
    return y;
}

int32_t filtre1100(int16_t m){
    int32_t y = m;
    int i;
    for (i=0;i<4;i++){
        y = stage1100(y,i);
    }
    return y;
}
