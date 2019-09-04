/**
 * @author wangdaopo
 * @email 3168270295@qq.com
 */
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include<string.h>
#define MAX_BLOCK_COUNT 200
#define MAX_DATA_BLOCK_SIZE 100
#define  BUG  1
typedef struct {
    int  data_len;
    char data[MAX_DATA_BLOCK_SIZE];
} user_data_t;


int init_mmap(const char * name_file,user_data_t **p_map)
{
    int	pagesize = sysconf(_SC_PAGESIZE);
    int max_data_size = sizeof(user_data_t)*MAX_BLOCK_COUNT;
    int page_count = max_data_size / pagesize ;
    page_count =  max_data_size % pagesize > 0 ? (page_count+1) : page_count;
    printf("max_data_size:%d pagesize is %d  page_count:%d \n",max_data_size,pagesize,page_count);
    int fd =open(name_file,O_CREAT|O_RDWR|O_TRUNC,00777);
    lseek(fd,pagesize*page_count-1,SEEK_SET);
    write(fd,"",pagesize*page_count);
    *p_map = (user_data_t*) mmap( NULL,sizeof(user_data_t)*MAX_BLOCK_COUNT,PROT_READ|PROT_WRITE,
                                  MAP_SHARED,fd,0 );//此处offset = 0编译成版本1；offset = pdata_lensize编译成版本2  //注意：文件被映射部分而不是整个文件决定了进程能够访问的空间大小，另外，如果指定文件的偏移部分，一定要注意为页面大小的整数倍。下面是对进程映射地址空间的访问范例：
    close( fd );
    return fd;
}

int  destory_mmap( user_data_t *p_map)
{
    munmap( p_map, sizeof(user_data_t)*MAX_BLOCK_COUNT );
    return 0;
}

int   write_mmap(user_data_t *p_map, const char* in_data_buff,const int in_data_len)
{
    static int write_offset = 0;
    if(p_map == NULL || in_data_buff == NULL)
    {
        printf( "p_map:%p  in_data_buff:%p retun -1\n",p_map,in_data_buff);
        return -1;
    }

    int count = in_data_len/MAX_DATA_BLOCK_SIZE;
    int remain_data_len = in_data_len %  MAX_DATA_BLOCK_SIZE;

    int  data_max_clock_count = count + ( remain_data_len > 0 ? 1:0 );
#if BUG
    printf( "write_offset:%d   in_data_len %d data_max_clock_count:%d\n",write_offset,in_data_len,data_max_clock_count );
#endif
    if( data_max_clock_count   >  MAX_BLOCK_COUNT)
    {
        printf( "data_max_clock_count:%d >  MAX_BLOCK_COUNT:%d retun -1\n",data_max_clock_count,MAX_BLOCK_COUNT);
        return -1;
    }
    for(int i=0; i<count ; i++)
    {
#if BUG
        printf( "write_offset:%d  data_len %d;\n",write_offset, MAX_DATA_BLOCK_SIZE );
#endif
        memcpy( ( *(p_map+write_offset) ).data, in_data_buff + i * MAX_DATA_BLOCK_SIZE, MAX_DATA_BLOCK_SIZE );
        ( *(p_map+write_offset) ).data_len = MAX_DATA_BLOCK_SIZE;
        write_offset++;
        write_offset = write_offset % MAX_BLOCK_COUNT;
    }
    if(remain_data_len)
    {
#if BUG
        printf( "write_offset:%d  data_len %d;\n",write_offset, remain_data_len);
#endif
        memcpy( ( *(p_map+write_offset) ).data, in_data_buff + count * MAX_DATA_BLOCK_SIZE, remain_data_len );
        ( *(p_map+write_offset) ).data_len = remain_data_len;
        write_offset++;
        write_offset = write_offset % MAX_BLOCK_COUNT;
    }

    return 0;
}

int   read_mmap(const user_data_t *p_map, const int  read_data_len , char*read_data_buff)//char*out_data_buff, int* out_data_len
{
    static int read_offset = 0;
    if(p_map == NULL || read_data_buff == NULL)
    {
        printf( "p_map:%p  read_data_buff:%p retun -1\n",p_map,read_data_buff);
        return -1;
    }
    int count = read_data_len/MAX_DATA_BLOCK_SIZE;
    int remain_data_len = read_data_len %  MAX_DATA_BLOCK_SIZE;

    int  data_max_clock_count = count + ( remain_data_len > 0 ? 1:0 ) ;
#if BUG
    printf( "read_offset:%d   read_data_len %d  data_max_clock_count:%d\n",read_offset,read_data_len, data_max_clock_count );
#endif
    if( data_max_clock_count >  MAX_BLOCK_COUNT)
    {
        printf( "data_max_clock_count:%d >  MAX_BLOCK_COUNT:%d retun -1\n",data_max_clock_count,MAX_BLOCK_COUNT);
        return -1;
    }
    for(int i=0; i<count ; i++)
    {
#if BUG
        printf( "read_offset:%d data: %s data_len %d;\n",read_offset,(*(p_map+read_offset)).data, (*(p_map+read_offset)).data_len );
#endif
        int data_len = (*(p_map+read_offset)).data_len;
        memcpy(read_data_buff + i*MAX_DATA_BLOCK_SIZE ,(*(p_map+read_offset)).data, data_len);
        read_offset++;
        read_offset = read_offset % MAX_BLOCK_COUNT;
    }
    if(remain_data_len)
    {
#if BUG
        printf( "read_offset:%d data: %s data_len %d;\n",read_offset,(*(p_map+read_offset)).data, (*(p_map+read_offset)).data_len );
#endif
        memcpy(read_data_buff + count*MAX_DATA_BLOCK_SIZE,(*(p_map+read_offset)).data, remain_data_len);
        read_offset++;
        read_offset = read_offset % MAX_BLOCK_COUNT;
    }
    return 0;
}

int main(int argc, char** argv)  // map a normal file as shared mem:
{
    user_data_t *p_map;
    init_mmap("./test",&p_map);

    char in_data_buff[MAX_DATA_BLOCK_SIZE*68+42];
    int in_data_len = MAX_DATA_BLOCK_SIZE*68+42 ;
    memset(in_data_buff,'a',in_data_len);
    write_mmap(p_map,in_data_buff,in_data_len);

    int read_data_len = MAX_DATA_BLOCK_SIZE*68+42;
    char read_data_buff[MAX_DATA_BLOCK_SIZE*68+42];
    read_mmap(p_map,read_data_len,read_data_buff);

    destory_mmap(p_map);
    return 0;
}
