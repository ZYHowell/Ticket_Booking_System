#include "bplustree.hpp"
#include <cstring>
int main(){
    bplustree<int, int, 4> test;
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    test.clear();
    test.insert(1, 1);
    test.insert(2, 3);
    test.insert(3, 5);
    for (int i = 1; i <= 5;i++)
        printf("%d\n", test.find(i));
    test.insert(4, 6);
    for (int i = 1; i <= 5;i++)
        printf("%d\n", test.find(i));
    test.insert(5, 7);
    for (int i = 1; i <= 5;i++)
        printf("%d\n", test.find(i));
    return 0;
}