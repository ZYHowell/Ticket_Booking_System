#ifndef SJTU_HASH_TABLE
#define SJTU_HASH_TABLE
template<class V>
class hash_table_t{
public:
    int get_value(const long &code){}
    V find(const long &code){}
    //even if U already exists, force to insert it.
    void insert(const long &code, const long &v){}
};
#endif