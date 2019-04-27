#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <stdio.h>
#include <cstring>
#include "exceptions.hpp"
#include "alloc.hpp"
#include "vector.hpp"
const pointer invalid_p = 0xdeadbeef;
template<class key_type,
         class value_type, 
         size_t part_size, 
         class Compare = std::less<key_type>
>   class bplustree{
    using pointer   =   long;
    using byte      =   char;
    using list_type =   pair<key_type, value_type>;
    struct node{
        key_type key;
        pointer prior, next;
        pointer pos;
        size_t size;                    //the size of its brothers
        bool type;                      //0 for a leaf and 1 otherwise
        node(key_type k = key_type(),
        pointer p = invalid_p, 
        pointer pre = invalid_p, pointer nex = invalid_p, 
        size_t s = 1, bool ty = 0)
        :key(k),pos(p),prior(pre),next(nex),size(s),type(ty){}
    };
    node root;
    Compare com;
    size_t num;
    FILE *datafile;
    const size_t leaf_size, non_leaf_size;
    const size_t leaf_inf_size, non_leaf_inf_size;
    ALLOC alloc;
    pointer root_pos;
    char *index_name, *data_name;

    inline bool equal(const key_type& k1,const key_type& k2){
        return !(com(k1, k2) || com(k2, k1));
    }
    inline void load_cache_n(byte *start,const node& p){
        fseek(datafile, p.pos + sizeof(node), SEEK_SET);
        // printf("load_cache_seek: %d\n", p.pos + sizeof(node));
        fread(start, 1, (sizeof(key_type) + sizeof(pointer)) * p.size, datafile);
        // printf("which are:\n");
        // for (int i = 0;i < p.size;i++) // printf("key: %d, pointer: %d; ", *nth_key_n(start, i), *nth_pointer(start, i));
        // printf("\n");
    }
    inline void load_cache_l(byte *start,const node& p){
        fseek(datafile, p.pos + sizeof(node), SEEK_SET);
        // printf("load_cache_seek: %d\n", p.pos + sizeof(node));
        fread(start, 1, (sizeof(key_type) + sizeof(value_type)) * p.size, datafile);
        // printf("which are:\n");
        // for (int i = 0;i < p.size;i++) // printf("key: %d, value: %d; ", *nth_key_l(start, i), *nth_value(start, i));
        // printf("\n");
    }
    inline void save_cache_n(byte *start,const node &p){
        fseek(datafile, p.pos + sizeof(node), SEEK_SET);
        // printf("save_cache_seek: %d\n", p.pos + sizeof(node));
        fwrite(start, 1, (sizeof(key_type) + sizeof(pointer)) * p.size, datafile);
        // printf("which are:\n");
        // for (int i = 0;i < p.size;i++)// printf("key: %d, pointer: %d; ", *nth_key_n(start, i), *nth_pointer(start, i));
        // printf("\n");
    }
    inline void save_cache_l(byte *start,const node &p){
        fseek(datafile, p.pos + sizeof(node), SEEK_SET);
        // printf("save_cache_seek: %d\n", p.pos + sizeof(node));
        fwrite(start, 1, (sizeof(key_type) + sizeof(value_type)) * p.size, datafile);
        // printf("which are:\n");
        // for (int i = 0;i < p.size;i++)// printf("key: %d, value: %d; ", *nth_key_l(start, i), *nth_value(start, i));
        // printf("\n");
    }
    inline node load_node(pointer l){
        fseek(datafile, l, SEEK_SET);
        // printf("load_node_seek: %d\n", l);
        node tmp;
        fread(&tmp, sizeof(node), 1, datafile);
        // printf("which is: pos:%d size:%d key:%d\n", tmp.pos, tmp.size, tmp.key);
        return tmp;
    }
    inline bool save_node(const node &p){
        if (p.pos == invalid_p) return false;
        try{
            fseek(datafile, p.pos, SEEK_SET);
            // printf("save_node_seek: %d\n", p.pos);
            fwrite(&p, sizeof(node), 1, datafile);
            // printf("which is: pos:%d size:%d key:%d\n", p.pos, p.size, p.key);
            return true;
        }
        catch(...){
            return false;
        }
    }
    inline key_type* nth_key_n(byte *start, size_t n = 0){
        return (key_type *)(start + (sizeof(key_type) + sizeof(pointer)) * n);
    }
    inline key_type* nth_key_l(byte *start, size_t n = 0){
        return (key_type *)(start + (sizeof(key_type) + sizeof(value_type)) * n);
    }
    inline pointer* nth_pointer(byte *start, size_t n = 0){
        return (pointer *)(start + sizeof(key_type) * (n + 1) + sizeof(pointer) * n);
    }
    inline value_type* nth_value(byte *start, size_t n = 0){
        return (value_type *)(start + sizeof(key_type) * (n + 1) + sizeof(value_type) * n);
    }
    inline pointer nth_value_loc(const node &now, size_t n = 0){
        return now.pos + sizeof(node) + (sizeof(key_type) + sizeof(value_type)) * n + sizeof(key_type);
    }
    inline value_type get_value(pointer loc){
        value_type v;
        fseek(datafile, loc, SEEK_SET);
        fread(&v, sizeof(value_type), 1, datafile);
        return v;
    }
    inline size_t binary_search_key_n(byte *start,const key_type& k, size_t n){
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) / 2;
            if ( com(*nth_key_n(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        if (l >= n) return n - 1;
        else if (equal(*nth_key_n(start, l), k)) return l;
        else return l - 1;
    }
    inline size_t binary_search_key_l(byte *start,const key_type& k, size_t n){
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) / 2;
            if ( com(*nth_key_l(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        if (l >= n) return n - 1;
        else if (equal(*nth_key_l(start, l), k)) return l;
        else return l - 1;
    }
    //OR, SHALL WE RETURN A VALUE_TYPE?waiting
    pointer _find(const node &p,const key_type& k){
        size_t ord;
        pointer tmp;
        if (!p.type){
            byte cache[leaf_inf_size];
            load_cache_l(cache, p);
            ord = binary_search_key_l(cache, k, p.size);
            tmp = nth_value_loc(p, ord);
            if (equal(*nth_key_l(cache, ord), k)) return tmp;
            else return invalid_p;
        }
        else{
            byte cache[non_leaf_inf_size];
            load_cache_n(cache, p);
            ord = binary_search_key_n(cache, k, p.size);
            tmp = *nth_pointer(cache, ord);
            return _find(load_node(tmp), k);
        }
    }
    //waiting
    vector<list_type> _listof(const node &p, key_type k, bool (*comp)(key_type a, key_type b)){
        size_t ord;
        pointer tmp;
        if (!p.type){
            byte cache[leaf_inf_size];
            load_cache_l(cache, p);
            ord = binary_search_key_l(cache, k, p.size);
            tmp = nth_value_loc(p, ord);
            vector<list_type> ret;
            node now = p;
            while ( !comp(k, *nth_key_l(cache, ord)) ){
                if (!comp(*nth_key_l(cache, ord), k) )
                ret.push_back(
                    list_type(*nth_key_l(cache, ord), 
                        *nth_value(cache, ord)
                    )
                );
                if (ord + 1 < now.size) ++ord;
                else{
                    if (now.next == invalid_p) break;
                    now = load_node(now.next);
                    load_cache_l(cache, now);
                    ord = 0;
                }
            }
            return ret;
        }
        else {
            byte cache[non_leaf_inf_size];
            load_cache_n(cache, p);
            ord = binary_search_key_n(cache, k, p.size);
            tmp = *nth_pointer(cache, ord);
            return _listof(load_node(tmp), k, comp);
        }
    }
    /*
        * make the first of p become the last of l while the size of p equals to part_size and that of l is less. 
        * save them both
    */
    inline void left_balance_n(node &p, node &l, byte *p_cache){
        byte left_cache[non_leaf_inf_size];
        load_cache_n(left_cache, l);
        *nth_key_n(left_cache, l.size) = p.key;
        *nth_pointer(left_cache, l.size) = *nth_pointer(p_cache);
        ++l.size, --p.size;
        p.key = *nth_key_n(p_cache, 1);
        save_cache_n(left_cache, l);
        save_cache_n((byte *)nth_key_n(p_cache, 1), p);
        save_node(l), save_node(p);
    }
    inline void left_balance_l(node &p, node &l, byte *p_cache){
        byte left_cache[leaf_inf_size];
        load_cache_l(left_cache, l);
        *nth_key_l(left_cache, l.size) = p.key;
        *nth_value(left_cache, l.size) = *nth_value(p_cache);
        ++l.size, --p.size;
        p.key = *nth_key_l(p_cache, 1);
        save_cache_l(left_cache, l);
        save_cache_l((byte *)nth_key_l(p_cache, 1), p);
        save_node(l), save_node(p);
    }
    /*
        * Make the last of p become the first of r while the size of p equals to part_size and that of r is less. 
        * Save them both. do we need so?
    */
    inline void right_balance_n(node &p, node &r, byte *p_cache){
        byte right_cache[non_leaf_inf_size];
        load_cache_n(right_cache, r);
        for (size_t i = r.size;i > 0;i--){
            *nth_key_n(right_cache, i)  = *nth_key_n(right_cache, i - 1);
            *nth_pointer(right_cache, i)= *nth_pointer(right_cache, i - 1);
        }
        ++r.size;--p.size;
        *nth_key_n(right_cache, 0) = *nth_key_n(p_cache, p.size);
        *nth_pointer(right_cache) = *nth_pointer(p_cache, p.size);
        r.key = *nth_key_n(right_cache, 0);
        save_cache_n(right_cache, r);
        save_cache_n(p_cache, p);
        save_node(r), save_node(p);
    }
    inline void right_balance_l(node &p, node &r, byte *p_cache){
        byte right_cache[leaf_inf_size];
        load_cache_l(right_cache, r);
        for (size_t i = r.size;i > 0;i--){
            *nth_key_l(right_cache, i) = *nth_key_l(right_cache, i - 1);
            *nth_value(right_cache, i) = *nth_value(right_cache, i - 1);
        }
        ++r.size;--p.size;
        *nth_key_l(right_cache, 0) = *nth_key_l(p_cache, p.size);
        *nth_value(right_cache) = *nth_value(p_cache, p.size);
        r.key = *nth_key_l(right_cache, 0);
        save_cache_l(right_cache, r);
        save_cache_l(p_cache, p);
        save_node(r), save_node(p);
    }
    /*
        * Receive a node from the left part if avaliable
        * Save them both
    */
    inline void receive_left_n(node &p, node &l, byte *p_cache){
        byte left_cache[non_leaf_inf_size];
        load_cache_n(left_cache, l);
        for (size_t i = p.size;i > 0;i--){
            *nth_key_n(p_cache, i)  = *nth_key_n(p_cache, i - 1);
            *nth_pointer(p_cache, i)= *nth_pointer(p_cache, i - 1);
        }
        p.key = *nth_key_n(p_cache, 0)  = *nth_key_n(left_cache, l.size);
        *nth_pointer(p_cache)           = *nth_pointer(left_cache, l.size);
        ++p.size;--l.size;
        save_cache_n(left_cache, l);
        save_cache_n(p_cache, p);
        save_node(l), save_node(p);
    }
    inline void receive_left_l(node &p, node &l, byte *p_cache){
        byte left_cache[leaf_inf_size];
        load_cache_l(left_cache, l);
        for (size_t i = p.size;i > 0;i--){
            *nth_key_l(p_cache, i) = *nth_key_l(p_cache, i - 1);
            *nth_value(p_cache, i) = *nth_value(p_cache, i - 1);
        }
        p.key = *nth_key_l(p_cache, 0)  = *nth_key_l(left_cache, l.size);
        *nth_value(p_cache)             = *nth_value(left_cache, l.size);
        ++p.size;--l.size;
        save_cache_l(left_cache, l);
        save_cache_l(p_cache, p);
        save_node(l), save_node(p);
    }
    /*
        * Receive a node from the right part if avaliable. 
        * Save them both
    */
    inline void receive_right_n(node &p, node &r, byte *p_cache){
        byte right_cache[non_leaf_inf_size];
        load_cache_n(right_cache, r);
        *nth_key_n(p_cache, p.size)     = *nth_key_n(right_cache, 0);
        *nth_pointer(p_cache, p.size)   = *nth_pointer(right_cache);
        ++p.size, --r.size;
        r.key = *nth_key_n(right_cache, 1);
        save_cache_n((byte *)nth_key_n(right_cache, 1), r);
        save_cache_n(p_cache, p);
        save_node(r), save_node(p);
    }
    inline void receive_right_l(node &p, node &r, byte *p_cache){
        byte right_cache[leaf_inf_size];
        load_cache_l(right_cache, r);
        *nth_key_l(p_cache, p.size) = *nth_key_l(right_cache, 0);
        *nth_value(p_cache, p.size) = *nth_value(right_cache);
        ++p.size, --r.size;
        r.key = *nth_key_l(right_cache, 1);
        save_cache_l((byte *)nth_key_l(right_cache, 1), r);
        save_cache_l(p_cache, p);
        save_node(r), save_node(p);
    }
    /*
        * Find the quickest way to solve the problem of extreme size.
        * save now and cache_now and left/right and its cache if necessary, 
        * but do not save the cache of the parent
    */
    inline void deal_surplus_n(node &now, node &par, byte *cache, byte *cache_par, size_t ord){
        node tmp;
        if (now.pos != *nth_pointer(cache_par)){
            tmp = load_node(now.prior);
            if (tmp.size < part_size - 1) {
                left_balance_n(now, tmp, cache);
                *nth_key_n(cache_par, ord) = now.key;
                return;
            }
        }
        if (now.pos != *nth_pointer(cache_par, par.size - 1)){
            tmp = load_node(now.next);
            if (tmp.size < part_size - 1){
                right_balance_n(now, tmp, cache);
                *nth_key_n(cache_par, ord + 1) = tmp.key;
                return;
            }
            else split_n(now, cache, par, cache_par, ord);
        }
        else split_n(now, cache, par, cache_par, ord);
    }
    inline void deal_surplus_l(node &now, node &par, byte *cache, byte *cache_par, size_t ord){
        node tmp;
        if (now.pos != *nth_pointer(cache_par)){
            tmp = load_node(now.prior);
            if (tmp.size < part_size - 1) {
                left_balance_l(now, tmp, cache);
                *nth_key_n(cache_par, ord) = now.key;
                return;
            }
        }
        if (now.pos != *nth_pointer(cache_par, par.size - 1)){
            tmp = load_node(now.next);
            if (tmp.size < part_size - 1){
                right_balance_l(now, tmp, cache);
                *nth_key_n(cache_par, ord + 1) = tmp.key;
                return;
            }
            else split_l(now, cache, par, cache_par, ord);
        }
        else split_l(now, cache, par, cache_par, ord);
    }
    inline void deal_deficit_n(node &now, node &par, byte *cache,byte *cache_par, size_t ord){
        node tmp;
        if (now.pos != *nth_pointer(cache_par, par.size - 1)){
            tmp = load_node(now.next);
            if (tmp.size >= part_size / 2){
                receive_right_n(now, tmp, cache);
                *nth_key_n(cache_par, ord + 1) = tmp.key;
                return;
            }
            else merge_n(now, cache, par, cache_par, tmp);
        }
        if (now.pos != *nth_pointer(cache_par)){
            tmp = load_node(now.prior);
            if (tmp.size >= part_size / 2){
                receive_left_n(now, tmp, cache);
                *nth_key_n(cache_par, ord) = now.key;
                return;
            }
            else{
                byte cache_tmp[non_leaf_inf_size];load_cache_n(cache_tmp, tmp);
                merge_n(tmp, cache_tmp, par, cache_par, now);
            }
        }
    }
    inline void deal_deficit_l(node &now, node &par, byte *cache,byte *cache_par, size_t ord){
        node tmp;
        if (now.pos != *nth_pointer(cache_par, par.size - 1)){
            tmp = load_node(now.next);
            if (tmp.size >= part_size / 2){
                receive_right_l(now, tmp, cache);
                *nth_key_n(cache_par, ord + 1) = tmp.key;
                return;
            }
            else merge_l(now, cache, par, cache_par, tmp);
        }
        if (now.pos != *nth_pointer(cache_par)){
            tmp = load_node(now.prior);
            if (tmp.size >= part_size / 2){
                receive_left_l(now, tmp, cache);
                *nth_key_n(cache_par, ord) = now.key;
                return;
            }
            else{
                byte cache_tmp[leaf_inf_size];load_cache_l(cache_tmp, tmp);
                merge_l(tmp, cache_tmp, par, cache_par, now);
            }
        }
    }
    /*
        * split the cache belonging to node now. 
        * save now and its cache but do not save any info of parent into storage,
        * though we have changed it in memory
    */
    void split_n(node &now, byte *cache, node &par, byte *cache_par, size_t order){
        size_t s = now.size / 2;
        now.size -= s;
        size_t ns = now.size;
        pointer pos = alloc.alloc(non_leaf_size);
        // printf("ask_for_bpt: %d where %d\n", non_leaf_size, pos);
        node tmp = node(*nth_key_n(cache, ns), pos, now.pos, now.next, s, now.type);
        if (now.next != invalid_p){
            node temp = load_node(now.next);
            temp.prior = tmp.pos;
            save_node(temp);
        }
        now.next = tmp.pos;
        byte *cache_tmp = (byte *)nth_key_n(cache, ns);
        save_cache_n(cache_tmp, tmp);
        save_cache_n(cache, now);
        ns = order;
        for (size_t i = par.size;i > ns + 1;i--){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i - 1);
            *nth_pointer(cache_par, i)  = *nth_pointer(cache_par, i - 1);
        }
        *nth_key_n(cache_par, ns + 1) = tmp.key;
        *nth_pointer(cache_par, ns + 1) = tmp.pos;
        ++par.size;
        save_node(now);save_node(tmp);
    }
    void split_l(node &now, byte *cache, node &par, byte *cache_par, size_t order){
        size_t s = now.size / 2;
        now.size -= s;
        size_t ns = now.size;
        pointer pos = alloc.alloc(leaf_size);
        // printf("ask_for_bpt: %d where %d\n", leaf_size, pos);
        node tmp = node(*nth_key_l(cache, ns), pos, now.pos, now.next, s, now.type);
        if (now.next != invalid_p){
            node temp = load_node(now.next);
            temp.prior = tmp.pos;
            save_node(temp);
        }
        now.next = tmp.pos;
        byte *cache_tmp = (byte *)nth_key_l(cache, ns);
        save_cache_l(cache_tmp, tmp);
        save_cache_l(cache, now);
        ns = order;
        for (size_t i = par.size;i > ns + 1;i--){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i - 1);
            *nth_pointer(cache_par, i)  = *nth_pointer(cache_par, i - 1);
        }
        *nth_key_n(cache_par, ns + 1) = tmp.key;
        *nth_pointer(cache_par, ns + 1) = tmp.pos;
        ++par.size;
        save_node(now);save_node(tmp);
    }
    //merge now and the next of it, if the result is too big, use split automatically
    void merge_n(node &now, byte *cache, node &par, byte *cache_par, node &tmp){
        now.next = tmp.next;
        if (tmp.next != invalid_p){
            node temp = load_node(tmp.next);
            temp.prior = now.pos;
            save_node(temp);
        }
        size_t s = now.size;
        load_cache_n(cache + now.size, tmp.size);
        now.size += tmp.size;
        alloc.free(tmp.pos, non_leaf_size);
        s = binary_search_key_n(cache_par, now.key, par.size);
        --par.size;
        for (size_t i = s + 1;i < par.size;i++){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i + 1);
            *nth_pointer(cache_par, i)  = *nth_pointer(cache_par, i + 1);
        }
        if (now.size >= part_size) split_n(now, cache, par, cache_par);
        save_cache_n(cache, now);
        save_node(now);
    }
    void merge_l(node &now, byte *cache, node &par, byte *cache_par, node &tmp){
        now.next = tmp.next;
        if (tmp.next != invalid_p){
            node temp = load_node(tmp.next);
            temp.prior = now.pos;
            save_node(temp);
        }
        size_t s = now.size;
        load_cache_l(cache + now.size, tmp.size);
        now.size += tmp.size;
        alloc.free(tmp.pos, leaf_size);
        s = binary_search_key_l(cache_par, now.key, par.size);
        --par.size;
        for (size_t i = s + 1;i < par.size;i++){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i + 1);
            *nth_pointer(cache_par, i)  = *nth_pointer(cache_par, i + 1);
        }
        if (now.size >= part_size) split_l(now, cache, par, cache_par);
        save_cache_l(cache, now);
        save_node(now);
    }
    /*
        * Insert (k,v) and return true if it is an insertion and false for a change, 
        * whether split or not is considered in its parent,
        * as we prove that all parts are smaller than part_size, the memory is safe.
        * But mention to judge the size of the root.(actually, it does need to be specially treated)
    */
    bool _insert_n(node &now, const key_type &k, const value_type &v, byte *cache){
        size_t ord  = binary_search_key_n(cache, k, now.size);
        pointer loc = *nth_pointer(cache, ord);
        node child_node = load_node(loc);
        bool ret;
        if (child_node.type){
            byte cache_child[non_leaf_inf_size];
            load_cache_n(cache_child, child_node);
            ret = _insert_n(child_node, k, v, cache_child);
            if (child_node.size >= part_size) {
                deal_surplus_n(child_node, now, cache_child, cache, ord);
            }
            else {
                save_node(child_node);
                save_cache_n(cache_child, child_node);
            }
        }
        else{
            byte cache_child[leaf_inf_size];
            load_cache_l(cache_child, child_node);
            ret = _insert_l(child_node, k, v, cache_child);
            if (child_node.size >= part_size) {
                deal_surplus_l(child_node, now, cache_child, cache, ord);
            }
            else {
                save_node(child_node);
                save_cache_l(cache_child, child_node);
            }
        }
        return ret;
    }
    bool _insert_l(node &now, const key_type &k, const value_type &v, byte *cache){
        size_t ord  = binary_search_key_l(cache, k, now.size);
        pointer loc = nth_value_loc(now, ord);
        if (equal(*nth_key_l(cache, ord), k)) return false;
        ++num;
        for (size_t i = now.size;i > ord + 1;i--){
            *nth_key_l(cache, i)    = *nth_key_l(cache, i - 1);
            *nth_value(cache, i)    = *nth_value(cache, i - 1);
        }
        ++now.size;
        *nth_key_l(cache, ord + 1) = k;
        *nth_value(cache, ord + 1) = v;
        // p.key = *nth_key(p, 0); shall we add this?
        return true;
    }
    void _insert_head_n(node &now, const key_type &k, const value_type &v, byte *cache){
        node node_child = load_node(*nth_pointer(cache));
        if (node_child.type){
            byte cache_child[non_leaf_inf_size];
            load_cache_n(cache_child, node_child);
            _insert_head_n(node_child, k, v, cache_child);
            *nth_key_n(cache, 0) = now.key = k;
            if (node_child.size >= part_size){
                deal_surplus_n(node_child, now, cache_child, cache, 0);
            }
            else{
                save_node(node_child);
                save_cache_n(cache_child, node_child);
            }
        }
        else{
            byte cache_child[leaf_inf_size];
            load_cache_l(cache_child, node_child);
            _insert_head_l(node_child, k, v, cache_child);
            *nth_key_l(cache, 0) = now.key = k;
            if (node_child.size >= part_size){
                deal_surplus_l(node_child, now, cache_child, cache, 0);
            }
            else {
                save_node(node_child);
                save_cache_l(cache_child, node_child);
            }
        }
    }
    void _insert_head_l(node &now, const key_type &k, const value_type &v, byte *cache){
        ++num;
        for (size_t i = now.size;i > 0;i--){
            *nth_key_l(cache, i)    = *nth_key_l(cache, i - 1);
            *nth_value(cache, i)    = *nth_value(cache, i - 1);
        }
        ++now.size;
        *nth_key_l(cache, 0) = now.key = k;
        *nth_value(cache, 0) = v;
    }
    bool _remove_n(node &p, const key_type &k, byte *cache){
        size_t ord = binary_search_key_n(cache, k, p.size);
        pointer loc = *nth_pointer(cache, ord);
        node child_node = load_node(loc);
        bool ret;
        if (child_node.type){
            byte cache_child[non_leaf_inf_size];
            load_cache_n(cache_child, child_node);
            ret = _remove_n(child_node, k, cache_child);
            if (child_node.size < part_size / 2){
                deal_deficit_n(child_node, p, cache_child, cache, ord);
            }
            else{
                save_node(child_node);
                save_cache_n(cache_child);
            }
        }
        else{
            byte cache_child[leaf_inf_size];
            load_cache_l(cache_child, child_node);
            ret = _remove_l(child_node, k, cache_child);
            if (child_node.size < part_size / 2){
                deal_deficit_l(child_node, p, cache_child, cache, ord);
            }
            else{
                save_node(child_node);
                save_cache_l(cache_child);
            }
        }
        return ret;
    }
    bool _remove_l(node &p, const key_type k, byte *cache){
        size_t ord = binary_search_key_l(cache, k, p.size);
        if (*nth_key_l(cache, ord) == k){
            --num;
            --p.size;
            for (size_t i = ord;i < p.size;i++){
                *nth_key_l(cache, i)    = *nth_key_l(cache, i + 1);
                *nth_value(cache, i)    = *nth_value(cache, i + 1);
            }
            save_cache(cache, p);
            p.key = *nth_key_l(cache, 0);
            return true;
        }
        else return false;
    }
public:
    bplustree():non_leaf_size(sizeof(node) + (sizeof(key_type) + sizeof(pointer)) * part_size), 
    leaf_size(sizeof(node) + (sizeof(key_type) + sizeof(value_type)) * part_size), 
    non_leaf_inf_size((sizeof(key_type) + sizeof(pointer)) * part_size), 
    leaf_inf_size((sizeof(key_type) + sizeof(value_type)) * part_size), 
    root(), num(0), root_pos(0){}
    void init(const char *datafile_name, const char *alloc_name){
        index_name = new char[strlen(alloc_name) + 1];
        strcpy(index_name, alloc_name);
        data_name = new char[strlen(datafile_name) + 1];
        strcpy(data_name, datafile_name);
        alloc.init(index_name);
        datafile = fopen(data_name, "rb+");
        if (!datafile) datafile = fopen(data_name, "wb+");
        fseek(datafile, 0, SEEK_SET);
        // printf("size_of_node: %d; leaf_size: %d; non_leaf_size: %d\n", sizeof(node), leaf_size, non_leaf_size);
        // printf("ini_root_pointer_seek: %d\n", 0);
        if (!fread(&root_pos, sizeof(pointer), 1, datafile)){
            alloc.alloc(sizeof(pointer));
            alloc.alloc(sizeof(size_t));
            root_pos = alloc.alloc(leaf_size);
            fseek(datafile, root_pos, SEEK_SET);
            // printf("ini_root_seek: %d\n", root_pos);
            fwrite(&root, sizeof(node), 1, datafile);
            // printf("which is: pos:%d size:%d key:%d\n", root.pos, root.size, root.key);
        }
        else{
            fread(&num, sizeof(size_t), 1, datafile);
            root = load_node(root_pos);
        }
        // printf("\n%d\n", root_pos);
    }
    void clear(){
        if (datafile) fclose(datafile);
        datafile = fopen(data_name, "wb+");
        alloc.refill(index_name);
        fseek(datafile, 0, SEEK_SET);
        num = 0;
        alloc.alloc(sizeof(pointer));
        alloc.alloc(sizeof(size_t));;
        root_pos = alloc.alloc(leaf_size);
        root = node();
        // printf("clear_root_pointer_seek: %d\n", 0);
        if (!fread(&root_pos, sizeof(pointer), 1, datafile)){
            fseek(datafile, root_pos, SEEK_SET);
            // printf("clear_root_seek: %d\n", 0);
            fwrite(&root, sizeof(node), 1, datafile);
            // printf("which is: pos:%d size:%d key:%d\n", root.pos, root.size, root.key);
        }
    }
    bool empty(){
        return !num;
    }
    ~bplustree(){
        alloc.save(index_name);
        save_node(root);
        fseek(datafile, 0, SEEK_SET);
        if (root.pos != invalid_p) root_pos = root.pos;
        // printf("decode_root_pointer_seek: %d\nwhich is: %d\n", 0, root_pos);
        fwrite(&root_pos, sizeof(pointer), 1, datafile);
        fwrite(&num, sizeof(size_t), 1, datafile);
        // printf("which is: %d\n", root_pos);
        if (datafile) fclose(datafile);
        delete index_name;delete data_name;
    }
    bool count(const key_type &k){
        if (empty()) return 0;
        pointer p = _find(root, k);
        if (p == invalid_p) return 0;
        return 1;
    }
    value_type find(const key_type &k){
        if (empty()) throw(container_is_empty());
        pointer p = _find(root, k);
        if (p == invalid_p) return value_type();
        fseek(datafile, p, SEEK_SET);
        value_type v;
        fread(&v, sizeof(value_type), 1, datafile);
        // printf("find_seek_in_database: %d\nwhich is: %d\n", p + sizeof(key_type),v);
        return v;
    }
    bool set(const key_type &k, const value_type &v){
        if (empty()) throw(container_is_empty());
        pointer p = _find(root, k);
        if (p == invalid_p) return 0;
        fseek(datafile, p + sizeof(key_type), SEEK_SET);
        fwrite(&v, sizeof(value_type), 1, datafile);
        return 1;
    }
    bool insert(const key_type &k,const value_type &v){
        if (root.pos == invalid_p){
            if (!datafile) throw(runtime_error());
            root.pos = root_pos;
            root.size = num = 1;root.key = k;root.type = 0;
            fseek(datafile, root.pos + sizeof(node), SEEK_SET);
            // printf("insert_no_root_seek\n");
            fwrite(&k, sizeof(key_type), 1, datafile);fwrite(&v, sizeof(value_type), 1, datafile);
            // printf("which is: key: %d value: %d\n", k, v);
            return true;
        }
        bool ret = true;
        if (root.type){
            byte cache[non_leaf_inf_size];
            load_cache_n(cache, root);
            if (com(root.key, k)) ret = _insert_n(root, k, v, cache);
            else _insert_head_n(root, k, v, cache);
            save_cache_n(cache, root);
            if (root.size >= part_size){
                pointer pos = alloc.alloc(non_leaf_size);
                // printf("ask_for_bpt: at %d\n", pos);
                node now_root(root.key, pos);
                now_root.type = 1, now_root.size = 1;
                size_t s = root.size / 2;
                root.size -= s;
                root.next = pos = alloc.alloc(non_leaf_size);
                // printf("ask_for_bpt: at %d\n", pos);
                node temp(*nth_key_n(cache, root.size), pos, root.pos);
                temp.type = root.type, temp.size = s;
                save_cache_n((byte *)nth_key_n(cache, root.size), temp);
                save_node(temp);
                pos = now_root.pos;
                fseek(datafile, pos + sizeof(node), SEEK_SET);
                // printf("extend_root_seek: %d\n", pos + sizeof(node));
                    fwrite(&root.key, sizeof(key_type), 1, datafile);
                    fwrite(&root.pos, sizeof(pointer), 1, datafile);
                    fwrite(&temp.key, sizeof(key_type), 1, datafile);
                    fwrite(&temp.pos, sizeof(pointer), 1, datafile);
                // printf("which are: first_key: %d first_pos: %d second_key: %d second_pos: %d\n", root.key, root.pos, temp.key, temp.pos);
                save_node(root);
                root = now_root;root_pos = root.pos;root.size = 2;
            }
        }
        else{
            byte cache[leaf_inf_size];
            load_cache_l(cache, root);
            if (com(root.key, k)) ret = _insert_l(root, k, v, cache);
            else _insert_head_l(root, k, v, cache);
            save_cache_l(cache, root);
            if (root.size >= part_size){
                pointer pos = alloc.alloc(non_leaf_size);
                // printf("ask_for_bpt: at %d\n", pos);
                node now_root(root.key, pos);
                now_root.type = 1, now_root.size = 1;
                size_t s = root.size / 2;
                root.size -= s;
                root.next = pos = alloc.alloc(leaf_size);
                // printf("ask_for_bpt: at %d\n", pos);
                node temp(*nth_key_l(cache, root.size), pos, root.pos);
                temp.type = root.type, temp.size = s;
                save_cache_l((byte *)nth_key_l(cache, root.size), temp);save_node(temp);
                pos = now_root.pos;
                fseek(datafile, pos + sizeof(node), SEEK_SET);
                // printf("extend_root_seek: %d\n", pos + sizeof(node));
                    fwrite(&root.key, sizeof(key_type), 1, datafile);
                    fwrite(&root.pos, sizeof(pointer), 1, datafile);
                    fwrite(&temp.key, sizeof(key_type), 1, datafile);
                    fwrite(&temp.pos, sizeof(pointer), 1, datafile);
                // printf("which are: first_key: %d first_pos: %d second_key: %d second_pos: %d\n", root.key, root.pos, temp.key, temp.pos);
                save_node(root);
                root = now_root;root_pos = root.pos;root.size = 2;
            }
        }
        return ret;
    }
    bool remove(key_type k){
        return true;
    }
    vector<list_type> listof(key_type k, bool (*comp)(key_type a, key_type b)){
        if (empty()) throw(container_is_empty());
        return _listof(root, k, comp);
    }
};
#endif