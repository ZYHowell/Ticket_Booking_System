#ifndef SJTU_HASH_TABLE
#define SJTU_HASH_TABLE
template<class V>
class hash_table_t{
    V list[512];
public:
    hash_table_t(){
        for (long i = 0;i < 512;i++) list[i] = 0;
    }
    ~hash_table_t(){}
    int get_value(const long &code){
        return code / 4096;
    }
    V find(const long &code){
        return list[code / 4096];
    }
    //even if U already exists, force to insert it.
    void insert(const long code, const V &v){
        list[code / 4096] = v;
    }
};
#endif