//in this version, free list has no use, since the only element to free is mutex.
//in this version, old_list has no use, only a simple LRU is done.(all elements are moved to HIR)
//some re-consider needed
//in the next version, a queue is needed in order to change LRU to LIRS
#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP

//node of a list
#include <iostream>
#include <stdio.h>
#include "hash.hpp"
template<typename TYPE>
struct ut_list_node{
	TYPE* prev;
	TYPE* next;
    ut_list_node(TYPE* p = nullptr, TYPE* n = nullptr):prev(p), next(n){}
};
//head of a list
template<typename TYPE>
struct ut_list_head{
	size_t count;
	TYPE* start;
	TYPE* end;
    ut_list_head(TYPE* p = nullptr, TYPE* n = nullptr, size_t c = 0):
        start(p), end(n), count(c){}
};
//block of a buffer
struct buf_block_t{
    using byte = char;
    byte                        *frame;
    size_t                      offset;

    //state = 0 when it is free, 1 when it is in HIR, 2 for in LIR and 3, 4 for it is dirty in such a list
    size_t                      state;
    //all info about hash
    int                         hash_value;
    buf_block_t                 *hash_next;
    //its location at three list
    ut_list_node<buf_block_t>   free, LRU, flush;
    size_t                      IRR, recency;

    buf_block_t(byte *f = nullptr):frame(f), offset(0), 
                                state(0), IRR(0), recency(),
                                hash_value(0), hash_next(nullptr),
                                free(), LRU(), flush(){}
    ~buf_block_t(){
        delete frame;
    }
};
template<size_t BUFFER_SIZE = 4096, size_t TOTAL_NUM = 2, size_t COLD_PERCENTAGE = 25>
class buf_pool_t{
    using byte = char;
    byte                                *mem_head;
    ut_list_head<buf_block_t>           free, LRU, flush;
    size_t                              clock;
    hash_table_t<buf_block_t*>          hash_table;
    buf_block_t                         *HIR_head;
    FILE                                *f;
    
