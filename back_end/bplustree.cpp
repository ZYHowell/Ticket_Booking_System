//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 improvement about memories
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
using byte      =   char;
template<class key_type, class value_type, size_t part_size, class Compare = std::less<key_type>>
class bplustree{
    struct node{
        key_type key;
        pointer prior, next, parent;
        pointer pos;
        size_t size;                    //the size of its brothers
        bool type;                      //0 for a leaf and 1 otherwise
        node(key_type k = key_type(), pointer p = nullptr, pointer par = nullptr, 
        pointer pre = nullptr, pointer nex = nullptr, 
        size_t s = 1, bool ty = 0)
        :key(k),pos(p),parent(par),prior(pre),next(nex),size(s),type(ty){}
    };
    pointer root;
    Compare com;
    size_t num;
    FILE *datafile, *bptfile;

    inline bool equal(const key_type& k1,const key_type& k2){
        return !(com(k1, k2) || com(k2, k1));
    }

        // inline pointer_to(FILE *f, pointer l)
        // {
        //     fseek(f, l, SEEK_SET);
        // }
    //load the info of node p and its brothers into the cache
    void load_cache(byte *start, node& p){
        fseek(bptfile, p.pos, SEEK_SET);
        fread(start, sizeof(node), p.size, bptfile);
    }
    //save the info in the cache
    void save_cache(byte *start, node &p){
        fseek(bptfile, p.pos, SEEK_SET);
        fwrite(start, sizeof(node), p.size, bptfile);
    }
    //get the value from the file with pointer l
    value_type& load_value(pointer l){
        fseek(datafile, l, SEEK_SET);
        value_type* tmp;
        fread(tmp, sizeof(value_type), 1, datafile);
        return *tmp;
    }
    //get the whole info of a node from the file with pointer l
    node& load_node(pointer l){
        fseek(bptfile, l, SEEK_SET);
        node tmp;
        fread(&tmp, sizeof(node), 1, bptfile);
        return tmp;
    }
    //write the whole info of a node to the file
    bool save_node(node &p){
        try{
            fseek(bptfile, p.pos, SEEK_SET);
            fwrite(&p, sizeof(node), 1, bptfile);
            return true;
        }
        catch(...){
            return false;
        }
    }
    //return the key of the number n element in the buffer, 0-base
    inline key_type* nth_element_key(byte *start, size_t n){
        return (key_type *)(start + (sizeof(key_type) + sizeof(pointer)) * n);
    }
    //return the pointer of the number n element in the buffer, 0-base
    inline pointer* nth_element_pointer(byte *start, size_t n){
        return (pointer *)(start + sizeof(key_type) * (n + 1) + sizeof(pointer) * n);
    }
    /*  
        use binary search to find the one smaller than or equal to k but the next is greater,
        mention that the key must be greater than or equal to the first key in the compared set.
        there are n elements in the compared set in total.
    */
    inline size_t binary_search_key(byte *start,const key_type& k, size_t n){
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) / 2;
            if ( cmp(*nth_element_key(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        return l;
    }
    //find the pointer to k, or maybe just find the leaf part of it?
    pointer _find(node &p,const key_type& k){
        byte *cache;
        load_cache(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        pointer* tmp = nth_element_pointer(cache, ord);
        if (!p.type){
            if (equal(nth_element_key(cache, ord), k)) return nth_element_pointer(cache, ord);
            else return nullptr;
        }
        return _find(load_node(tmp), k);
    }
    
    //make the first of p become the last of l while the size of p equals to pars_size and that of l is less
    void left_balance(const node &p, const node &l, byte *p_cache){
        byte *left_cache;
        load_cache(left_cache, l);
        *nth_element_key(left_cache, l.size) = *nth_element_key(p_cache, 0);
        *nth_element_pointer(left_cache, l.size) = *nth_element_pointer(p_cache, 0);
        ++l.size, --p.size;
        for (size_t i = 0; i < p.size;i++){
            *nth_element_key(p_cache, i) = *nth_element_key(p_cache, i + 1);
            *nth_element_pointer(p_cache, i) = *nth_element_pointer(p_cache, i + 1);
        }
        // if (l.size >= part_size)
        // if (l.prior){
        //     node tmp = load_node(l.next);
        //     if (tmp.size < part_size) left_balance(l,tmp,left_cache);
        // }
        save_cache(left_cache, l);
        save_cache(p_cache, p);
    }
    //make the last of p become the first of r while the size of p equals to part_size and that of r is less
    void right_balance(node &p, node &r, byte *p_cache){
        byte *right_cache;
        load_cache(right_cache, r);
        ++r.size;
        for (size_t i = p.size;i > 0;i++){
            *nth_element_key(right_cache, i) = *nth_element_key(right_cache, i - 1);
            *nth_element_pointer(right_cache, i) = *nth_element_pointer(right_cache, i - 1);
        }
        --p.size;
        *nth_element_key(right_cache, 0) = *nth_element_key(p_cache, p.size);
        *nth_element_pointer(right_cache, 0) = *nth_element_pointer(p_cache, p.size);
        // if (r.size >= part_size)
        // if (r.next){
        //     node tmp = load_node(r.next);
        //     if (tmp.size < part_size) right_balance(r, tmp);
        // }
        save_cache(right_cache, r);
        save_cache(p_cache, p);
    }
    //receive a node from the left part if avaliable
    void receive_left(node &p, node &l, byte *p_cache){
        byte *left_cache;
        load_cache(left_cache, r);
        ++r.size;
        for (size_t i = p.size;i > 0;i++){
            *nth_element_key(right_cache, i) = *nth_element_key(right_cache, i - 1);
            *nth_element_pointer(right_cache, i) = *nth_element_pointer(right_cache, i - 1);
        }
        --p.size;
        *nth_element_key(right_cache, 0) = *nth_element_key(p_cache, p.size);
        *nth_element_pointer(right_cache, 0) = *nth_element_pointer(p_cache, p.size);
        // if (r.size >= part_size)
        // if (r.next){
        //     node tmp = load_node(r.next);
        //     if (tmp.size < part_size) right_balance(r, tmp);
        // }
        save_cache(right_cache, r);
        save_cache(p_cache, p);
    }
    //receive a node from the right part if avaliable
    void receive_right(node &p, node &r, byte *p_cache){
        byte *right_cache;
        load_cache(right_cache, r);
        *nth_element_key(p_cache, p.size) = *nth_element_key(right_cache, 0);
        *nth_element_pointer(p_cache, p.size) = *nth_element_pointer(right_cache, 0);
        ++p.size, --r.size;
        for (size_t i = 0; i < right.size;i++){
            *nth_element_key(right_cache, i) = *nth_element_key(right_cache, i + 1);
            *nth_element_pointer(right_cache, i) = *nth_element_pointer(right_cache, i + 1);
        }
        // if (l.size >= part_size)
        // if (l.prior){
        //     node tmp = load_node(l.next);
        //     if (tmp.size < part_size) left_balance(l,tmp,left_cache);
        // }
        save_cache(left_cache, l);
        save_cache(p_cache, p);
    }
    //find the quickest way to solve the problem of the size of a node, save before break.
    //mode = 0 when the size is too big and 1 otherwise
    void consider(node &p, bool mode){
        node tmp;
        int tried = 0;
        byte *cache;
        load_cache(cache, p);
        while (tried < 2){
            if (!tried){
                if (p.prior == nullptr) ++tried;
                else{
                    tmp = load_node(p.prior);
                    if (tmp.size < part_size) {
                        left_balance(p, tmp, cache);
                        break;
                    }
                    else ++tried;
                }
            }
            else{
                if (p.next == nullptr) {split(p, cache);break;}
                else{
                    tmp = load_node(p.next);
                    if (tmp.size < part_size){
                        right_balance(p, tmp, cache);
                        break;
                    }
                    else {split(p,cache);break}
                }
            }
        }
    }
    //waiting to complete
    void split(node &now, byte *cache){
        node tmp(key_value(), nullptr, now.parent, now, now.next, 1, now->type);


        node p = load_node(now.parent);
        ++p.size;
        if (p.size >= part_size) consider(p);
        else save_node(p);
    }
    //merge now and the next of it, the correctness must be judged before using it.
    void merge(node &now){
        node tmp = load_node(now.next);
        now.next = tmp.next;
        if (tmp.next != nullptr){
            node temp = load_node(tmp.next);
            temp.prior = now.pos;
            save_node(temp);
        }
        byte *cache_a, *cache_b;
        size_t s = now.size;
        load_cache(cache_a, now), load_cache(cache_b, tmp);
        for (size_t i = 0;i < tmp.size;i++){
            *nth_element_key(cache_a, s + i)        =   *nth_element_key(cache_b, i);
            *nth_element_pointer(cache_a, s + i)    =   *nth_element_pointer(cache_b, i);
        }
        now.size += tmp.size;
        save_cache(cache_a, now);
    }
    //insert (k,v) and return true if it is an insertion and false for a change
    //whether split or not is considered in its parent,
    //as we prove that all parts are smaller than part_size, the memory is save.
    //but mention to judge the size of the root.(actually, it does need to be specially treated)
    bool _insert(node &p, const key_type &k, const value_type &v){
        size_t ord = binary_search_key(p, k);
        pointer loc = nth_element_pointer(p, ord);
        if (p.type){
            node child_node = load_node(loc);
            bool ret = _insert(child_node, k, v);
            if (child_node.size >= part_size) {
                consider(child_node, 0);
                ++p.size;
            }
            return ret;
        }
        else{
            if (equal(nth_element_key(p, ord), k)) {
                change_the_value_in_value_file
                return false;
            }
            alloc_new_memory_in_data_file_and_return_its_postion_pos_which_is_a_pointer
            byte *cache;
            load_cache(cache, p);
            for (size_t i = p.size;i > ord + 1;i++){
                *nth_element_key(p, i) = *nth_element_key(p, i - 1);
                *nth_element_pointer(p, i) = *nth_element_pointer(p, i - 1);
            }
            ++p.size;
            *nth_element_key(p, ord + 1) = k;
            *nth_element_pointer(p, ord + 1) = pos;
            save_cache(cache, p);
            //w......if it is oversized, this save function is surplus.
            p.key = nth_element_key(p, 0);
            return true;
        }
    }
    bool _remove(const key_type &k){

    }
public:
    bplustree():root(),num(0){}
    value_type& find(const key_type &k){

    }
    bool insert(key_type k,value_type v){
        
    }
    bool remove(key_type k){
        
    }

};
#endif