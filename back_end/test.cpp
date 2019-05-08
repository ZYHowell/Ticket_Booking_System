#include <cstring>
#include <algorithm>
#include <iostream>
#include <random>
//#define OUTPUT_INIT
//#define DEBUG_MODE
#define TEST_INT_MODE
#include "bplustree.hpp"
#include <Windows.h>
const int maxn = 100000 + 5;
const int test_size = 1000;

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
bool comp(const test_t &a, const test_t &b){
    if (a.b != b.b) return a.b < b.b;
    return a.a < b.a;
}
#endif
#ifdef TEST_STRING_MODE
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
#ifdef TEST_EASY_MODE
const unsigned Ta = 33333331, Tb = 1 << 16 | 3;
unsigned Tc;
inline unsigned randint(){
    return Tc = (Tc * Ta + Tb);
}
using test_t = long long;
using value_t = long long;
#endif
test_t its[maxn];
int main(){
    DWORD k = ::GetTickCount();
	FILE *file = fopen("data.txt", "wb");fclose(file);
	file = fopen("alloc.txt", "wb");fclose(file);
    srand(214748364);
    bplustree <test_t, value_t, 82> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
#ifdef DEBUG_MODE
	printf("\n\n");
#endif
    for (int i = 1;i <= test_size;i++){
#ifdef TEST_INT_MODE
        its[i] = test_t(randint(), i);
    }
    std::sort(its + 1, its + test_size + 1);
    for (int i = 1; i <= test_size;i++) {
        its[i].a = i;
    #ifdef DEBUG_MODE
        printf("%d %d; ", its[i].a, its[i].b);
    #endif
    }
    std::sort(its + 1, its + test_size + 1, comp);
    #ifdef DEBUG_MODE
    printf("\n");
    for (int i = 1;i <= test_size;i++) printf("%d %d; ", its[i].a, its[i].b);
    printf("\n");
    #endif
    for (int i = 1;i <= test_size;i++) test.insert(its[i], i);
#endif
    #ifdef TEST_STRING_MODE
        its[i] = randstr();
        test.insert(its[i], its[i]);
    }
    #endif
    #ifdef TEST_EASY_MODE
        its[i] = i;//randint();
        test.insert(its[i], i);
        printf("%lld;",its[i]);
    }
    #endif
    //printf("remove now\n");
    for (int i = 1; i < test_size / 2;i++)
    #ifdef TEST_EASY_MODE
        test.remove(its[i]);
    #endif
    #ifdef TEST_INT_MODE
    {
        #ifdef DEBUG_MODE
        printf("\n\n");
        printf("remove: %d %d\n", its[i].a, its[i].b);
        #endif
        test.remove(its[i]);
        if (test.find(its[i]).second) printf("wrong_have %d;\n", i);
    } 
    #endif
    //printf("find_now\n");
    for (int i = 1;i <= test_size;i++){
    #ifdef TEST_INT_MODE
        //printf("%d ",its[i].a);
        //printf("\n");
        if (test.find(its[i]).second == i) 
            if (i < test_size / 2) printf("wrong_have %d %d;\n", its[i].a, its[i].b);
            else;
        else if (i > test_size / 2) printf("wrong_not_have %d %d;\n", its[i].a, its[i].b);
    #endif
    #ifdef TEST_STRING_MODE
		if(strcmp(test.find(its[i]).second.inf, its[i].inf) != 0) printf("wrong\n");
    #endif
    #ifdef TEST_EASY_MODE
        if (test.find(its[i]).second == i) 
            if (i < test_size / 2) printf("wrong\n");
            else;
        else if (i > test_size / 2) printf("wrong\n");
    #endif
    }
    #ifdef TEST_EASY_MODE
    vector<pair<test_t, value_t>> v = test.listof(test_t(15, 0), judgement);
    for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    #endif
    std::cout << std::endl << ::GetTickCount() - k;
    printf("\n");
    return 0;
}