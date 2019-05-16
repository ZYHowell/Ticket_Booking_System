//#define DEBUG_MODE
#include "bufferpool.hpp"
int main(){
    FILE *datafile;
    datafile = fopen("buf_test","wb+");
    buf_pool_t<> *buf = new buf_pool_t<>;
    buf->init(datafile);
    const int len = 4080 / sizeof(long long);
    printf("%d\n",len);
    buf->print_lists();
    buf_block_t *buf_b;
    long long *data;

    for (int j = 0;j < 5;j++){
        buf_b = buf->load_it(4096 * j);
        data = (long long*)(buf_b->frame);
        for (int i = 0;i < len;i++) data[i] = i + j;
        buf->dirty(buf_b);
    }


    buf->print_lists();
    delete buf;
    fclose(datafile);
    datafile = fopen("buf_test","rb+");
    buf = new buf_pool_t<>;
    buf->init(datafile);
    for (int j = 0;j < 5;j++){
        data = (long long *)((buf->load_it(4096 * j))->frame);
        for (int i = len - 2;i < len;i++){
            printf("%lld; ",data[i]);
        }
    }
    delete buf;
    fclose(datafile);
    return 0;
}