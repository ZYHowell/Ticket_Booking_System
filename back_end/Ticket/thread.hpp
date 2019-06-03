#ifndef SJTU_THREAD_HPP
#define SJTU_THREAD_HPP

#include <thread>
#include <shared_mutex>
using rw_latch_t    = std::shared_mutex;
using shared_lock_t = std::shared_lock<rw_latch_t>;
using unique_lock_t = std::unique_lock<rw_latch_t>;

#endif