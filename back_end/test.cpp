#include <cstring>
#include <algorithm>
#include <iostream>
#include <random>
#include "bplustree.hpp"
#include <Windows.h>
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
test_t its[10005];
int main(){
	srand(214748364);
    DWORD k = ::GetTickCount();
	FILE *file = fopen("data.txt", "wb");fclose(file);
	file = fopen("alloc.txt", "wb");fclose(file);
    bplustree <test_t, test_t> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
	//printf("\n\n");
    int i = 0;
    for (i = 0;i < 10000;i++){
        its[i] = randstr();
		//printf("%d;",i);
        test.insert(its[i], its[i]);
		//printf("%s\n\n",its[i].inf);
        // printf("%d %d; ", its[i], con++);
    }
    for (int i = 0;i < 10000;i++){
		if(strcmp(test.find(its[i]).inf, its[i].inf) != 0) printf("wrong\n");
		//else printf("%s\n",test.find(its[i]).inf);
        //printf("should be: %s;", its[i].inf);
        //printf("it is: %s\n", test.find(its[i]).inf);
    }
    // std::sort(its, its + 10000);
    //for (int i = 0;i < 10000;i++) cout << test.find(its[i]) << endl;
    //vector<pair<test_str, int>> v = test.listof(test_str(15, 0), judgement);
    //for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    std::cout << std::endl << ::GetTickCount() - k << std::endl;
    return 0;
}
