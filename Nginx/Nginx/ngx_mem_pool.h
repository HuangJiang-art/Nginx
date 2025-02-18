#pragma once
/*
*��ֲnginx�ڴ�ش��룬��OOP��ʵ��
*/
/*
* �ص�����������
*/
using u_char = unsigned char;
using ngx_uint_t = unsigned int;

//������(�ص�����)����  ---->void (void*)
typedef void (*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;//����ָ��->������
    void* data;//�ص������Ĳ���
    ngx_pool_cleanup_s* next;//����
};

/*
* ����ڴ��ͷ����Ϣ
*/
struct ngx_pool_large_s {//16�ֽ�
    ngx_pool_large_s* next;//����
    void* alloc;//�����ȥ�Ĵ���ڴ�ĵ�ַ
};

/*
* С���ڴ��ͷ��������Ϣ
*/
struct ngx_pool_s;//����ǰ������
struct ngx_pool_data_t {
    u_char* last;//С���ڴ�ؿ����ڴ����ʼ��ַ
    u_char* end;//С���ڴ�ؿ����ڴ��ĩβ��ַ
    ngx_pool_s* next;//�˴�������������� Ҫ����ǰ������
    ngx_uint_t            failed;//��¼��ǰС���ڴ�ط����ڴ�ʧ�ܵĴ���
} ;


/*
* nginx�ڴ�ص�ͷ����Ϣ�͹����Ա��Ϣ
*/
struct ngx_pool_s {
    ngx_pool_data_t       d;//С���ڴ��ʹ�����
    size_t                max;//С���ڴ�ʹ���ڴ�ķֽ���
    ngx_pool_s* current;//ָ���һ�������ṩС���ڴ�����С���ڴ��
    ngx_pool_large_s* large;//ָ�����ڴ����ڵ�ַ
    ngx_pool_cleanup_s* cleanup;//ָ��Ԥ�õ�������������->�������ڴ���ⲿ��Դ ����
};
//buf����������
#define ngx_memzero(buf, n)       (void) memset(buf, 0, n)

//С���ڴ���俼���ֽڶ���ĵ�λ
#define NGX_ALIGNMENT   sizeof(unsigned long)    /* platform word */

//����ֵD�������ٽ���A�ı���
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))

//��ָ��p������a���ٽ��ı���
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

const int ngx_pagesize = 4096;//һ��ҳ��Ĵ�С

//С���ڴ�ؿɷ�������ռ�
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;
//Ĭ���ڴ�ؿ��ٴ�С
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//16K
//�ڴ�ش�С��16�ֽڶ���
const int NGX_POOL_ALIGNMENT = 16;
//ngxС���ڴ����С��size����NGX_POOL_ALIGNMENT�ֽ�
//������ٵĴ�С��ngx_pool_s��С��û�������
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);

class ngx_mem_pool
{
public:
    //����Size��С���ڴ�ز�����һ��ҳ��Ĵ�С
    void* ngx_create_pool(size_t size);

    //�����ڴ��ֽڶ��룬���ڴ������Size��С���ڴ�
    void* ngx_palloc(size_t size);

    //������һ�����������ڴ����
    void* ngx_pnalloc(size_t size);

    //���õ���ngx_pallocʵ���ڴ���䣬�����ʼ��0
    void* ngx_pcalloc(size_t size);

    //�ͷŴ���ڴ�  ����nginx��ƽṹ��ԭ�� С���ڴ治�ܱ��ͷ�
    void ngx_pfree(void* p);

    //�ڴ����ú���
    void ngx_reset_pool();

    //�ڴ�����ٺ���
    void ngx_destroy_pool();

    //��ӻص������������
    ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);

private:

	ngx_pool_s* pool;//ָ��nginx�ڴ�ص����ָ��
    //С���ڴ����
    void* ngx_palloc_small(size_t size, ngx_uint_t align);
    //����ڴ����
    void* ngx_palloc_large(size_t size);
    //�����µ�С���ڴ��
    void* ngx_palloc_block(size_t size);
};