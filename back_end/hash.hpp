#ifndef SJTU_HASH_TABLE
#define SJTU_HASH_TABLE
template<class U, class V>
class hash_table_t{
public:
    int get_value(const U &code){}
    V find(const U &code){}
    //even if U already exists, force to insert it.
    void insert(const U &code, const V &v){}
};
#endif