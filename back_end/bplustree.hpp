//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 a cache
//version 0.0.1 builds a structure and solves details later. all datas are pretended to be in memories
//version 0.0.2 makes insertion in memories avaliable
//version 0.1.1 change the data structure in order to fit the file system. Now
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <stdio.h>
#include "exceptions.h"
using pointer   =   long;
using byte  =   char;
template<class key_type, class value_type, size_t part_size, class Compare = std::less<key_type>>
class bplustree
{
    struct node
    {
        key_type key;
        pointer head, tail;
        pointer prior, next, parent;    //maybe tail and parent can be unnecessary?
        pointer pos;
        size_t size;                    //the size of its brothers
        bool type;                      //0 for a leaf and 1 otherwise
        node(key_type k = key_type(),pointer p = nullptr, pointer par = nullptr, 
        pointer h = nullptr, pointer t = nullptr, pointer pre = nullptr, pointer nex = nullptr, 
        size_t s = 0, bool type = 0)
        :key(k),pos(p),parent(par),head(h),tail(t),prior(pre),next(nex),size(s),type(ty){}
    };
    pointer root;
    Compare com;
    size_t num;
    FILE *datafile, *bptfile;

    inline bool equal(const key_type& k1,const key_type& k2)
    {
        return !(com(k1, k2) || com(k2, k1));
    }

    // inline pointer_to(FILE *f, pointer l)
    // {
    //     fseek(f, l, SEEK_SET);
    // }
    //load the info of node p and its brothers into the cache
    void load_cache(byte *start, node& p)
    {
        fseek(bptfile, p.pos, SEEK_SET);
        fread(start, sizeof(node), p.size, bptfile);
    }
    //save the info in the cache
    void save_cache(byte *start, node &p)
    {
        fseek(bptfile, p.pos, SEEK_SET);
        fwrite(start, sizeof(node), p.size, bptfile);
    }
    //get the value from the file with pointer l
    value_type& get_value(pointer l)
    {
        fseek(datafile, l, SEEK_SET);
        value_type* tmp;
        fread(tmp, sizeof(value_type), 1, datafile);
        return *tmp;
    }
    //get the whole info of a node from the file with pointer l
    node& get_node(pointer l)
    {
        fseek(bptfile, l, SEEK_SET);
        node tmp;
        fread(&tmp, sizeof(node), 1, bptfile);
        return tmp;
    }

    //return the key of the number n element in the buffer, 0-base
    inline key_type* nth_element_key(byte *start, size_t n)
    {
        return (key_type *)(start + (sizeof(key_type) + sizeof(pointer)) * n);
    }
    //return the pointer of the number n element in the buffer, 0-base
    inline pointer* nth_element_pointer(byte *start, size_t n)
    {
        return (pointer *)(start + sizeof(key_type) * (n + 1) + sizeof(pointer) * n);
    }
    /*  
        use binary search to find the one smaller than or equal to k but the next is greater,
        mention that the key must be greater than or equal to the first key in the compared set.
        there are n elements in the compared set in total.
    */
    inline size_t binary_search_key(byte *start,const key_type& k, size_t n)
    {
        size_t l = 0, r = n, mid;
        while (l < r)
        {
            mid = (l + r) / 2;
            if ( cmp(*nth_element_key(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        return l;
    }
    //find the value attached to k, or maybe just return the pointer of this value greater to use more?
    pointer _find(node &p,const key_type& k)
    {
        byte *cache;
        load(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        _FUNCTION_POINTER_DEDUCTION_GUIDE* tmp = nth_element_pointer(cache, ord);
        if (!p.type){
            clear(cache, p);
            if (equal(nth_element_key(cache, ord), k)) return nth_element_pointer(cache, ord);
            else return nullptr;
        }
        return _find(get_node(pointer), k);
    }
    
    //make the first of p become the last of l while the size of p equals to pars_size and that of l is less
    void left_balance(const node &p, const node &l, byte *p_cache)
    {
        byte *left_cache;
        load_cache(left_cache, l);
        *nth_element_key(left_cache, l.size) = *nth_element_key(p_cache, 0);
        *nth_element_pointer(left_cache, l.size) = *nth_element_pointer(p_cache, 0);
        ++l.size, --p.size;
        for (size_t i = 0; i < p.size;i++)
        {
            *nth_element_key(p_cache, i) = *nth_element_key(p_cache, i + 1);
            *nth_element_pointer(p_cache, i) = *nth_element_pointer(p_cache, i + 1);
        }
        if (l.size >= part_size)
        if (l.prior){
            node tmp = get_node(l.next);
            if (tmp.size < part_size) left_balance(l,tmp,left_cache);
        }
        save_cache(left_cache, l);
        //save_cache(p_cache, p);
    }
    //make the last of p become the first of r while the size of p equals to part_size and that of r is less
    void right_balance(node &p, node &r, byte *p_cache)
    {
        byte *right_cache;
        load_cache(right_cache, r);
        ++r.size;
        for (size_t i = p.size;i > 0;i++)
        {
            *nth_element_key(right_cache, i) = *nth_element_key(right_cache, i - 1);
            *nth_element_pointer(right_cache, i) = *nth_element_pointer(right_cache, i - 1);
        }
        --p.size;
        *nth_element_key(right_cache, 0) = *nth_element_key(p_cache, p.size);
        *nth_element_pointer(right_cache, 0) = *nth_element_pointer(p_cache, p.size);
        if (r.size >= part_size)
        if (r.next){
            node tmp = get_node(r.next);
            if (tmp.size < part_size) right_balance(r, tmp);
        }
        save_cache(right_cache, r);
        //save_cache(p_cache, p);
    }
    //receive a node from the left part if avaliable
    void receive_left()
    {

    }
    //receive a node from the right part if avaliable
    void receive_right()
    {

    }
    void split(node &now)
    {
        
    }
    //merge now and the next of it
    void merge(node &now){
    
    }
    node _insert(const key_type &k, const value_type &v)
    {

    }
    bool _remove(const key_type &k)
    {
        
    }
public:
    bplustree():root(),num(0)
    {}
    value_type& find(const key_type &k)
    {

    }
    bool insert(key_type k,value_type v)
    {
        
    }
    bool remove(key_type k)
    {
        
    }

};
#endif