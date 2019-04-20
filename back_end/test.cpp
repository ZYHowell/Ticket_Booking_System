#include "bplustree.hpp"
#include <cstring>
int main(){
    bplustree<int, int, 4> test;
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    test.insert(1, 1);
    printf("\n%d\n", test.find(1));
    test.insert(2, 3);
    printf("\n%d\n", test.find(2));
    return 0;
}