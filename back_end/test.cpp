#include <cstring>
#include <algorithm>
#include <iostream>
#include <random>
#include "bplustree.hpp"
#include <Windows.h>

const int maxn = 100000 + 5;

#ifdef TEST_INT_MODE
const unsigned Ta = 33333331, Tb = 1 << 16 | 3;
unsigned Tc;
inline unsigned randint(){
    return Tc = (Tc * Ta + Tb);
}
struct test_t{
    int a, b;
    test_t(int aa = 0, int bb = 0):a(aa), b(bb){}
    test_t operator=(const test_t &other){
        a = other.a, b = other.b;
        return *this;
    }
};
bool operator<(const test_t &a, const test_t &b){
    if (a.a != b.a) return a.a < b.a;
    return a.b < b.b;
}
using value_t = int;
bool judgement(const test_t &a, const test_t &b){
    return a.a < b.a;
}
#endif
#ifndef TEST_INT_MODE
struct test_t{
    char inf[100];
    test_t (const char *aa = ""){
        inf[0] = '\0';
        strcpy(inf, aa);
    }
	~test_t(){};
};
bool operator<(const test_t&a, const test_t&b){
    return strcmp(a.inf, b.inf) < 0;
}
test_t randstr(){
    int a = rand() % 70 + 1;
    char ret[80];
    for (int i = 0;i < a;i++) ret[i] = rand() % 26 + 'A';
    ret[a] = '\0';
    return ret;
}
using value_t = test_t;
#endif
test_t its[maxn];
int main(){
    DWORD k = ::GetTickCount();
	FILE *file = fopen("data.txt", "wb");fclose(file);
	file = fopen("alloc.txt", "wb");fclose(file);
    srand(214748364);
    bplustree <test_t, value_t> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
#ifdef TEST_MODE
	printf("\n\n");
#endif
    int i = 0;
    for (i = 0;i < 10000;i++){
    #ifdef TEST_INT_MODE
        its[i] = test_t(randint() % 20, i);
        test.insert(its[i], i);
        //to test the listof function, change randint() to "randint() % 20"
    #endif
    #ifndef TEST_INT_MODE
        its[i] = randstr();
        test.insert(its[i], its[i]);
    #endif
    }
    for (int i = 0;i < 10000;i++){
    #ifdef TEST_INT_MODE
        if (test.find(its[i]) != i) printf("wrong\n");
    #endif
    #ifndef TEST_INT_MODE
		if(strcmp(test.find(its[i]).inf, its[i].inf) != 0) printf("wrong\n");
    #endif
    }
    #ifdef TEST_INT_MODE
    std::sort(its, its + 10000);
    for (int i = 0;i < 10000;i++)
        std::cout << test.find(its[i]) << std::endl;
    vector<pair<test_t, value_t>> v = test.listof(test_t(15, 0), judgement);
    for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    #endif
    std::cout << std::endl << ::GetTickCount() - k << std::endl;
    return 0;
}