    /*
        * a basic function in order to know whether the needed key exists.
        * even the page is found, its clock is not changed
    */
    //done
    buf_block_t *_find(const size_t &pos){
        buf_block_t *temp = hash_table.find(pos);
        while(temp != nullptr && temp->offset != pos) temp = temp->hash_next;
        return temp;
    }
    /*
        * get the oldest avaliable page in LRU_list
    */
    buf_block_t *_LRU_oldest(){
        buf_block_t *tmp = LRU.end;
        while(tmp != nullptr){
            if (tmp->state < 3) break;
            tmp = (tmp->LRU).prev;
        }
        return tmp;
    }
    //the following functions are movements of LRU-list: insert or remove
    /*
        * pick out a block from the LRU-list.(simply change the pointer to it)
        * HIR_head is considered
    */
    inline void _pick_out_LRU(buf_block_t &p){
        if (&p == HIR_head) HIR_head = p.LRU.next;
        if (p.LRU.next != nullptr) (p.LRU.next)->LRU.prev = p.LRU.prev;
        else {
            if (p.LRU.prev != nullptr) LRU.end = p.LRU.prev;
        }
        if (p.LRU.prev != nullptr) (p.LRU.prev)->LRU.next = p.LRU.next;
        else {
            if (p.LRU.next != nullptr) LRU.start = p.LRU.next;
        }
        --LRU.count;
    }
    /*
        * can only be used while p is a clean buf_block not in LRU. 
        * get info from the storage, and move it to LRU-old-head.
        * DO NOT CHANGE LRU.COUNT BUT P.STATE = 1.
    */
    void _to_HIR(buf_block_t *p){
        p->state = 1;
        if (HIR_head != nullptr) {
            (p->LRU).prev = (HIR_head->LRU).prev;
            (p->LRU).next = HIR_head;
            if (HIR_head = LRU.start) LRU.start = p;
            HIR_head = p;
            return;
        }
        else{
            //all nodes are young or no node exists
            if (LRU.end != nullptr){
                (p->LRU).prev = LRU.end;
                HIR_head = LRU.end = p;
            }
            else{
                LRU.start = LRU.end = HIR_head = p;
            }
        }
    }
        /*
            * certainly p must be in the lru list before this.(young or old)
            * make it the first of lru-list as well as a LIR one. 
            * re-consider-needed: pointer lru-old; oldest in LIR set 
        */
        //done; re-consider needed
        // void _to_LIR(buf_block_t &p){
        //     _pick_out_LRU(p);
        //     ++LRU.count;
        //     if ((p.state == 1) || (p.state == 3)) ++p.state;     
        //     p.LRU.prev = nullptr, p.LRU.next = LRU.start;
        //     LRU.start = &p;
        //     p.recency = ++clock;
        //     //to check if the oldest in LIR set should be removed to the cold list(HIR)?
        //     //or instead of cold list, flush it?
        // }
    /*
        * make it the last of LRU-list.
    */
    void _to_LRU_HIR_end(buf_block_t &p){
        _pick_out_LRU(p);
        ++LRU.count;
        if (p.state == 2 || p.state == 4) --p.state;
        p.LRU.next = nullptr, p.LRU.prev = LRU.end;
        if (HIR_head == nullptr) HIR_head = &p;
        if (LRU.start == nullptr) LRU.start = &p;
        LRU.end = &p;
    }
        // void _to_LRU_LIR_end(buf_block_t &p){
        //     _pick_out_LRU(p);
        //     ++LRU.count;
        //     if (p.state == 1 || p.state == 3) ++p.state;
        //     if (HIR_head == nullptr) {
        //         p.LRU.prev = LRU.end;
        //         LRU.end = &p;
        //         if (LRU.start == nullptr) LRU.start = &p;
        //     }
        //     else {
        //         p.LRU.prev = HIR_head->LRU.prev;
        //         if (LRU.start == HIR_head) LRU.start = &p;
        //     }
        //     p.LRU.next = HIR_head;
        // }
    //the following are those involving i/o
    /*
        * read info and make the page the HIR_head
        * DO NOT CHANGE LRU.COUNT
    */
    void _read_inf(buf_block_t *it, const size_t &pos, FILE *f){
        fseek(f, pos, SEEK_SET);
        fread(it->frame, 1, BUFFER_SIZE, f);
        it->state = 1, it->offset = pos;
        it->hash_value = hash_table.get_value(pos);
        it->hash_next = nullptr;
        buf_block_t *temp = hash_table.find(pos);
        if (temp == nullptr) it->hash_next = temp;
        hash_table.insert(pos, it);
        _to_HIR(it);
    }
    /*
        * fulfill a free page and move it to the LRU-list
    */
    //done
    buf_block_t *free_use(const size_t &pos, FILE *f){
        buf_block_t *tmp = free.start;
        free.start = (tmp->free).next;
        (tmp->free).next = free.start->free.prev = nullptr; // do we really need this sentence?
        if (free.start == nullptr) free.end = nullptr;
        --free.count;
        _read_inf(tmp, pos, f);
        ++LRU.count;
        return tmp;
    }
    /*
        * rewrite a page in LRU_page and make it the HIR_head
    */
    buf_block_t *LRU_use(const size_t &pos, FILE *f){
        buf_block_t *tmp = _LRU_oldest();
        buf_block_t *pre = hash_table.find(pos);
        if (pre == tmp){
            hash_table.insert(pos, tmp->hash_next);
        }
        else{
            while(pre->hash_next != tmp) pre = pre->hash_next;
            pre->hash_next = tmp->hash_next;
        }
        tmp->hash_next = nullptr;
        _read_inf(tmp, pos, f);
        return tmp;
    }
    /*
        * it must be in flush queue
        * flush-back to the storage
        * remove it from the flush queue
    */
    //done
    void flush_back(buf_block_t *it, FILE *f){
        if (it->flush.next != nullptr) (it->flush.next)->flush.prev = it->flush.prev;
        else flush.end = it->flush.prev;
        if (it->flush.prev != nullptr) (it->flush.prev)->flush.next = it->flush.next;
        else flush.start = it->flush.next;
        if (flush.start == nullptr) flush.end = nullptr;
        else if (flush.end == nullptr) flush.start = nullptr;
        fseek(f, it->offset, SEEK_SET);
        fwrite(it->frame, 1, BUFFER_SIZE, f);
            --(it->state);
            --flush.count;
    }
    //mode = 0 for free_list, 1 for lru list and 2 for flush list
    inline void flush_all(){
        for (auto temp = flush.start;temp != nullptr;temp = (temp->flush).next){
            fseek(f, temp->offset, SEEK_SET);
            fwrite(temp->frame, 1, BUFFER_SIZE, f);
            if ((temp->flush).prev != nullptr){
                (((temp->flush).prev)->flush).next = nullptr;
                (temp->flush).prev = nullptr;
            }
        }
        flush.start = flush.end = nullptr;
        flush.count = 0;
    }
    inline void clean_connection_list(){
        int n = 0;
        for (auto temp = free.start;temp != nullptr;temp = (temp->free).next){
            if ((temp->free).prev != nullptr){
                (((temp->free).prev)->free).next = nullptr;
                delete (temp->free).prev;
                (temp->free).prev = nullptr;
            }
            n++;
        }
        if (free.end != nullptr) delete free.end;
        n = 0;
        for (auto temp = LRU.start;temp != nullptr;temp = (temp->LRU).next){
            if ((temp->LRU).prev != nullptr){
                (((temp->LRU).prev)->LRU).next = nullptr;
                delete (temp->LRU).prev;
                (temp->LRU).prev = nullptr;
            }
            n++;
        }
        if (LRU.end != nullptr) delete LRU.end;
        n = 0;
    }
public:
    buf_pool_t(FILE *fil):  f(fil),
                            free(), LRU(), flush(),HIR_head(nullptr),
                            clock(0),
                            hash_table(){
        mem_head = new byte[BUFFER_SIZE * (TOTAL_NUM + 1)];
        buf_block_t *temp;
        free.count = TOTAL_NUM;
        free.start = free.end = new buf_block_t(mem_head);
        for (size_t i = 1;i < TOTAL_NUM;i++){
            (free.end)->free.next = temp = new buf_block_t(mem_head + BUFFER_SIZE * i);
            (temp->free).prev = free.end;
            free.end = temp;
        }
    }

