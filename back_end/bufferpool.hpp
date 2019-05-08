#ifndef SJTU_BUFFER_POOL_HPP
#define SJTU_BUFFER_POOL_HPP
template <class value_t> 
struct UT_LIST_NODE_T{
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

template<size_t BUFFER_SIZE = 4096, size_t TOTAL_PAGE_NUMBER = 256>
struct buf_pool_t{
    using byte = char;

    template<size_t BUFFER_SIZE = 4096>
    struct buf_block_t{
        byte                            *frame;
        //where it is in memory(head)
        size_t                          space, offset;
        /*
            * space refers to the id of its space(may not be used), 
            * and offset refers to its loc at storage.
        */
        size_t                          lock_hash_val;
        buf_block_t                     *hash;
        //means its position at hash lists to deal with collisions
        UT_LIST_NODE_T<buf_block_t>     flush, free, LRU;
        //means its position(pre and next) at these three lists
        size_t                          LRU_position;
        size_t                          newest_modification, oldest_modification;
        bool                            is_old, accessed;
    };

    byte                                *frame_mem, *frame_zero, *high_end;
    /*
        * the beginning of the bufferpool,
        * the 16kb alined beginning of the bufferpool,
        * the end of the bufferpool
    */
    buf_block_t                         *blocks;
    //all blocks
    size_t                              curr_size;
    //current size, which must be smaller than TOTAL_PAGE_NUMBER
    hash_table_t                        *page_hash;
    //a hash table
    size_t                              n_pend_reads;
    //tasks remained to be done, be useful for multi-thread version
    size_t                              clock, freed_page_clock;
    //when a page is added into the LRU list, ++clock.
    UT_LIST_BASE_NODE_T<buf_block_t>    flush_list, free, LRU;
    buf_block_t                         *LRU_old;
    size_t                              LRU_old_len;
    //bool                                init_flush[BUF_FLUSH_LIST + 1];

};
#endif