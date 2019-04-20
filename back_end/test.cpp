#include "bplustree.hpp"
#include <cstring>
int main(){
    bplustree<int, int, 4> test;
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    // test.insert(1, 1);
    return 0;
}