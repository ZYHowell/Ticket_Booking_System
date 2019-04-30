#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP
template<size_t buffer_size>
class bufferpool{
    using byte = char;
    struct node{
        node *next, *prior;
        byte content[buffer_size];
        node(int l1 = 0, int l2 = 0):next(n), prior(p), content(nullptr){}
    };
};
#endif