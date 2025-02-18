#pragma once
/*
*移植nginx内存池代码，用OOP来实现
*/
/*
* 回调函数的类型
*/
using u_char = unsigned char;
using ngx_uint_t = unsigned int;

//清理函数(回调函数)类型  ---->void (void*)
typedef void (*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;//函数指针->清理函数
    void* data;//回调函数的参数
    ngx_pool_cleanup_s* next;//链表
};

/*
* 大块内存的头部信息
*/
struct ngx_pool_large_s {//16字节
    ngx_pool_large_s* next;//链表
    void* alloc;//分配出去的大块内存的地址
};

/*
* 小块内存的头部数据信息
*/
struct ngx_pool_s;//类型前置声明
struct ngx_pool_data_t {
    u_char* last;//小块内存池可用内存的起始地址
    u_char* end;//小块内存池可用内存的末尾地址
    ngx_pool_s* next;//此处调用下面的类型 要进行前置声明
    ngx_uint_t            failed;//记录当前小块内存池分配内存失败的次数
} ;


/*
* nginx内存池的头部信息和管理成员信息
*/
struct ngx_pool_s {
    ngx_pool_data_t       d;//小块内存的使用情况
    size_t                max;//小块内存和大块内存的分界线
    ngx_pool_s* current;//指向第一个可以提供小块内存分配的小块内存池
    ngx_pool_large_s* large;//指向大块内存的入口地址
    ngx_pool_cleanup_s* cleanup;//指向预置的清理操作的入口->清理大块内存的外部资源 链表
};
//buf缓冲区清零
#define ngx_memzero(buf, n)       (void) memset(buf, 0, n)

//小块内存分配考虑字节对齐的单位
#define NGX_ALIGNMENT   sizeof(unsigned long)    /* platform word */

//把数值D调整到临近的A的倍数
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))

//把指针p调整到a的临近的倍数
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

const int ngx_pagesize = 4096;//一个页面的大小

//小块内存池可分配的最大空间
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;
//默认内存池开辟大小
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//16K
//内存池大小按16字节对齐
const int NGX_POOL_ALIGNMENT = 16;
//ngx小块内存池最小的size对其NGX_POOL_ALIGNMENT字节
//如果开辟的大小比ngx_pool_s还小是没有意义的
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);

class ngx_mem_pool
{
public:
    //创建Size大小的内存池不超过一个页面的大小
    void* ngx_create_pool(size_t size);

    //考虑内存字节对齐，从内存池申请Size大小的内存
    void* ngx_palloc(size_t size);

    //和上面一样但不考虑内存对齐
    void* ngx_pnalloc(size_t size);

    //调用的是ngx_palloc实现内存分配，但会初始化0
    void* ngx_pcalloc(size_t size);

    //释放大块内存  由于nginx设计结构的原因 小块内存不能被释放
    void ngx_pfree(void* p);

    //内存重置函数
    void ngx_reset_pool();

    //内存池销毁函数
    void ngx_destroy_pool();

    //添加回调清理操作函数
    ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);

private:

	ngx_pool_s* pool;//指向nginx内存池的入口指针
    //小块内存分配
    void* ngx_palloc_small(size_t size, ngx_uint_t align);
    //大块内存分配
    void* ngx_palloc_large(size_t size);
    //分配新的小块内存池
    void* ngx_palloc_block(size_t size);
};