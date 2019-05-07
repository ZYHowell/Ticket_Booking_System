

#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP
template <class value_t> 
class UT_LIST_NODE_T{
public:
    value_t *pre, *next;
    UT_LIST_NODE_T(value_t *p = nullptr, value_t *n = nullptr):pre(p), next(n){}
};
template <class value_t> 
class UT_LIST_BASE_NODE_T{
public:
    size_t count;
    value_t *pre, *next;
    UT_LIST_NODE_T(value_t *p = nullptr, value_t *n = nullptr):pre(p), next(n), count(0){}
};
template<size_t buffer_size = 65536, size_t max_size = 65536 * 65536>
class bufferpool{
    using byte = char;
    struct buf_chunk_t{

    };
    struct buf_block_t{
        byte                            *frame;
        size_t                          space, offset;
        size_t                          lock_hash_val;
        buf_block_t                     *hash;
        UT_LIST_NODE_T<buf_block_t>     flush_list, free, LRU;
        size_t                          LRU_position;
        size_t                          newest_modification, oldest_modification;
        bool                            old, accessed;
    };
    struct buf_pool_t{
        byte                            *frame_mem, *frame_zero, *high_end;
        buf_block_t                     *blocks;
        size_t                          curr_size;
        size_t                          clock, freed_page_clock;
        
    };
    /*
    free
    LRU
    FLUSH
    buf_page_t *LRU_old, *LRU_new, *LRU_tail;
    buf_block_t buf_LRU_get_free_block(){}
    void buf_LRU_add_block(){}
    void buf_LRU_free_block(){}
    buf_block_t buf_LRU_search_and_free_block(){}
    buf_block_t buf_LRU_get_free_only(){}
    void buf_LRU_try_free_flushed_blocks(){}
    void BUF_FLUSH_LRU(){}
    void 
    */
public:
    bufferpool(){}
    ~bufferpool(){}
};
#endif