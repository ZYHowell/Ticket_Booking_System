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

    inline bool equal(const key_type& k1,const key_type& k2)
    {
        return !(com(k1, k2) || com(k2, k1));
    }

    inline pointer_to(FILE *f, loc l)
    {
        fseek(f, l, SEEK_SET);
    }
    //load the info of node p and its brothers into the buffer
    void load(byte *start, node& p)
    {}
    //clear the buffer
    void clear(byte *start, node &p)
    {}
    //get the value from the file with location l
    value_type& get_value(loc l)
    {}
    //get the whole info of a node from the file with location l
    node& get_node(loc l)
    {}

    //return the key of the number n element in the buffer, 0-base
    inline key_type* nth_element_key(byte *start, size_t n)
    {
        return (key_type *)(start + (sizeof(key_type) + sizeof(loc)) * n);
    }
    //return the loc of the number n element in the buffer, 0-base
    inline loc* nth_element_loc(byte *start, size_t n)
    {
        return (loc *)(start + sizeof(key_type) * (n + 1) + sizeof(loc) * n);
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
    //find the value attached to k, or maybe just return the loc of this value greater to use more?
    value_type& _find(node &p,const key_type& k)
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
        return _find(get_node(loc), k);
    }

    node _insert(const key_type &k, const value_type &v)
    {

    }
    //make the first of p become the last of l while the size of p equals to pars_size and that of l is less
    void left_balance(node &p, node &l)
    {
        if (l.size >= part_size)
        if (l.prior){
            node tmp = get_node(l.next);
            if (tmp.size < part_size) left_balance(l,tmp);
        }
    }
    //make the last of p become the first of r while the size of p equals to part_size and that of r is less
    void right_balance(node &p, node &r)
    {
        if (r.size >= part_size)
        if (r.next){
            node tmp = get_node(r.next);
            if (tmp.size < part_size) right_balance(r, tmp);
        }
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