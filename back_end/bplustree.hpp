//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 a cache
//version 0.0.1 builds a structure and solves details later. all datas are pretended to be located in memories
//version 0.0.2 makes insertion in memories avaliable
//version 0.1.1 change the data structure in order to fit the file system. Now
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <stdio.h>
#include "exceptions.h"
using loc   =   long;
using byte  =   char;
template<class key_type, class value_type, size_t part_size, class Compare = std::less<key_type>>
class bplustree
{
    struct node
    {
        key_type key;
        loc head, tail;
        loc prior, next, parent;
        loc pos;
        size_t size;                    //the size of its brothers
        bool type;                      //0 for a leaf and 1 otherwise
        node(key_type k = key_type(),loc p = nullptr
        loc par = nullptr, loc h = nullptr, loc t = nullptr, loc pre = nullptr, loc nex = nullptr, 
        size_t s = 0, bool type = 0)
        :key(k),pos(p),parent(par),head(h),tail(t),prior(pre),next(nex),size(s),type(ty){}
    };
    loc root;
    Compare com;
    size_t num;
    FILE *fp;

    inline bool equal(const key_type& k1,const key_type& k2){
        return !(com(k1, k2) || com(k2, k1));
    }
//return info of the number n element, from 0 to n
    void load(byte *start, node& p)
    {}
    void clear(byte *start, node &p)
    {}
    value_type& get_value(loc l)
    {}
    node& get_node(loc l)
    {}


    inline key_type* nth_element_key(byte *start, size_t n)
    {
        return (key_type *)(start + (sizeof(key_type) + sizeof(loc)) * n);
    }
    inline loc* nth_element_loc(byte *start, size_t n)
    {
        return (loc *)(start + sizeof(key_type) * (n + 1) + sizeof(loc) * n);
    }
    inline size_t binary_search_key(byte *start,const key_type& k, size_t n)
    {
    //find the one smaller than k but the next is larger, if k is the smallest,
    //simply return the head waiting the extern function to judge
    //there are n + 1 elements in total, which are 0 to n
    //return the pointer to the contains instead of that to the key
        size_t l = 0, r = n, mid;
        while (r > l)
        {
            mid = (l + r) / 2;
            if (cmp(*nth_element_key(start, mid), k)) l = mid + 1;
            else r = mid;
        }
        return l;
    }
    value_type& find(node &p,const key_type& k)
    {
        byte *cache;
        load(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        loc* tmp = nth_element_loc(cache, ord);
        if (!p.type){
            clear(cache, p);
            if (equal(nth_element_key(cache, ord), k)) return get_value(tmp);
            else return value_type();
        }
        clear(cache, p);
        return find(get_node(loc), k);
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

};
#endif