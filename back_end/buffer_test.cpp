//#define DEBUG_MODE
#include "bufferpool.hpp"
int main(){
    FILE *datafile;
    datafile = fopen("buf_test","wb+");
    buf_pool_t<> *buf = new buf_pool_t<>(datafile);
    const int len = 4080 / sizeof(long long);
    printf("%d\n",len);
    buf->print_lists();
    buf_block_t *buf_b = buf->load_it(0);
    long long *data = (long long*)(buf_b->frame);
    for (int i = 0;i < len;i++) data[i] = i;
    buf->dirty(buf_b);

    buf_b = buf->load_it(4096);
    data = (long long*)(buf_b->frame);
    for (int i = 0;i < len;i++) data[i] = i + 1;
    buf->dirty(buf_b);


    buf->print_lists();
    delete buf;
    fclose(datafile);
    datafile = fopen("buf_test","rb+");
    buf = new buf_pool_t<>(datafile);
    data = (long long *)((buf->load_it(0))->frame);
    for (int i = 0;i < len;i++){
        printf("%lld; ",data[i]);
    }
    data = (long long *)((buf->load_it(4096))->frame);
    for (int i = 0;i < len;i++){
        printf("%lld; ",data[i]);
    }
    delete buf;
    fclose(datafile);
    return 0;
}