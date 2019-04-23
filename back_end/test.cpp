#include "bplustree.hpp"
#include <cstring>
int it = 2333;
int fake_random(int a){
    return (a * 23) % 10007;
}
int main(){
    bplustree<int, int, 4> test;
    test.initialize("data.txt", "ind.txt", "data_all.txt", "inall.txt");
    test.clear();
    int its[50];
    for (int i = 0;i < 30;i++){
        its[i] = it = fake_random(it);
        test.insert(its[i], its[i] * 2);
    }
    for (int i = 0;i < 30;i++){
        printf("should be: %d ", its[i] * 2);
        printf("it is: %d\n", test.find(its[i]));
    }
    return 0;
}