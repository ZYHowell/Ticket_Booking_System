//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 ??
//version 0.0.1 now
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
template<class key_type, class value_type, size_t part_size, class Compare = std::less<key_type>>
class bplustree{
//a node contains relations, the key as well as 
//the child containing what is greater than the prior key but smaller than this key.
//as a leaf, the child of the node pointing to the actual value.
    struct node{
        key_type key;
        void *child;
        node *prior, *next, *parent;
        size_t size;
        node(key_type k = key_type(),
        void *par = nullptr, void *ch = nullptr,void *pre = nullptr, void *nex = nullptr,size_t s = 0)
        :key(k),parent(par),child(ch),prior(pre),next(nex),size(s){}
    };
    node *root_first;
    Compare com;
    size_t num;
    
    void *find(key_type k){
        node *now = root_first;
        while(now->size > 1 || !com(k, now->key)){
            if (!com(k, now->key)) now = now->next;
            else now = (node *)now->prior->child;
        }
        return nullptr;
    }
    bool del(node *it){
        return true;
    }
    void merge(node *now){}
public:
    bplustree():root_first(nullptr),num(0){}
    bool insert(key_type k,value_type v){
        node *now = (node *)find(k);
        //break when size = 1 and k < now->key, i.e., k is added between the prior of now and now
    }
    bool remove(key_type k){
        node *now = (node *)find(k);
        return del(now);
    }
    value_type search(key_type k){
        return *(T *)find(k);
    }
};
#endif