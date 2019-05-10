//in this version, free list has no use, since the only element to free is mutex.
//some re-consider needed
//in the next version, a queue is needed in order to change LRU to LIRS
#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP

//node of a list
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
    ut_list_head(TYPE* p = nullptr, TYPE* n = nullptr, size_t c):
        prev(p), next(n), count(c){}
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
                                state(0), IRR(0), recency()
                                hash_value(0), hash_next(nullptr),
                                free(), LRU(), flush(){}
};
template<size_t BUFFER_SIZE = 4096, size_t TOTAL_NUM = 256, size_t COLD_PERCENTAGE = 25>
class buf_pool_t{
    using byte = char;
    byte                                *mem_head;
    ut_list_head<buf_block_t>           free, LRU, flush;
    size_t                              clock;
    hash_table_t<buf_block_t*>          hash_table;
    buf_block_t                         *HIR_head;
    
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
        * can only be used while p is a clean buf_block. 
        * get info from the storage, and move it to LRU-old-head.
        * DO NOT CHANGE LRU.COUNT BUT P.STATE = 1.
    */
    void _to_HIR(buf_block_t *p){
        p.state = 1;
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
    void _to_LIR(buf_block_t &p){
        _pick_out_LRU(p);
        ++LRU.count;
        if ((p.state == 1) || (p.state == 3)) ++p.state;     
        p.LRU.prev = nullptr, p.LRU.next = LRU.start;
        LRU.start = &p;
        p.LRU_pos = ++clock;
        //to check if the oldest in LIR set should be removed to the cold list(HIR)?
        //or instead of cold list, flush it?
    }
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
    void _to_LRU_LIR_end(buf_block_t &p){
        _pick_out_LRU(p);
        ++LRU.count;
        if (p.state == 1 || p.state == 3) ++p.state;
        if (HIR_head == nullptr) {
            p.LRU.prev = LRU.end;
            LRU.end = &p;
            if (LRU.start == nullptr) LRU.start = &p;
        }
        else {
            p.LRU.prev = HIR_head->LRU.prev;
            if (LRU.start == HIR_head) LRU.start = &p;
        }
        p.LRU.next = HIR_head;
    }
    //the following are those involving i/o
    /*
        * read info and make the page the head of LRU_old.
        * DO NOT CHANGE LRU.COUNT
    */
    void _read_inf(buf_block_t *p, const size_t &pos, FILE *f){
        fseek(f, pos, SEEK_SET);
        fread(p->frame, 1, BUFFER_SIZE, f);
        p->state = 1, p->offset = pos;
        p->hash_value = hash_table.get_value(pos);
        p->hash_next = nullptr;
        buf_block_t *temp = hash_table.find(pos);
        if (temp == nullptr) p->hash_next = temp;
        hash_table.insert(pos, p);
        _to_LIS(p);
    }
    /*
        * fulfill a free page and move it to the LRU-list
    */
    //done
    buf_block_t *free_use(const size_t &pos, FILE *f){
        buf_block_t *tmp = free.start;
        free.start = (tmp->free).next;
        if (free.start == nullptr) free.end = nullptr;
        --free.count;
        _read_inf(tmp, pos, f);
        ++LRU.count;
        return tmp;
    }
    /*
        * it must be in flush queue
        * flush-back to the storage
        * remove it from the flush queue
    */
    //done
    void flush_back(buf_block_t &it, FILE *f){
        --it.state;
        if (it.flush.next != nullptr) (it.flush.next)->flush.prev = it.flush.prev;
        else flush.end = it.flush.prev;
        if (it.flush.prev != nullptr) (it.flush.prev)->flush.next = it.flush.next;
        else flush.start = it.flush.next;
        if (flush.start == nullptr) flush.end = nullptr;
        else if (flush.end == nullptr) flush.start = nullptr;
        fseek(f, it.offset, SEEK_SET);
        fwrite(it.frame, 1, BUFFER_SIZE, f);
    }
public:
    buf_pool_t():free(), LRU(), flush(),HIR_head(nullptr)
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

    ~buf_pool_t(){}

    buf_block_t *load_it(const size_t &pos, FILE *f){
        //in a further mode, if it is used(a x-lathe added), spin and wait.
        buf_block_t *temp = _find(pos);
        if (temp != nullptr) {
            
            return temp;
        }
        if (free.count) return free_use(pos, f);
        if (LRU.count > flush.count) return LRU_use(pos, f);
    }

    
};

#endif