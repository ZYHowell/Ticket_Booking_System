#pragma warning(disable : 4996)
#ifndef SJTU_BPLUSTREE_HPP
#define SJTU_BPLUSTREE_HPP
#include <cstddef>
#include <functional>
#include <cstring>
#include "exceptions.h"
#include "alloc.hpp"
#include "vector.hpp"
#include "bufferpool.hpp"
#include "tool.h"
const point invalid_p = 0xdeadbeef;

template<class key_t,
         class value_type, 
         size_t node_size = 4096, 
         class Compare = std::less<key_t>
>   class bplustree
{
    using point         =   long;
    using byte          =   char;
    using list_t        =   pair<key_t, value_type>;
    using find_t        =   pair<bool, value_type>;
    using sub_find_t    =   pair<size_t, buf_block_t*>;
    struct node
    {
        key_t       key;
        point       prior, next;
        point       pos;
        size_t      size;                    //the size of its brothers
        bool        type;                    //0 for a leaf and 1 otherwise
        node(key_t k = key_t(),
        point p = invalid_p, size_t s = 1, bool ty = 0, 
        point pre = invalid_p, point nex = invalid_p)
        :key(k),pos(p),prior(pre),next(nex),size(s),type(ty){}
        node operator=(const node &other){
            key = other.key, prior = other.prior, next = other.next;
            size = other.size, type = other.type, pos = other.pos;
            return *this;
        }
    };
    buf_pool_t<node_size, node>   *buf;
    Compare                 com;
    size_t                  num;
    FILE                    *datafile;
	const size_t            part_size_l, part_size_n;
    ALLOC<node_size>        alloc;
    point                   root_pos;
    char                    *index_name, *data_name;

    inline bool equal(const key_t& k1,const key_t& k2) const
    {
        return !(com(k1, k2) || com(k2, k1));
    }
    /*
        * basic function in order to get particular info of a node
    */
    inline key_t* nth_key_n(byte *start, size_t n = 0) const
    {
        return (key_t *)(start + (sizeof(key_t) + sizeof(point)) * n);
    }
    inline key_t* nth_key_l(byte *start, size_t n = 0) const
    {
        return (key_t *)(start + (sizeof(key_t) + sizeof(value_type)) * n);
    }
    inline point* nth_point(byte *start, size_t n = 0) const
    {
        return (point *)(start + sizeof(key_t) * (n + 1) + sizeof(point) * n);
    }
    inline value_type* nth_value(byte *start, size_t n = 0) const
    {
        return (value_type *)(start + sizeof(key_t) * (n + 1) + sizeof(value_type) * n);
    }
    /*
        * binary search function to help find a particular child
    */
    inline size_t b_search_n(byte *start,const key_t& k, size_t n) const
    {
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) >> 1;
            if ( com(*nth_key_n(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        if (l >= n) return n - 1;
        else if (equal(*nth_key_n(start, l), k)) return l;
        else return l - 1;
    }
    inline size_t b_search_l(byte *start,const key_t& k, size_t n) const
    {
        size_t l = 0, r = n, mid;
        while (l < r){
            mid = (l + r) >> 1;
            if ( com(*nth_key_l(start, mid), k) ) l = mid + 1;
            else r = mid;
        }
        if (l >= n) return n - 1;
        else if (equal(*nth_key_l(start, l), k)) return l;
        else return l - 1;
    }
    /*
        * find a particular key and return its pointer
        * notice that as sub_find_t is returned, 
        * THE LEAF BLOCK IS STILL SHARED LOCKED
    */
    sub_find_t _find(buf_block_t *it,const key_t& k) const
    {
        size_t ord;
        byte *cache = it->frame + sizeof(node);
        node *p = (node *)(cache - sizeof(node));
        if (!p->type){
            ord = b_search_l(cache, k, p->size);
            if (equal(*nth_key_l(cache, ord), k)) {
                return sub_find_t(ord, it);
            }
            else {
                it->RW_latch.unlock_shared();
                return sub_find_t(false, nullptr);
            }
        }
        else{
            ord = b_search_n(cache, k, p->size);
            it->RW_latch.unlock_shared();
            buf_block_t *tmp = buf->load_it(*nth_point(cache, ord), 0);
            return _find(tmp, k);
        }
    }
    /*
        * return a continuous segment of key-value sets
    */
    vector<list_t> _listof(buf_block_t *it, const key_t &k, 
                            bool (*comp)(const key_t &a, const key_t &b)) const
    {
        size_t ord;
        buf_block_t *tmp;
        byte *cache = it->frame + sizeof(node);
        node *p = (node *)(cache - sizeof(node));
        if (!p->type){
            if (com(k, p->key)) ord = 0;
            else ord = b_search_l(cache, k, p->size);
            vector<list_t> ret;
            node *now = p;
            while ( !comp(k, *nth_key_l(cache, ord)) ){
                if (!comp(*nth_key_l(cache, ord), k) )
                ret.push_back(
                    list_t(*nth_key_l(cache, ord), 
                        *nth_value(cache, ord)
                    )
                );
                if (ord + 1 < now->size) ++ord;
                else{
                    if (now->next == invalid_p) break;
                    tmp->RW_latch.unlock_shared();
                    tmp = buf->load_it(now->next, 0);
                    now = (node *)(tmp->frame);
					cache = tmp->frame + sizeof(node);
                    ord = 0;
                }
            }
            tmp->RW_latch.unlock_shared();
            return ret;
        }
        else {
            if (com(k, p->key)) ord = 0;
            else ord = b_search_n(cache, k, p->size);
            it->RW_latch.unlock_shared();
            tmp = buf->load_it(*nth_point(cache, ord), 0);
            return _listof(tmp, k, comp);
        }
    }
    /*
        * Make the first of p become the last of l,
        * while the size of p equals to part_size and that of l is less. 
        * Save them both
    */
    inline void Lbalance_n(buf_block_t *it, buf_block_t *left)
    {
        byte *cache_l = left->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)),
             *l = (node *)(cache_l - sizeof(node));
        size_t ls = l->size;
        size_t mov = p->size - ((p->size + ls) >> 1);
        for (int i = 0;i < mov;i++){
            *nth_key_n(cache_l, ls + i) = *nth_key_n(cache_p, i);
            *nth_point(cache_l, ls + i) = *nth_point(cache_p, i);
        }
        l->size += mov, p->size -= mov;
        for (int i = 0;i < p->size;i++){
            *nth_key_n(cache_p, i) = *nth_key_n(cache_p, i + mov);
            *nth_point(cache_p, i) = *nth_point(cache_p, i + mov);
        }
        p->key = *nth_key_n(cache_p);
        buf->dirty(left), buf->dirty(it);
    }
    inline void Lbalance_l(buf_block_t *it, buf_block_t *left)
    {
        byte *cache_l = left->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)),
             *l = (node *)(cache_l - sizeof(node));
        size_t ls = l->size;
        size_t mov = p->size - ((p->size + ls) >> 1);
        for (int i = 0;i < mov;i++){
            *nth_key_l(cache_l, ls + i) = *nth_key_l(cache_p, i);
            *nth_value(cache_l, ls + i) = *nth_value(cache_p, i);
        }
        l->size += mov, p->size -= mov;
        for (int i = 0;i < p->size;i++){
            *nth_key_l(cache_p, i) = *nth_key_l(cache_p, i + mov);
            *nth_value(cache_p, i) = *nth_value(cache_p, i + mov);
        }
        p->key = *nth_key_l(cache_p);
        buf->dirty(left), buf->dirty(it);
        //print_node_l(it);print_node_l(left);
    }
    /*
        * Make the last of p become the first of r,
        * while the size of p equals to part_size and that of r is less. 
        * Save them both.
    */
    inline void Rbalance_n(buf_block_t *it, buf_block_t *right)
    {
        byte *cache_r = right->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)), 
             *r = (node *)(cache_r - sizeof(node));
        size_t mov = p->size - ((p->size + r->size) >> 1);
        for (size_t i = r->size + mov - 1;mov - 1 < i;i--){
            *nth_key_n(cache_r, i)  = *nth_key_n(cache_r, i - mov);
            *nth_point(cache_r, i)  = *nth_point(cache_r, i - mov);
        }
        r->size += mov, p->size -= mov;
        size_t ps = p->size;
        for (size_t i = 0;i < mov;i++){
            *nth_key_n(cache_r, i)  = *nth_key_n(cache_p, ps + i);
            *nth_point(cache_r, i)  = *nth_point(cache_p, ps + i);
        }
        r->key = *nth_key_n(cache_r, 0);
        buf->dirty(it), buf->dirty(right);
    }
    inline void Rbalance_l(buf_block_t *it, buf_block_t *right)
    {
        byte *cache_r = right->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)), 
             *r = (node *)(cache_r - sizeof(node));
        size_t mov = p->size - ((p->size + r->size) >> 1);
        for (size_t i = r->size + mov - 1;mov - 1 < i;i--){
            *nth_key_l(cache_r, i)  = *nth_key_l(cache_r, i - mov);
            *nth_value(cache_r, i)  = *nth_value(cache_r, i - mov);
        }
        r->size += mov, p->size -= mov;
        size_t ps = p->size;
        for (size_t i = 0;i < mov;i++){
            *nth_key_l(cache_r, i)  = *nth_key_l(cache_p, ps + i);
            *nth_value(cache_r, i)  = *nth_value(cache_p, ps + i);
        }
        r->key = *nth_key_l(cache_r, 0);
        buf->dirty(it), buf->dirty(right);
    }
    /*
        * Receive a node from the left part if avaliable
        * Save them both
    */
    inline void Lreceive_n(buf_block_t *it, buf_block_t *left)
    {
        byte *cache_l = left->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)),
             *l = (node *)(cache_l - sizeof(node));
        for (size_t i = p->size;i;i--){
            *nth_key_n(cache_p, i)  = *nth_key_n(cache_p, i - 1);
            *nth_point(cache_p, i)  = *nth_point(cache_p, i - 1);
        }
        ++(p->size);--(l->size);
        p->key = *nth_key_n(cache_p, 0) = *nth_key_n(cache_l, l->size);
        *nth_point(cache_p)             = *nth_point(cache_l, l->size);
        buf->dirty(it), buf->dirty(left);
    }
    inline void Lreceive_l(buf_block_t *it, buf_block_t *left)
    {
        byte *cache_l = left->frame + sizeof(node),
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)),
             *l = (node *)(cache_l - sizeof(node));
        for (size_t i = p->size;i;i--){
            *nth_key_l(cache_p, i) = *nth_key_l(cache_p, i - 1);
            *nth_value(cache_p, i) = *nth_value(cache_p, i - 1);
        }
        ++(p->size);--(l->size);
        p->key = *nth_key_l(cache_p, 0) = *nth_key_l(cache_l, l->size);
        *nth_value(cache_p)             = *nth_value(cache_l, l->size);
        buf->dirty(it), buf->dirty(left);
    }
    /*
        * Receive a node from the right part if avaliable. 
        * Save them both
    */
    inline void Rreceive_n(buf_block_t *it, buf_block_t *right)
    {
        byte *cache_r = right->frame + sizeof(node), 
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)), 
             *r = (node *)(cache_r - sizeof(node));
        *nth_key_n(cache_p, p->size) = *nth_key_n(cache_r);
        *nth_point(cache_p, p->size) = *nth_point(cache_r);
        ++(p->size), --(r->size);
        r->key = *nth_key_n(cache_r, 1);
        for (size_t i = 0;i < r->size;i++){
            *nth_key_n(cache_r, i) = *nth_key_n(cache_r, i + 1);
            *nth_point(cache_r, i) = *nth_point(cache_r, i + 1);
        }
        buf->dirty(it), buf->dirty(right);
    }
    inline void Rreceive_l(buf_block_t *it, buf_block_t *right)
    {
        byte *cache_r = right->frame + sizeof(node), 
             *cache_p = it->frame + sizeof(node);
        node *p = (node *)(cache_p - sizeof(node)), 
             *r = (node *)(cache_r - sizeof(node));
        *nth_key_l(cache_p, p->size) = *nth_key_l(cache_r, 0);
        *nth_value(cache_p, p->size) = *nth_value(cache_r);
        ++(p->size), --(r->size);
        r->key = *nth_key_l(cache_r, 1);
        for (size_t i = 0;i < r->size;i++){
            *nth_key_l(cache_r, i) = *nth_key_l(cache_r, i + 1);
            *nth_value(cache_r, i) = *nth_value(cache_r, i + 1);
        }
        buf->dirty(it), buf->dirty(right);
    }
    /*
        * Find the quickest way to solve the problem of extreme size.
        * save now and cache and left/right and its cache if necessary, 
        * the parent is also dirty there
    */
    inline void deal_surplus_n(buf_block_t *now_b, buf_block_t *par_b, size_t ord)
    {
        byte *cache     = now_b->frame + sizeof(node), 
             *cache_par = par_b->frame + sizeof(node);
        node *now = (node *)(cache     - sizeof(node)), 
             *par = (node *)(cache_par - sizeof(node));
        node *tmp;
        buf_block_t *tmp_b;
        if (now->pos != *nth_point(cache_par)){
            tmp_b = buf->load_it(now->prior, 1);
            tmp = (node *)(tmp_b->frame);
            if (tmp->size < part_size_n * 3 / 4) {
                Lbalance_n(now_b, tmp_b);
                *nth_key_n(cache_par, ord) = now->key;
                buf->dirty(par_b);
                tmp_b->RW_latch.unlock();
                return;
            }
            tmp_b->RW_latch.unlock();
        }
        if (now->pos != *nth_point(cache_par, par->size - 1)){
            tmp_b = buf->load_it(now->next, 1);
            tmp = (node *)(tmp_b->frame);
            if (tmp->size < part_size_n * 3 / 4){
                Rbalance_n(now_b, tmp_b);
                *nth_key_n(cache_par, ord + 1) = tmp->key;
                buf->dirty(par_b);
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                tmp_b->RW_latch.unlock();
                split_n(now_b, par_b, ord);
            }
        }
        else split_n(now_b, par_b, ord);
    }
    inline void deal_surplus_l(buf_block_t *now_b, buf_block_t *par_b, size_t ord)
    {
        byte *cache     = now_b->frame + sizeof(node), 
             *cache_par = par_b->frame + sizeof(node);
        node *now = (node *)(cache     - sizeof(node)), 
             *par = (node *)(cache_par - sizeof(node));
        node *tmp;
        buf_block_t *tmp_b;
        if (now->pos != *nth_point(cache_par)){
            tmp_b = buf->load_it(now->prior, 1);
            tmp = (node *)(tmp_b->frame);
            if (tmp->size < part_size_l * 3 / 4) {
                Lbalance_l(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord), now->key)){
                    *nth_key_n(cache_par, ord) = now->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            tmp_b->RW_latch.unlock();
        }
        if (now->pos != *nth_point(cache_par, par->size - 1)){
            tmp_b = buf->load_it(now->next, 1);
            tmp = (node *)(tmp_b->frame);
            if (tmp->size < part_size_l * 3 / 4){
                Rbalance_l(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord + 1), tmp->key)){
                    *nth_key_n(cache_par, ord + 1) = tmp->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                tmp_b->RW_latch.unlock();
                split_l(now_b, par_b, ord);
            }
        }
        else split_l(now_b, par_b, ord);
    }
    inline void deal_deficit_n(buf_block_t *now_b, buf_block_t *par_b, size_t ord)
    {
        byte *cache     = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node);
        node *now = (node *)(cache     - sizeof(node)),
             *par = (node *)(cache_par - sizeof(node));
        node *tmp;
        buf_block_t *tmp_b;
        if (now->pos != *nth_point(cache_par, par->size - 1)){
            tmp_b = buf->load_it(now->next, 1);
            tmp = (node *)(tmp_b->frame);
            if ((part_size_n >> 1) < tmp->size){
                Rreceive_n(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord + 1), tmp->key)){
                    *nth_key_n(cache_par, ord + 1) = tmp->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                merge_n(now_b, par_b, tmp_b, ord);
                tmp_b->RW_latch.unlock();
                return;
            }
        }
        if (now->pos != *nth_point(cache_par)){
            tmp_b = buf->load_it(now->prior, 1);
            tmp = (node *)(tmp_b->frame);
            if ((part_size_n >> 1) < tmp->size){
                Lreceive_n(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord), now->key)){
                    *nth_key_n(cache_par, ord) = now->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                merge_n(tmp_b, par_b, now_b, ord - 1);
                tmp_b->RW_latch.unlock();
            }
        }
    }
    inline void deal_deficit_l(buf_block_t *now_b, buf_block_t *par_b, size_t ord)
    {
        byte *cache     = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node);
        node *now = (node *)(cache - sizeof(node)),
             *par = (node *)(cache_par - sizeof(node));
        node *tmp;
        buf_block_t *tmp_b;
        if (now->pos != *nth_point(cache_par, par->size - 1)){
            tmp_b = buf->load_it(now->next, 1);
            tmp = (node *)(tmp_b->frame);
            if ((part_size_l >> 1) < tmp->size){
                Rreceive_l(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord + 1), tmp->key)){
                    *nth_key_n(cache_par, ord + 1) = tmp->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                merge_l(now_b, par_b, tmp_b, ord);
                tmp_b->RW_latch.unlock();
                return;
            }
        }
        if (now->pos != *nth_point(cache_par)){
            tmp_b = buf->load_it(now->prior, 1);
            tmp = (node *)(tmp_b->frame);
            if ((part_size_l >> 1) < tmp->size){
                Lreceive_l(now_b, tmp_b);
                if (!equal(*nth_key_n(cache_par, ord), now->key)){
                    *nth_key_n(cache_par, ord) = now->key;
                    buf->dirty(par_b);
                }
                tmp_b->RW_latch.unlock();
                return;
            }
            else {
                merge_l(tmp_b, par_b, now_b, ord - 1);
                tmp_b->RW_latch.unlock();
            }
        }
    }
    /*
        * Split the cache belonging to node now. 
        * save now and its cache but do not save any info of parent into storage,
        * though we have changed it in memory
    */
    void split_n(buf_block_t *now_b, buf_block_t *par_b, size_t order)
    {
        byte *cache = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node)), 
             *par   = (node *)(cache_par - sizeof(node));
        size_t s    = now->size >> 1;
        now->size -= s;
        size_t ns = now->size;

        point pos = alloc.alloc(node_size);
        buf_block_t *tmp_b = buf->load_it(pos, 1);

        byte *cache_tmp = tmp_b->frame + sizeof(node);
        node *tmp       = (node *)(cache_tmp - sizeof(node));
        tmp->key = *nth_key_n(cache, ns), tmp->type = now->type, tmp->pos = pos, tmp->size = s;
        tmp->prior = now->pos, tmp->next = now->next;

        if (now->next != invalid_p){
            buf_block_t *temp = buf->load_it(now->next, 1);
            ((node *)(temp->frame))->prior = tmp->pos;
            buf->dirty(temp);
            temp->RW_latch.unlock();
        }
        now->next = tmp->pos;

        for (size_t i = 0;i < s;i++){
            *nth_key_n(cache_tmp, i) = *nth_key_n(cache, ns + i);
            *nth_point(cache_tmp, i) = *nth_point(cache, ns + i);
        }
        buf->dirty(tmp_b), buf->dirty(now_b);
        tmp_b->RW_latch.unlock();
        ns = order;
        for (size_t i = par->size;ns + 1 < i;i--){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i - 1);
            *nth_point(cache_par, i)    = *nth_point(cache_par, i - 1);
        }
        *nth_key_n(cache_par, ns + 1) = tmp->key;
        *nth_point(cache_par, ns + 1) = tmp->pos;
        ++(par->size);
        buf->dirty(par_b);
    }
    void split_l(buf_block_t *now_b, buf_block_t *par_b, size_t order){
        byte *cache     = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node)), 
             *par   = (node *)(cache_par - sizeof(node));
        size_t s    = now->size >> 1;
        now->size -= s;
        size_t ns = now->size;

        point pos           = alloc.alloc(node_size);
        buf_block_t *tmp_b  = buf->load_it(pos, 1);
        byte *cache_tmp     = tmp_b->frame + sizeof(node);
        node *tmp           = (node *)(cache_tmp - sizeof(node));
        tmp->key    = *nth_key_l(cache, ns), tmp->type = now->type, tmp->pos = pos, tmp->size = s;
        tmp->prior  = now->pos, tmp->next = now->next;

        if (now->next != invalid_p){
            buf_block_t *temp = buf->load_it(now->next, 1);
            ((node *)(temp->frame))->prior = tmp->pos;
            buf->dirty(temp);
            temp->RW_latch.unlock();
        }
        now->next = tmp->pos;

        for (size_t i = 0;i < s;i++){
            *nth_key_l(cache_tmp, i) = *nth_key_l(cache, ns + i);
            *nth_value(cache_tmp, i) = *nth_value(cache, ns + i);
        }
        buf->dirty(tmp_b), buf->dirty(now_b);
        tmp_b->RW_latch.unlock();
        ns = order;
        for (size_t i = par->size;ns + 1 < i;i--){
            *nth_key_n(cache_par, i)    = *nth_key_n(cache_par, i - 1);
            *nth_point(cache_par, i)    = *nth_point(cache_par, i - 1);
        }
        *nth_key_n(cache_par, ns + 1) = tmp->key;
        *nth_point(cache_par, ns + 1) = tmp->pos;
        ++(par->size);
        buf->dirty(par_b);
    }
    /*
        * Merge now and the next of it,
        * if the result is too big, use split automatically.
        * save now and cache, but not par or cache_par
    */
    void merge_n(buf_block_t *now_b, buf_block_t *par_b, buf_block_t *tmp_b, size_t s)
    {
        byte *cache     = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node),
             *cache_tmp = tmp_b->frame + sizeof(node); 
        node *now = (node *)(cache - sizeof(node)), 
             *par = (node *)(cache_par - sizeof(node)),
             *tmp = (node *)(cache_tmp - sizeof(node));
        now->next = tmp->next;
        if (tmp->next != invalid_p){
            buf_block_t *temp = buf->load_it(tmp->next, 1);
            ((node *)(temp->frame))->prior = now->pos;
            buf->dirty(temp);
            temp->RW_latch.unlock();
        }
        size_t ns = now->size;
        for (size_t i = 0;i < tmp->size;i++){
            *nth_key_n(cache, i + ns) = *nth_key_n(cache_tmp, i);
            *nth_point(cache, i + ns) = *nth_point(cache_tmp, i);
        }
        now->size += tmp->size;
        alloc.free(tmp->pos, node_size);
        --(par->size);
        for (size_t i = s + 1;i < par->size;i++){
            *nth_key_n(cache_par, i) = *nth_key_n(cache_par, i + 1);
            *nth_point(cache_par, i) = *nth_point(cache_par, i + 1);
        }
        if (now->size >= part_size_n)
            split_n(now_b, par_b, s);
        buf->dirty(now_b), buf->dirty(par_b);
    }
    void merge_l(buf_block_t *now_b, buf_block_t *par_b, buf_block_t *tmp_b, size_t s)
    {
        byte *cache     = now_b->frame + sizeof(node),
             *cache_par = par_b->frame + sizeof(node),
             *cache_tmp = tmp_b->frame + sizeof(node); 
        node *now = (node *)(cache - sizeof(node)), 
             *par = (node *)(cache_par - sizeof(node)),
             *tmp = (node *)(cache_tmp - sizeof(node));
        now->next = tmp->next;
        if (tmp->next != invalid_p){
            buf_block_t *temp = buf->load_it(tmp->next, 1);
            ((node *)(temp->frame))->prior = now->pos;
            buf->dirty(temp);
            temp->RW_latch.unlock();
        }
        size_t ns = now->size;
        for (size_t i = 0;i < tmp->size;i++){
            *nth_key_l(cache, i + ns) = *nth_key_l(cache_tmp, i);
            *nth_value(cache, i + ns) = *nth_value(cache_tmp, i);
        }
        now->size += tmp->size;
        alloc.free(tmp->pos, node_size);
        --(par->size);
        for (size_t i = s + 1;i < par->size;i++){
            *nth_key_n(cache_par, i)  = *nth_key_n(cache_par, i + 1);
            *nth_point(cache_par, i)  = *nth_point(cache_par, i + 1);
        }
        if (now->size >= part_size_l) split_n(now_b, par_b, s);
        buf->dirty(now_b), buf->dirty(par_b);
    }
    /*
        * Insert (k,v) and return true if it is an insertion and false for a change, 
        * whether split or not is considered in its parent,
        * as we prove that all parts are smaller than part_size, the memory is safe.
        * return 0 when it fails, 1 when the successfully-inserted 'it' does not oversized and 2 otherwise.
        * that means, only deal_surplus is considered in its parent, dirty or not is considered in itself, 
        * requiring the deal_surplus function dirty the parent
    */
    int _insert_n(buf_block_t *it, const key_t &k, const value_type &v)
    {
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        size_t ord  = b_search_n(cache, k, now->size);
        point loc   = *nth_point(cache, ord);
        buf_block_t *buf_child  = buf->load_it(loc, 1);    
        byte *cache_child       = buf_child->frame + sizeof(node);
        node *node_child        = (node *)(cache_child - sizeof(node));
        int ret;
        if (node_child->type){
            ret = _insert_n(buf_child, k, v);
            if (!ret) {
                buf_child->RW_latch.unlock();
                return 0;
            }
            if (ret == 2) {
                deal_surplus_n(buf_child, it, ord);
                if (now->size >= part_size_n) ret = 2;
                else ret = 1;
            }
        }
        else{
            ret = _insert_l(buf_child, k, v);
            if (!ret) {
                buf_child->RW_latch.unlock();
                return 0;
            }
            if (ret == 2) {
                deal_surplus_l(buf_child, it, ord);
                if (now->size >= part_size_n) ret = 2;
                else ret = 1;
            }
            else buf->dirty(buf_child);
        }
        buf_child->RW_latch.unlock();
        return ret;
    }
    int _insert_l(buf_block_t *it, const key_t &k, const value_type &v)
    {
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        size_t ord  = b_search_l(cache, k, now->size);
        if (equal(*nth_key_l(cache, ord), k)) return 0;
        ++num;
        for (size_t i = now->size;ord + 1 < i;i--){
            *nth_key_l(cache, i)    = *nth_key_l(cache, i - 1);
            *nth_value(cache, i)    = *nth_value(cache, i - 1);
        }
        ++(now->size);
        *nth_key_l(cache, ord + 1) = k;
        *nth_value(cache, ord + 1) = v;
        if (now->size >= part_size_l) return 2;
        return 1;
    }
    /*
        * Insert (k,v) at the head of the tree and return true, 
        * whether split or not is considered in its parent,
        * as we prove that all parts are smaller than part_size, the memory is safe.
        * But mention to judge the size of the root.
    */
    void _insert_head_n(buf_block_t *it, const key_t &k, const value_type &v)
    {
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        buf_block_t *buf_child  = buf->load_it(*nth_point(cache), 1);    
        byte *cache_child       = buf_child->frame + sizeof(node);
        node *node_child        = (node *)(cache_child - sizeof(node));
        if (node_child->type){
            _insert_head_n(buf_child, k, v);
            *nth_key_n(cache, 0) = now->key = k;
            if (node_child->size >= part_size_n){
                deal_surplus_n(buf_child, it, 0);
            }
            else buf->dirty(buf_child);
        }
        else{
            _insert_head_l(buf_child, k, v);
            *nth_key_l(cache, 0) = now->key = k;
            if (node_child->size >= part_size_l){
                deal_surplus_l(buf_child, it, 0);
            }
            else buf->dirty(buf_child);
        }
        buf_child->RW_latch.unlock();
    }
    void _insert_head_l(buf_block_t *it, const key_t &k, const value_type &v)
    {
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        ++num;
        for (size_t i = now->size;i;i--){
            *nth_key_l(cache, i)    = *nth_key_l(cache, i - 1);
            *nth_value(cache, i)    = *nth_value(cache, i - 1);
        }
        ++(now->size);
        *nth_key_l(cache, 0) = now->key = k;
        *nth_value(cache, 0) = v;
    }
    /*
        * remove (k,v) and return true if it does remove k and false when it cannot find k, 
        * whether merge or not is considered in its parent,
        * thus a merge is save unless the part_size is greater than 3.
        * But mention to judge the size of the root.
    */
    bool _remove_n(buf_block_t *it, const key_t &k){
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        size_t ord  = b_search_n(cache, k, now->size);
        point loc   = *nth_point(cache, ord);
        buf_block_t *buf_child  = buf->load_it(loc, 1);    
        byte *cache_child       = buf_child->frame + sizeof(node);
        node *node_child        = (node *)(cache_child - sizeof(node));
        bool ret;
        if (node_child->type){
            ret = _remove_n(buf_child, k);
            if (!ret) return false;
            *nth_key_n(cache, ord) = node_child->key;
            if (node_child->size < (part_size_n >> 1)){
                deal_deficit_n(buf_child, it, ord);
            }
            else buf->dirty(buf_child);
        }
        else{
            ret = _remove_l(buf_child, k);
            if (!ret) return false;
            *nth_key_n(cache, ord) = node_child->key;
            if (node_child->size < (part_size_l >> 1)){
                deal_deficit_l(buf_child, it, ord);
            }
            else buf->dirty(buf_child);
        }
        now->key = *nth_key_n(cache, 0);
        buf_child->RW_latch.unlock();
        return ret;
    }
    bool _remove_l(buf_block_t *it, const key_t k){
        byte *cache = it->frame + sizeof(node);
        node *now   = (node *)(cache - sizeof(node));
        size_t ord  = b_search_l(cache, k, now->size);
        if (equal(*nth_key_l(cache, ord), k)){
            --num;
            --(now->size);
            for (size_t i = ord;i < now->size;i++){
                *nth_key_l(cache, i)    = *nth_key_l(cache, i + 1);
                *nth_value(cache, i)    = *nth_value(cache, i + 1);
            }
            now->key = *nth_key_l(cache, 0);
            return true;
        }
        else return false;
    }
