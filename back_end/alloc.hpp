#ifndef SJTU_ALLOC_HPP
#define SJTU_ALLOC_HPP
using pointer = long
class ALLOC{
    struct node{
        node *nex, *prior;
        pointer loc[2];
        int type
        node(int l1 = 0, int l2 = 65536, node *n = nullptr, node *p = nullptr):nex(n), prior(p), loc(l1, l2){}
    };
    
public:
    ALLOC(){}
};
#endif