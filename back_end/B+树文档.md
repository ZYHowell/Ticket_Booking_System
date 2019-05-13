## B+树文档

B+树的具体原理在这里不再提及

### 硬盘空间管理(class file_alloc)

使用自定义类ALLOC模仿stl::alloc管理内存的方式来管理外存，即预设一个大小，小于该大小的空间申请会经过一个储存剩余空间池的链表进行分配，大于该大小时直接从文件末尾进行申请。当外存池分配满后，会从文件当前末尾开始申请一定大小的外存作为新的外存池。在释放外存时，会收回所有大小的外存，并将一定长度范围内的连续外存适当合并。

#### 接口说明

- 构造函数

  `ALLOC::ALLOC`

  其实并不需要什么参数。

- 从文件中加载外存池的相关信息以初始化。如果文件不存在，创建文件并申请外存

  `void ALLOC::init(const char * filename = "")`

  从文件名为filename的文件中加载硬盘空间使用信息

- 把信息写入文件

  `void ALLOC::save(const char *filename = "")`

  把硬盘空间使用情况写入名为filename的文件当中

- 判断是否为空

  `inline bool ALLOC::empty()`

- 清空

  `void ALLOC::clear();`

- 申请新空间

  `pointer ALLOC::alloc(size_t s);`

- 释放空间

  `void ALLOC::free(pointer pos, size_t s);`


### B+树结构(class bptree)

这是一个类模板，其实例化时依次需要提供的参数包括：key_type和value_type两个类、与硬件有关的part_size（默认为4096）和用于比较的类Compare（默认为std::less）。

#### 接口说明

- 构造函数

  `bplustree()`

  done

- 初始化

  `init(const char *datafile_name,const char *alloc_name)`

  分别是数据库文件和作为索引的B+树文件的文件名及其外存管理文件的文件名。

- 询问是否有某个key

  `bool count(const key_type &key)`

  如果有返回1，否则返回0

- 根据key查找元素

  `pair<bool,value_type> find(const key_t &key)`

  返回key所对应的value，若key不存在，返回false和value_type的默认构造

- 判断是否为空

  `inline bool empty()`

- 插入元素

  `bool insert(const key_t &key, const value_t &v)`

  若key已存在，则什么事情都不会做，返回false，插入成功则返回true

- 修改

  `bool set(const key_t &key, const value_t &v)`

  如果不存在key，不进行任何操作并返回false，否则返回true

- 删除

  `void remove(const key_t &key)`

  如果不存在key，不进行任何操作并返回false，否则返回true

- 区间查找

  `vector<key_t,value_type> listof(const key_t & key,  bool (*comp)(key_type k1, key_type k2))`

  需要确保comp所确定的区间范围在磁盘上连续

  comp的意义为“小于”。将B+树中key既不大于key又不小于key的记录填入一个vector中返回