    ~buf_pool_t(){
        flush_all();
        clean_connection_list();
    }

    buf_block_t *load_it(const size_t pos){
        //in a further mode, if it is used(a x-lathe added), spin and wait.
        buf_block_t *it = _find(pos);
        if (it != nullptr) {
            _to_HIR(it);
            return it;
        }
        if (free.count) return free_use(pos, f);
        if (LRU.count > flush.count) return LRU_use(pos, f);
        flush_all();
        return LRU_use(pos, f);
    }
    bool release_it(buf_block_t *it){
        if (it == nullptr) return false;
        flush_back(it, f);
        return true;
    }
    void dirty(buf_block_t *it){
        if (flush.start != it){
            it->flush.next = flush.start;
            if (flush.start != nullptr) flush.start->flush.prev = it;
            else flush.end = it;
            flush.start = it;
        }
        if (it->state % 2) {
            ++it->state;
            ++flush.count;
        }
    }
    void print_lists(){
    #ifdef DEBUG_MODE
        printf("debug print free list:\n");
        for (auto temp = free.start;temp != nullptr;temp = (temp->free).next){
            std::cout<< temp->hash_value << ' ' << temp->free.prev << ' ' << temp->free.next << "; ";
        }
        printf("now LRU list:\n");
        for (auto temp = LRU.start;temp != nullptr;temp = (temp->LRU).next){
            std::cout<< temp->hash_value << ' ' << temp->LRU.prev << ' ' << temp->LRU.next << "; ";
        }
        printf("\n");
    #endif
    }
};

#endif