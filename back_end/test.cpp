#include "bplustree.hpp"
#include <cstring>
int main(){
    bplustree<int, int, 4> test;
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    test.clear();
    printf("\n");
    for (int i = 1;i < 20;i++){
        if (i % 2)
        test.insert(i, i * 2);
    }
    printf("\n\nwrong_part_now\n");
    for (int i = 1;i <= 20;i++)
        if (! (i % 2)) test.insert(i, i * 2);
    for (int i = 1;i < 15;i++) {
        printf("%d\n", test.find(i));
        // printf("\n");
    }
    return 0;
}