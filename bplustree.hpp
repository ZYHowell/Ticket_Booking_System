//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 a cache
//version 0.0.1 builds a structure and solves details later. all datas are pretended to be located in memories
//version 0.0.2 makes insertion in memories avaliable
//version 0.1.1 uses file to read and write. Now
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include "exceptions.h"
typedef void* loc;
template<class key_type, class value_type, size_t part_size, class Compare = std::less<key_type>>
class bplustree{
    struct node{
        key_type key;
        loc head, tail;
        loc prior, next, parent;
        size_t size;
        bool type;                      //0 for a leaf and 1 otherwise
        node(key_type k = key_type(),
        loc par = nullptr, loc h = nullptr, loc t = nullptr, loc pre = nullptr, loc nex = nullptr, 
        size_t s = 1, bool type = 0)
        :key(k),parent(par),head(h),tail(t),prior(pre),next(nex),size(s),type(ty){}
    };
    node *root_first;
    Compare com;
    size_t num;
    FILE *fp;

    loc find(key_type k){
    //find the one smaller than k but the next is larger
        loc now = root_first->head;
        if (now == nullptr) return now;
        while(now->type || !com(k, now->key)){
            if (!com(k, now->key)) now = (node *)(now->next);
            else now = (node *)((node *)now->prior)->child;
        }
    //break when size = 1 and k < now->key, i.e., k is added between the prior of now and now
        return now->prior;
    }
    void split(node *now){
        size_t s = now->size / 2;
        node *temp = new node(key_type(),now->parent, nullptr, now->tail, now, now->next, s, now->type);
    //in fact it can be a new in buffer instead of directly using new operator. maybe a placement new?
        node *tmp = now->tail;
        for (size_t i = 0;i < s;i++, tmp = tmp->prior) tmp->parent = temp;
        temp->head = tmp->next;
        now->tail = tmp;
        now->next->prior = tmp;
        now->next = tmp;
        tmp->next = nullptr, temp->head->prior = nullptr;
        if (now->parent != nullptr){
            now->parent->size++;
            if (now->parent->size > part_size) split(now->parent);
        }
        else{
            root_first = now->parent = new node(key_type(), nullptr, now, temp);
            root_first->size = 2, root_first->type = 1;
        }
    }
    void merge(node *now){
    //merge now and the next. the corretness is considered before using this function
        node *nex = now->next;
        nex->head->prior = now->tail;
        if (nex->parent->tail == nex) nex->parent->tail = now;
        --(now->parent->size);
        now->size += nex->size;
        for (auto i = nex->head;i != nex->tail;i = i->next) i->parnet = now;
        nex->tail->parnet = now;
        if (now->size > part_size) split(now);
        if (now->parent != root_first && now->parent->size < part_size / 2) merge(now);
    }
public:
    bplustree():root_first(nullptr),num(0){}
    bool insert(key_type k,value_type v){
        node *now = (node *)find(k);
        if (!(com(now->key, k) || com(k, now->key))) return 0;
        node *temp = new node(k, now->parent, nullptr, nullptr, now, now->next, 1, 1);
        temp->head = temp->tail = new value_type(v);
        if (now->next != nullptr) now->next->prior = temp;
        now->next = temp;
        if (++(now->parent->size) > part_size) split(now->parent);
    }
    bool remove(key_type k){
        node *now = (node *)find(k);
        if (!(com(now->key, k) || com(k,now->key))) throw(runtime_error());
        return del(now);
    }
    value_type search(key_type k){
        loc l = find(k);
        if (l != nullptr)
            return *(value_type *)l;
        else throw(runtime_error());
    }
};
#endif