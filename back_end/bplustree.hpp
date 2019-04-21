//version 0.1 only complete :find; split; merge; insert and delete functions.
//version 0.2 consider the input and output.
//version 0.3 the alloc function
//an alloc file for secondary storage is needed and can be greatly improved, writing at last
//there have some inspiration about the alloc function: 
//the only thing to alloc in index_file is always "a node and the list of its kids", 
//the only thing to alloc in database_file is "a key and a value", the same is true when free in two files
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <stdio.h>
#include <cstring>
#include "exceptions.h"
#include "alloc.hpp"
using pointer   =   long;
using byte      =   char;
const pointer invalid_p = 0xdeadbeef;
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
        node(key_type k = key_type(),
        pointer p = invalid_p, pointer par = invalid_p, 
        pointer pre = invalid_p, pointer nex = invalid_p, 
        size_t s = 1, bool ty = 0)
        :key(k),pos(p),parent(par),prior(pre),next(nex),size(s),type(ty){}
    };
    node root;
    Compare com;
    size_t num;
    FILE *datafile, *bptfile;
    const size_t node_size;
    const size_t inf_size;
    ALLOC alloc, alloc_data;
    pointer root_pos;
    char *dfa_name, *ba_name, *b_name, *df_name;

    inline bool equal(const key_type& k1,const key_type& k2){
        return !(com(k1, k2) || com(k2, k1));
    }
    /*
        * Load the info of node p and its brothers into the cache. 
        * The info of node p contains those keys greater than p.key but smaller than p.next.key
    */
    inline void load_cache(byte *start,const node& p){
        fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
        fread(start, (sizeof(key_type) + sizeof(pointer)) * p.size, 1, bptfile);
    }
    //save the info in the cache
    inline void save_cache(byte *start,const node &p){
        fseek(bptfile, p.pos + sizeof(node), SEEK_SET);
        fwrite(start, (sizeof(key_type) + sizeof(pointer)) * p.size, 1, bptfile);
    }
    //get the value from the file with pointer l, 
    //the result cannot be value_type& (notice the '&')
    inline value_type load_value(pointer l){
        fseek(datafile, l, SEEK_SET);
        value_type tmp;
        fread(&tmp, sizeof(value_type), 1, datafile);
        return tmp;
    }
    //get the whole info of a node from the file with pointer l
    inline node load_node(pointer l){
        fseek(bptfile, l, SEEK_SET);
        node tmp;
        fread(&tmp, sizeof(node), 1, bptfile);
        return tmp;
    }
    //write the whole info of a node to the file
    inline bool save_node(const node &p){
        if (p.pos == invalid_p) return false;
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
        * use binary search to find the one smaller than or equal to k but the next is greater,
        * mention that the key must be greater than or equal to the first key in the compared set.
        * there are n elements in the compared set in total.
        * mention that the result cannot be n. great.
        OOOOOP! mention that if the inserted (k, v) has a key smaller than any exist key, it should be judged at first
    */
    inline size_t binary_search_key(byte *start,const key_type& k, size_t n){
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) / 2;
            if ( com(*nth_element_key(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        if (l >= n) return n - 1;
        else if (equal(*nth_element_key(start, l), k)) return l;
        else return l - 1;
    }
    /*
        * find the pointer of database file locating the value of key k. 
        * k must be not smaller than the smallest key 
    */
    pointer _find(const node &p,const key_type& k){
        byte cache[inf_size];
        load_cache(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        pointer tmp = *nth_element_pointer(cache, ord);
        if (!p.type){
            if (equal(*nth_element_key(cache, ord), k)) return tmp;
            else return invalid_p;
        }
        return _find(load_node(tmp), k);
    }

    /*
        * make the first of p become the last of l while the size of p equals to pars_size and that of l is less. 
        * save them both
    */
    inline void left_balance(node &p, node &l, byte *p_cache){
        byte left_cache[inf_size];
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
        * Save them both. do we need so?
    */
    inline void right_balance(node &p, node &r, byte *p_cache){
        byte right_cache[inf_size];
        load_cache(right_cache, r);
        for (size_t i = r.size;i > 0;i++){
            *nth_element_key(right_cache, i) = *nth_element_key(right_cache, i - 1);
            *nth_element_pointer(right_cache, i) = *nth_element_pointer(right_cache, i - 1);
        }
        ++r.size;--p.size;
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
    inline void receive_left(node &p, node &l, byte *p_cache){
        byte left_cache[inf_size];
        load_cache(left_cache, l);
        for (size_t i = p.size;i > 0;i++){
            *nth_element_key(p_cache, i) = *nth_element_key(p_cache, i - 1);
            *nth_element_pointer(p_cache, i) = *nth_element_pointer(p_cache, i - 1);
        }
        ++p.size;--l.size;
        *nth_element_key(p_cache, 0)        = *nth_element_key(left_cache, p.size);
        *nth_element_pointer(p_cache, 0)    = *nth_element_pointer(left_cache, p.size);
        // if (p.size >= part_size)
        // if (p.next){
        //     node tmp = load_node(p.next);
        //     if (tmp.size < part_size) right_balance(p, tmp);
        // }
        save_cache(left_cache, l);
        save_cache(p_cache, p);
    }
    /*
        * Receive a node from the right part if avaliable. 
        * Save them both
    */
    inline void receive_right(node &p, node &r, byte *p_cache){
        byte right_cache[inf_size];
        load_cache(right_cache, r);
        *nth_element_key(p_cache, p.size)       = *nth_element_key(right_cache, 0);
        *nth_element_pointer(p_cache, p.size)   = *nth_element_pointer(right_cache, 0);
        ++p.size, --r.size;
        for (size_t i = 0; i < r.size;i++){
            *nth_element_key(right_cache, i)        = *nth_element_key(right_cache, i + 1);
            *nth_element_pointer(right_cache, i)    = *nth_element_pointer(right_cache, i + 1);
        }
        // if (l.size >= part_size)
        // if (l.prior){
        //     node tmp = load_node(l.next);
        //     if (tmp.size < part_size) left_balance(l,tmp,left_cache);
        // }
        save_cache(right_cache, r);
        save_cache(p_cache, p);
    }
    /*
        * Find the quickest way to solve the problem of the size of a node, 
        * mode = 0 when the size is too big and 1 otherwise. 
        * Save p and its brother, save parent?
        * another question is raised that do rebalance faster than split/merge?
        * and do the result change considering further using ratial and its influence?
    */
    void consider(node &p, bool mode, node &par, byte *cache_par = nullptr){
        node tmp;
        int tried = 0;
        byte cache[inf_size];
        load_cache(cache, p);
        if (cache_par == nullptr) load_cache(cache_par, par);
        while (tried < 2){
            if (!tried){
                if (p.pos == *nth_element_pointer(cache_par, 0)) ++tried;
                else{
                    tmp = load_node(p.prior);
                    if (mode){
                        if (tmp.size > part_size / 2){
                            receive_left(p, tmp, cache);
                            break;
                        }
                        else ++tried;
                    }
                    else if (tmp.size < part_size - 1) {
                        left_balance(p, tmp, cache);
                        break;
                    }
                    else ++tried;
                }
            }
            else{
                if (p.pos == *nth_element_pointer(cache_par, par.size - 1)) {
                    if (mode){
                        if (p.prior == invalid_p){
                            if (par.parent != invalid_p) {
                                node pp = load_node(p.parent);
                                consider(par, 1, pp);
                            }
                            //shall we save node p there?
                            //or does this condition actually exist?
                            //yep, unless a better solution that is to decrease the height is usen
                            break;
                        }
                        else{
                            byte cache_tmp[inf_size];
                            load_cache(cache_tmp, tmp);
                            merge(tmp, cache_tmp, cache, par);
                            break;
                        }
                    }
                    else{
                        split(p, cache, par);
                        //++par.size; in split function it is already checked
                        save_node(par);
                        break;
                    }
                }
                else{
                    tmp = load_node(p.next);
                    if (mode){
                        byte cache_tmp[inf_size];
                        load_cache(cache_tmp, tmp);
                        merge(p, cache, cache_tmp, par);
                        break;
                    }
                    else if (tmp.size < part_size - 1){
                        right_balance(p, tmp, cache);
                        break;
                    }
                    else {
                        split(p,cache, par);
                        //++par.size; in split function it is already checked
                        break;
                    }
                }
            }
        }
    }
    /*
        * split the cache belonging to node now and change the size of its parent p. 
        * shall we save the info of p? I saved now but it may can be improved
    */
    void split(node &now, byte *cache, node &p){
        size_t s = now.size / 2;
        now.size -= s;
        size_t ns = now.size;
        node tmp = node(*nth_element_key(cache, ns), invalid_p, now.parent, now.pos, now.next, s, now.type);
        if (now.next != invalid_p){
            node temp = load_node(now.next);
            temp.prior = tmp.pos;
            save_node(temp);
        }
        now.next = tmp.pos;
        pointer pos = alloc.alloc(node_size);
        tmp.pos = pos;
        byte cache_tmp[inf_size];
        for (size_t i = 0;i < s;i++){
            *nth_element_key(cache_tmp, i)      = *nth_element_key(cache, ns + i);
            *nth_element_pointer(cache_tmp, i)  = *nth_element_pointer(cache, ns + i);
            //or maybe it needs to add "+1"?
        }
        save_cache(cache_tmp, tmp);
        // save_cache(cache, now);
        load_cache(cache_tmp, p);
        ns = binary_search_key(cache_tmp, now.key, p.size);
        for (size_t i = p.size;i > ns + 1;i++){
            *nth_element_key(cache_tmp, i)      = *nth_element_key(cache_tmp, i - 1);
            *nth_element_pointer(cache_tmp, i)  = *nth_element_pointer(cache_tmp, i - 1);
        }
        *nth_element_key(cache_tmp, ns + 1) = tmp.key;
        *nth_element_pointer(cache_tmp, ns + 1) = tmp.pos;
        ++p.size;
        save_cache(cache_tmp, p);
        if (p.size >= part_size){
            if (p.parent == invalid_p){
                root.pos = alloc.alloc(node_size);
                fseek(bptfile, root.pos + sizeof(node), SEEK_SET);
                fwrite(&p.pos, sizeof(pointer), 1, bptfile);
                p.parent = root.pos;
            }
            node par = load_node(p.parent);
            consider(p, 0, par);
        }
        else save_node(p);
    }
    //merge now and the next of it, if the result is too big, use split automatically
    void merge(node &now, byte *cache_a, byte *cache_b, node &par){
        node tmp = load_node(now.next);
        now.next = tmp.next;
        if (tmp.next != invalid_p){
            node temp = load_node(tmp.next);
            temp.prior = now.pos;
            save_node(temp);
        }
        size_t s = now.size;
        for (size_t i = 0;i < tmp.size;i++){
            *nth_element_key(cache_a, s + i)        = *nth_element_key(cache_b, i);
            *nth_element_pointer(cache_a, s + i)    = *nth_element_pointer(cache_b, i);
        }
        now.size += tmp.size;
        alloc.free(tmp.pos, node_size);
        byte cache_par[inf_size];
        load_cache(cache_par, par);
        s = binary_search_key(cache_par, now.key, par.size);
        --par.size;
        for (size_t i = s + 1;i < par.size;i++){
            *nth_element_key(cache_par, i)      = *nth_element_key(cache_par, i + 1);
            *nth_element_pointer(cache_par, i)  = *nth_element_pointer(cache_par, i + 1);
        }
        save_cache(cache_par, par);
        if (now.size >= part_size) split(now, cache_a, par);
        else {
            if (par.size > part_size / 2 || par.parent == invalid_p)
                save_node(par);
            else {
                node pp = load_node(par.parent);
                consider(par, 1, pp);
            }
        }
        save_cache(cache_a, now);
    }
    /*
        * Insert (k,v) and return true if it is an insertion and false for a change, 
        * whether split or not is considered in its parent,
        * as we prove that all parts are smaller than part_size, the memory is safe.
        * But mention to judge the size of the root.(actually, it does need to be specially treated)
    */
    bool _insert(node &p, const key_type &k, const value_type &v){
        byte cache[inf_size];load_cache(cache, p);
        size_t ord  = binary_search_key(cache, k, p.size);
        pointer loc = *nth_element_pointer(cache, ord);
        if (p.type){
            node child_node = load_node(loc);
            //w...it will not actually clear the cache there, but clear it might be a good idea?
            //nonsense, the memory is not tested and will not be tooooo big.
            bool ret = _insert(child_node, k, v);
            if (child_node.size >= part_size) {
                consider(child_node, 0, p, cache);
                save_node(p);//do we need this?
            }
            else save_node(child_node);
            return ret;
        }
        else{
            if (equal(*nth_element_key(cache, ord), k)) {
                pointer value_pointer = *nth_element_pointer(cache, ord);
                fseek(datafile, value_pointer, SEEK_SET);
                //shall we change it? or shall we simply throw something? or return the pointer waiting 
                return false;
            }
            num++;
            pointer pos = alloc_data.alloc(sizeof(key_type) + sizeof(value_type));
            fseek(datafile, pos, SEEK_SET);
            fwrite(&k, sizeof(key_type), 1, datafile);
            fwrite(&v, sizeof(value_type), 1, datafile);
            for (size_t i = p.size;i > ord + 1;i--){
                *nth_element_key(cache, i)      = *nth_element_key(cache, i - 1);
                *nth_element_pointer(cache, i)  = *nth_element_pointer(cache, i - 1);
            }
            ++p.size;
            *nth_element_key(cache, ord + 1) = k;
            *nth_element_pointer(cache, ord + 1) = pos;
            save_cache(cache, p);
            // w......if it is oversized, this save function is surplus. can we improve it?
            // p.key = *nth_element_key(p, 0); shall we add this?
            return true;
        }
    }
    //mention that the root node is not saved in this function
    bool _remove(node &p, const key_type &k){
        byte cache[inf_size];
        load_cache(cache, p);
        size_t ord = binary_search_key(cache, k, p.size);
        if (p.type){
            pointer loc = *nth_element_pointer(cache, ord);
            node child_node = load_node(loc);
            bool ret = _remove(child_node, k);
            p.key = *nth_element_key(cache, 0);
            if (child_node.size < part_size / 2) {
                consider(child_node, 1, p, cache);
            }
            save_node(child_node);
            return ret;
        }
        else{
            if (*nth_element_key(cache, ord) == k){
                alloc_data.free(*nth_element_pointer(cache, ord), sizeof(key_type) + sizeof(pointer));
                --num;
                --p.size;
                for (size_t i = ord;i < p.size;i++){
                    *nth_element_key(cache, i)      = *nth_element_key(cache, i + 1);
                    *nth_element_pointer(cache, i)  = *nth_element_pointer(cache, i + 1);
                }
                save_cache(cache, p);
                p.key = *nth_element_key(cache, 0);
                return true;
            }
            else return false;
            //or throw something?
        }
    }
    bool empty(){
        return !num;
    }
public:
    bplustree():node_size(sizeof(node) + (sizeof(key_type) + sizeof(pointer)) * part_size), 
    inf_size((sizeof(key_type) + sizeof(pointer)) * part_size), 
    root(), num(0), root_pos(0){}
    void initialize(const char *datafile_name, const char *bptfile_name, const char *data_alloc, const char *bpt_alloc){
        ba_name = new char[strlen(bpt_alloc) + 1];
        strcpy(ba_name, bpt_alloc);
        dfa_name = new char[strlen(data_alloc) + 1];
        strcpy(dfa_name, data_alloc);
        b_name = new char[strlen(bptfile_name) + 1];
        strcpy(b_name, bptfile_name);
        df_name = new char[strlen(datafile_name) + 1];
        strcpy(df_name, datafile_name);
        alloc.initialize(ba_name);
        alloc_data.initialize(dfa_name);
        bptfile = fopen(bptfile_name, "rb+");
        if (!bptfile) bptfile = fopen(bptfile_name, "wb+");
        datafile = fopen(datafile_name, "rb+");
        if (!datafile) datafile = fopen(datafile_name, "wb+");
        fseek(bptfile, 0, SEEK_SET);
        if (!fread(&root_pos, sizeof(pointer), 1, bptfile)){
            alloc.alloc(sizeof(pointer));
            root_pos = alloc.alloc(node_size);
            fseek(bptfile, root_pos, SEEK_SET);
            fwrite(&root, sizeof(node), 1, bptfile);
        }
        // printf("\n%d\n", root_pos);
    }
    void clear(){
        if (bptfile) fclose(bptfile);
        if (datafile) fclose(datafile);
        bptfile = fopen(b_name, "wb+");
        datafile = fopen(df_name, "wb+");
        delete b_name;delete df_name;
        alloc.refill(ba_name);
        alloc_data.refill(dfa_name);
    }
    ~bplustree(){
        alloc.save(ba_name);
        alloc_data.save(dfa_name);
        save_node(root);
        fseek(bptfile, 0, SEEK_SET);
        if (root.pos != invalid_p) root_pos = root.pos;
        fwrite(&root_pos, sizeof(pointer), 1, bptfile);
        if (bptfile) fclose(bptfile);
        if (datafile) fclose(datafile);
        delete ba_name;delete dfa_name;delete b_name;delete df_name;
    }
    value_type find(const key_type &k){
        if (empty()) throw(container_is_empty());
        pointer p = _find(root, k);
        if (p == invalid_p) return value_type();
        fseek(datafile, p + sizeof(key_type), SEEK_SET);
        value_type v;
        fread(&v, sizeof(value_type), 1, datafile);
        return v;
    }
    bool insert(const key_type &k,const value_type &v){
        if (root.pos == invalid_p){
            if (!bptfile) throw(runtime_error());
            root.pos = root_pos;
            root.size = num = 1;root.key = k;root.type = 0;
            byte cache[inf_size];
            load_cache(cache, root);
            *nth_element_key(cache, 0) = k;
            *nth_element_pointer(cache, 0) = alloc_data.alloc(sizeof(key_type) + sizeof(value_type));
            fseek(datafile, *nth_element_pointer(cache, 0), SEEK_SET);
            fwrite(&k, sizeof(key_type), 1, datafile);fwrite(&v, sizeof(value_type), 1, datafile);
            save_cache(cache, root);
            return true;
        };
        if (com(root.key, k)) _insert(root, k, v);
        else{
            
        };//waiting_to_complete;
        if (root.size >= part_size){
            pointer pos = alloc.alloc(node_size);
            fseek(bptfile, pos + sizeof(node), SEEK_SET);
            fwrite(&root.pos, sizeof(pointer), 1, bptfile);
            root.parent = pos;
            node now_root;
            now_root.pos = pos, now_root.type = 1, now_root.size = 1, now_root.key = root.key;
            byte cache[inf_size];
            load_cache(cache, root);
            split(root, cache, now_root);
            root = now_root;root_pos = pos;
        }
    }
    bool remove(key_type k){
        
    }
    void listof(key_type k, bool (*comp)(key_type a, key_type b)){

    }
};
#endif