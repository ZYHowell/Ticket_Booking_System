#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP
template<size_t buffer_size>
class bufferpool{
    using byte = char;
    struct buf_chunk_t{

    };
    struct buf_bolck_t{

    };
    struct buf_page_t{

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