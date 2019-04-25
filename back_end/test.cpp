#include "bplustree.hpp"
#include <cstring>
int it = 2333;
int fake_random(int a){
    return (a * 23) % 10007;
}
bool judgement(int a, int b){
    return a < b && 2 * a < b;
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
    vector<pair<int, int>> v = test.listof(7352 / 2, judgement);
    for (int i = 0;i < v.size();i++) printf("%d %d; ", v[i].first, v[i].second);
    return 0;
}