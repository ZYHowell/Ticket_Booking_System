#include <cstring>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <string>
#include <random>
#include "bplustree.hpp"
int it = 23333;
const unsigned Ta = 33333331, Tb = 1 << 16 | 3;
unsigned Tc;
inline unsigned randint()
{
    return Tc = (Tc * Ta + Tb);
}
int fake_random(int a){
    return (a * 23) % 100007;
}
// struct test_str{
//     char a[20], b[20];
//     test_str(char *aa = "", char *bb = ""):a(""), b(""){
//         strcpy(a, aa), strcpy(b, bb);
//     }
// };
// bool judgement(test_str a, test_str b){
//     return a.a < b.a;
// }
std::string randstr(){
    int a = rand() % 100 + 1;
    std::string ret;
    for (int i = 0;i < a;i++) ret += rand() % 26 + 'A';
    return ret;
}
int its[100000];
int main(){
    DWORD k = ::GetTickCount();
    bplustree <int, int, 512> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
    for (int i = 0;i < 100000;i++){
        its[i] = randint();
        test.insert(its[i], its[i]);
        // printf("%d %d; ", its[i], con++);
    }
    // for (int i = 0;i < 10000;i++){
    //     printf("should be: %d ", its[i] * 2);
    //     printf("it is: %d\n", test.find(its[i]));
    // }
    // std::sort(its, its + 10000);
    //for (int i = 0;i < 10000;i++) cout << test.find(its[i]) << endl;
    //vector<pair<test_str, int>> v = test.listof(test_str(15, 0), judgement);
    //for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    std::cout << std::endl << ::GetTickCount() - k << std::endl;
    return 0;
}