public:
    bplustree():part_size_n((node_size - sizeof(node)) / (sizeof(key_t) + sizeof(point))),
	            part_size_l((node_size - sizeof(node)) / (sizeof(key_t) + sizeof(value_type))), 
                num(0), root_pos(0), buf(nullptr){}
    void init(const char *datafile_name, const char *alloc_name)
    {
        buf         = new buf_pool_t<node_size, node>;
        index_name  = new char[strlen(alloc_name) + 1];
        strcpy(index_name, alloc_name);
        data_name   = new char[strlen(datafile_name) + 1];
        strcpy(data_name, datafile_name);
        alloc.init(index_name);
        datafile    = fopen(data_name, "rb+");
        if (!datafile)
            datafile = fopen(data_name, "wb+");
        buf->init(datafile);
		#ifdef OUTPUT_INIT
        printf("size_of_node: %d; leaf_part: %d; non_leaf_part: %d\n", sizeof(node), part_size_l, part_size_n);
        printf("ini_root_point_seek: %d\n", 0);
		#endif
        if (!fread(&root_pos, sizeof(point), 1, datafile)){
            alloc.alloc(node_size);
            root_pos = alloc.alloc(node_size);
            node tmp;
            fseek(datafile, root_pos, SEEK_SET);
            fwrite(&tmp, sizeof(node), 1, datafile);
        }
        else{
            fread(&num, sizeof(size_t), 1, datafile);
        }
    }
    void clear()
    {
        if (datafile) fclose(datafile);
        datafile = fopen(data_name, "wb+");
        alloc.refill(index_name);
        num = 0;
        alloc.alloc(node_size);
        root_pos = alloc.alloc(node_size);
        node tmp;
        fseek(datafile, root_pos, SEEK_SET);
        fwrite(&tmp, sizeof(node), 1, datafile);
		#ifdef DEBUG_MODE
        printf("clear_root_point_seek: %d\n", 0);
		#endif
        buf->stop_it();
        buf->file_change(datafile);
        buf->re_init();
		#ifdef DEBUG_MODE
		printf("clear_root_seek: %d\n", 0);
            #ifdef TEST_INT_MODE
            printf("which is: pos:%d size:%d key:%s\n", root.pos, root.size, root.key.a);
		    #endif
            #ifdef TEST_STRING_MODE
            printf("which is: pos:%d size:%d key:%s\n", root.pos, root.size, root.key.inf);
            #endif
		#endif
    }
    bool empty() const
    {
        return !num;
    }
    ~bplustree()
    {
        alloc.save(index_name);
        fseek(datafile, 0, SEEK_SET);
		#ifdef DEBUG_MODE
        printf("decode_root_point_seek: %d\nwhich is: %d\n", 0, root_pos);printf("which is: %d\n", root_pos);
		#endif
        fwrite(&root_pos, sizeof(point), 1, datafile);
        fwrite(&num, sizeof(size_t), 1, datafile);
		if (index_name != nullptr)        
			delete []index_name;
		if (data_name != nullptr)
			delete []data_name;
        delete buf;
		if (datafile) fclose(datafile);
    }
    bool count(const key_t &k) const
    {
        if (empty()) return 0;
        buf_block_t *root = buf->load_it(root_pos, 0);
        if (com(k, ((node *)(root->frame))->key)) return 0;
        sub_find_t p = _find(root, k);
        root->RW_latch.unlock_shared();
        if (p.second == nullptr) return 0;
        p.second->RW_latch.unlock_shared();
        return 1;
    }
    find_t find(const key_t &k) const
    {
		if (empty()) return find_t(false,value_type());
        buf_block_t *root = buf->load_it(root_pos, 0);
        if (com(k, ((node *)(root->frame))->key)) return find_t(false, value_type());
        sub_find_t p = _find(root, k);
        root->RW_latch.unlock_shared();
        if (p.second == nullptr) return find_t(false, value_type());
		#ifdef DEBUG_MODE
        printf("find_seek_in_database: %d\nwhich is: %d\n", p + sizeof(key_t),v);
		#endif
        value_type v = *nth_value(p.second->frame + sizeof(node), p.first);
        p.second->RW_latch.unlock_shared();
        return find_t(true, v);
    }
    bool set(const key_t &k, const value_type &v) const
    {
        if (empty()) throw(container_is_empty());
        buf_block_t *root = buf->load_it(root_pos, 0);
        if (com(k, ((node *)(root->frame))->key)) return false;
        sub_find_t p = _find(root, k);
        root->RW_latch.unlock_shared();
        if (p.second == nullptr) return false;
        p.second->RW_latch.unlock_shared();
        p.second->RW_latch.lock()
        *nth_value(p.second->frame + sizeof(node), p.first) = v;
        buf->dirty(p.second);
        p.second->RW_latch.unlock();
        return 1;
    }
    bool insert(const key_t &k, const value_type &v)
    {
        buf_block_t *root = buf->load_it(root_pos, 1);
        node *root_n = (node *)(root->frame);
        if (root_n->pos == invalid_p){
            if (!datafile) throw(runtime_error());
            root_n->pos     = root_pos, root_n->size    = num = 1;
            root_n->key     = k,        root_n->type    = 0;
            root_n->prior   = root_n->next = invalid_p;
            *nth_key_l(root->frame + sizeof(node)) = k;
            *nth_value(root->frame + sizeof(node)) = v;
            buf->dirty(root);
            root->RW_latch.unlock();
            return true;
        }
        int ret = 1;
        byte *cache = root->frame + sizeof(node);
        if (root_n->type){
            if (com(root_n->key, k))
                ret = _insert_n(root, k, v);
            else {
                ret = 2;
                _insert_head_n(root, k, v);
            }
            if (ret == 2){
                buf->dirty(root);
                if (root_n->size >= part_size_n){
                    point pos = alloc.alloc(node_size);
                    buf_block_t *new_root_b = buf->load_it(pos, 1);
                    byte *cache_new_root = new_root_b->frame + sizeof(node);
                    node *new_root = (node *)(cache_new_root - sizeof(node));
                    new_root->key = root_n->key,    new_root->size = 1;
                    new_root->pos = pos,            new_root->type = 1;
                    new_root->prior = new_root->next = invalid_p;
                    *nth_key_n(cache_new_root) = root_n->key;
                    *nth_point(cache_new_root) = root_n->pos;
                    split_n(root, new_root_b, 0); 
                    root_pos = new_root->pos;
                    new_root_b->RW_latch.unlock();
                }
            }
        }
        else{
            if (com(root_n->key, k))
                ret = _insert_l(root, k, v);
            else {
                ret = 2;
                _insert_head_l(root, k, v);
            }
            if (ret == 2){
                buf->dirty(root);
                if (root_n->size >= part_size_l){
                    point pos = alloc.alloc(node_size);
                    buf_block_t *new_root_b = buf->load_it(pos, 1);
                    byte *cache_new_root = new_root_b->frame + sizeof(node);
                    node *new_root = (node *)(cache_new_root - sizeof(node));
                    new_root->key = root_n->key, new_root->size = 1;
                    new_root->pos = pos, new_root->type = 1;
                    new_root->prior = new_root->next = invalid_p;
                    *nth_key_n(cache_new_root) = root_n->key;
                    *nth_point(cache_new_root) = root_n->pos;
                    split_l(root, new_root_b, 0);
                    root_pos = new_root->pos;
                    new_root_b->RW_latch.unlock();
                }
            }
        }
        root->RW_latch.unlock();
        return ret;
    }
    bool remove(const key_t &k)
    {
        if (empty()) return false;
        buf_block_t *root_b = buf->load_it(root_pos, 1);
        byte *cache = root_b->frame + sizeof(node);
        node *root  = (node *)(cache - sizeof(node));
        if (com(k, root->key)) return 0;
        bool ret = true;
        if (root->type) {
            ret = _remove_n(root_b, k);
            if (!ret) {
                root_b->RW_latch.unlock();
                return 0;
            }
            if (root->size < 2){
                alloc.free(root->pos, node_size);
                root_b->RW_latch.unlock();
                root_b      = buf->load_it(*nth_point(cache), 1);
                root_pos    = ((node *)(root_b->frame))->pos;
            }
            else buf->dirty(root_b);
        }
        else {
            ret = _remove_l(root_b, k);
            if (!ret) {
                root_b->RW_latch.unlock();
                return 0;
            }
            buf->dirty(root_b);
        }
        root_b->RW_latch.unlock();
        return ret;
    }
    vector<list_t> listof(key_t k, bool (*comp)(const key_t &a, const key_t &b)) const
    {
        if (empty()) return vector<list_t>();
        buf_block_t *root_b = buf->load_it(root_pos, 0);
        return _listof(root_b, k, comp);
    }
    int size() const
    {
        return num;
    }
    void double_check()
    {
        buf->check_hash();
        //buf->check_flush();
        buf->check_LRU();
    }
};

#endif
