#include <cstring>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <random>
#include "bplustree.hpp"
int it = 23333;
const unsigned Ta = 33333331, Tb = 1 << 16 | 3;
unsigned Tc;
inline unsigned randint()
{
    return Tc = (Tc * Ta + Tb);
}
struct test_t{
    char inf[100];
    test_t (const char *aa = ""){
        inf[0] = '\0';
        strcpy(inf, aa);
    }
};
bool operator<(const test_t&a, const test_t&b){
    return strcmp(a.inf, b.inf) < 0;
}
test_t randstr(){
    int a = rand() % 50 + 1;
    char ret[50];
    for (int i = 0;i < a;i++) ret[i] = rand() % 26 + 'A';
    ret[a] = '\0';
    return ret;
}
int its[100000];
int main(){
    DWORD k = ::GetTickCount();srand(k);
    bplustree <int, int> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
    int i = 0;
    for (i = 0;i < 10000;i++){
        its[i] = randint();
        test.insert(its[i], its[i]);
        // printf("%d %d; ", its[i], con++);
    }
    for (int i = 0;i < 10000;i++){
        printf("should be: %d;", its[i]);
        printf("it is: %d\n", test.find(its[i]));
    }
    // std::sort(its, its + 10000);
    //for (int i = 0;i < 10000;i++) cout << test.find(its[i]) << endl;
    //vector<pair<test_str, int>> v = test.listof(test_str(15, 0), judgement);
    //for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    std::cout << std::endl << ::GetTickCount() - k << std::endl;
    return 0;
}