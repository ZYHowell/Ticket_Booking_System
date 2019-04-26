#include "bplustree.hpp"
#include <cstring>
#include <algorithm>
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
struct test_str{
    int a,b;
    test_str(int aa = 0, int bb = 0):a(aa),b(bb){}
};
bool judgement(test_str a, test_str b){
    return a.a < b.a;
}
bool operator<(test_str a, test_str b){
    if (a.a != b.a) return a.a < b.a;
    return a.b < b.b;
}
int main(){
    bplustree <test_str, int, 4> test;
    test.init("data.txt", "alloc.txt");
    test.clear();
    int its[10000];int con = 0;
    for (int i = 0;i < 100;i++){
        its[i] = randint() % 50;
        test.insert(test_str(its[i], con), its[i] * 2);
        printf("%d %d; ", its[i], con++);
    }
    // for (int i = 0;i < 10000;i++){
    //     printf("should be: %d ", its[i] * 2);
    //     printf("it is: %d\n", test.find(its[i]));
    // }
    std::sort(its, its + 100);
    for (int i = 0;i < 100;i++) printf("%d ", its[i]);
    vector<pair<test_str, int>> v = test.listof(test_str(15, 0), judgement);
    for (int i = 0;i < v.size();i++) printf("%d %d %d; ", v[i].first.a, v[i].first.b, v[i].second);
    return 0;
}