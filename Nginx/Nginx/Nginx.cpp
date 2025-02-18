// Nginx.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ngx_mem_pool.h"
#include<stdio.h>
#include<string.h>
typedef struct Data stData;
struct Data
{
    char* ptr;
    FILE* pfile;
};

void func1(void* p1)//void (*)(void*)
{
    char* p = (char*)p1;
    printf("free ptr mem!");
    free(p);
}
void func2(void* p1)
{
    FILE* pf = (FILE*)p1;
    printf("close file!");
    fclose(pf);
}
int main()
{
    // 512 - sizeof(ngx_pool_t) - 4095   =>   max
     //ngx_create_pool可以直接是现在memorypool的构造函数中
    ngx_mem_pool mempool;//ngx_mem_pool中的pool成员变量还没有初始化
    ngx_pool_s* pool = (ngx_pool_s*)mempool.ngx_create_pool(512);//设置内存池的大小
    if (pool ==nullptr)
    {
        printf("ngx_create_pool fail...");
        return -1;
    }

    void* p1 = mempool.ngx_palloc(128); // 从小块内存池分配的
    if (p1 == nullptr)
    {
        printf("ngx_palloc 128 bytes fail...");
        return -1;
    }

    stData* p2 = (stData*)mempool.ngx_palloc(512); // 从大块内存池分配的
    if (p2 == nullptr)
    {
        printf("ngx_palloc 512 bytes fail...");
        return -1;
    }
    p2->ptr = (char*)malloc(12);
    strcpy(p2->ptr, "hello world");
    p2->pfile = fopen("data.txt", "w");

    ngx_pool_cleanup_s* c1 = mempool.ngx_pool_cleanup_add(sizeof(char*));//此时已经链接到pool->cleanup里面了
    c1->handler = func1;
    c1->data = p2->ptr;

    ngx_pool_cleanup_s* c2 = mempool.ngx_pool_cleanup_add(sizeof(FILE*));
    c2->handler = func2;
    c2->data = p2->pfile;
    //ngx_destroy_pool可以直接是现在memorypool的析构函数中
    mempool.ngx_destroy_pool(); // 1.调用所有的预置的清理函数 2.释放大块内存 3.释放小块内存池所有内存

    return 0;
}



