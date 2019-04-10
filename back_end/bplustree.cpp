//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 the alloc function
//an alloc file for secondary storage is needed and can be greatly improved, writing at last
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <stdio.h>
#include "exceptions.h"
using pointer   =   long;
using byte      =   char;
template<class key_type,
         class value_type, 
         size_t part_size, 
         class Compare = std::less<key_type>
>   class bplustree{
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
    /*
        * Load the info of node p and its brothers into the cache. 
        * The info of node p contains those keys greater than p.key but smaller than p.next.key
    */
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
        OOOOOP! mention that if the inserted (k, v) has a key smaller than any exist key, it should be judged at first
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
    /*
        * find the pointer of database file locating the value of key k
    */
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
    
    /*
        * make the first of p become the last of l while the size of p equals to pars_size and that of l is less. 
        * save them both
    */
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
    /*
        * Make the last of p become the first of r while the size of p equals to part_size and that of r is less. 
        * Save them both
    */
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
    /*
        * Receive a node from the left part if avaliable
        * Save them both
    */
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
    /*
        * Receive a node from the right part if avaliable. 
        * Save them both
    */
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
    /*
        * Find the quickest way to solve the problem of the size of a node, 
        * mode = 0 when the size is too big and 1 otherwise. 
        * Save p and its brother, save parent?
    */
    void consider(node &p, bool mode, node &par){
        node tmp;
        int tried = 0;
        byte *cache;
        load_cache(cache, p);
        while (tried < 2){
            if (!tried){
                if (p.prior == nullptr) ++tried;
                else{
                    tmp = load_node(p.prior);
                    if (mode){
                        if (tmp.size > part_size / 2){
                            receive_left(p, tmp, cache);
                            break;
                        }
                        else ++tried;
                    }
                    else if (tmp.size < part_size) {
                        left_balance(p, tmp, cache);
                        break;
                    }
                    else ++tried;
                }
            }
            else{
                if (p.next == nullptr) {
                    if (mode){
                        if (p.prior == nullptr){
                            tmp = load_node(p.parent);
                            if (tmp.parent != nullptr) consider(tmp, 1, load_node(tmp.parent));
                            //shall we save node p there?
                            break;
                        }
                        else{
                            byte *cache_tmp;
                            load_cache(cache_tmp, tmp);
                            merge(tmp, cache_tmp, cache, par);
                            break;
                        }
                    }
                    else{
                        split(p, cache);
                        ++par.size;
                        break;
                    }
                }
                else{
                    tmp = load_node(p.next);
                    if (mode){
                        byte *cache_tmp;
                        load_cache(cache_tmp, tmp);
                        merge(p, cache, cache_tmp);
                        break;
                    }
                    else if (tmp.size < part_size){
                        right_balance(p, tmp, cache);
                        break;
                    }
                    else {
                        split(p,cache);
                        ++par.size;
                        break;
                    }
                }
            }
        }
    }
    //waiting to complete
    void split(node &now, byte *cache){
        size_t s = now.size / 2;
        now.size -= s;
        node tmp(key_value(), nullptr, now.parent, now, now.next, now.size / 2, now->type);
        if (now.next != nullptr){
            node temp = load_node(now.next);
            temp.prior = tmp;
            save_node(temp);
        }
        alloc_a_pointe_as_the_position_of_tmp_named_pos
        tmp.pos = pos;
        byte *cache_tmp;
        size_t ns = now.size;
        for (size_t i = 0;i < s;i++){
            *nth_element_key(cache_tmp, i)      = *nth_element_key(cache, ss + i);
            *nth_element_pointer(cache_tmp, i)  = *nth_element_pointer(cache, ss + i);
            //or maybe it needs to add "+1"?
        }
        save_cache(cache_tmp, tmp);
        save_cache(cache, now);
        node p = load_node(now.parent);
        ++p.size;
        if (p.size >= part_size){
            if (p.parent == nullptr){
                locate_a_new_node_as_the_root
                p.parent = root;
            }
            consider(p, 0, load_node(p.parent));
        }
        else save_node(p);
    }
    //merge now and the next of it, if the result is too big, use split automatically
    void merge(node &now, byte *cache_a, byte *cache_b, node &par){
        node tmp = load_node(now.next);
        now.next = tmp.next;
        if (tmp.next != nullptr){
            node temp = load_node(tmp.next);
            temp.prior = now.pos;
            save_node(temp);
        }
        size_t s = now.size;
        for (size_t i = 0;i < tmp.size;i++){
            *nth_element_key(cache_a, s + i)        =   *nth_element_key(cache_b, i);
            *nth_element_pointer(cache_a, s + i)    =   *nth_element_pointer(cache_b, i);
        }
        now.size += tmp.size;
        if (now.size >= part_size) split(now, cache_a);
        else --par.size;
        save_cache(cache_a, now);
    }
    /*
        * Insert (k,v) and return true if it is an insertion and false for a change, 
        * whether split or not is considered in its parent,
        * as we prove that all parts are smaller than part_size, the memory is safe.
        * But mention to judge the size of the root.(actually, it does need to be specially treated)
    */
    bool _insert(node &p, const key_type &k, const value_type &v){
        byte *cache;load_cache(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        pointer loc = nth_element_pointer(cache, ord);
        if (p.type){
            node child_node = load_node(loc);
            //w...it will not actually clear the cache there, but clear it might be a good idea?
            //nonsense, the memory is not tested and will not be tooooo big.
            bool ret = _insert(child_node, k, v);
            if (child_node.size >= part_size) {
                consider(child_node, 0, p);
                save_node(p);//do we need this?
            }
            return ret;
        }
        else{
            if (equal(nth_element_key(p, ord), k)) {
                pointer value_pointer = *nth_element_pointer(p, ord);
                use_this_to_locate_the_value_in_the_database_file_and_change_it
                return false;
            }
            alloc_new_memory_in_database_file_and_return_its_postion_pos_which_is_a_pointer
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
    bool _remove(node &p, const key_type &k){
        byte *cache;
        load_cache(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        if (p.type){
            pointer loc = nth_element_pointer(cache, ord);
            node child_node = load_node(loc);
            bool ret = _remove(child_node, k);
            if (child_node.size < part_size / 2) {
                consider(child_node, 1, p);
                save_node(p);
            }
            return ret;
        }
        else{
            if (*nth_element_key(cache, ord) == k){
                delete_value_in_database_file
                --p.size;
                for (size_t i = ord - 1;i < p.size;i++){
                    *nth_element_key(cache, i)      = *nth_element_key(cache, i + 1);
                    *nth_element_pointer(cache, i)  = *nth_element_pointer(cache, i + 1);
                }
                save_cache(cache, p);
                p.key = nth_element_key(cache, 0);
                return true;
            }
            else return false;
            //or throw something?
        }
    }
public:
    bplustree(){}
    value_type& find(const key_type &k){

    }
    bool insert(key_type k,value_type v){
        if (com(min_key, k)) _insert(root_node, k, v);
        else waiting_to_complete
        if (root_node.size >= part_size){
            alloc_a_new_node_and_return_its_position_pos
            root_node.parent = pos;
            node now_root = load_node(pos);
            
        }
    }
    bool remove(key_type k){
        
    }

};
#endif