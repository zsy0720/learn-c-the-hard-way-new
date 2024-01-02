#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
 
#define report_exceptional_condition() abort ()
 
struct ring_buffer
{
  void *address;
 
  unsigned long count_bytes;
  unsigned long write_offset_bytes;
  unsigned long read_offset_bytes;
};
 
//Warning order should be at least 12 for Linux
void
ring_buffer_create (struct ring_buffer *buffer, unsigned long order)
{
  char path[] = "/dev/shm/ring-buffer-XXXXXX";
  
//mkstemp (建立唯一临时文件)
//头文件: #include <stdlib.h>
//声明:   int mkstemp(char *template)
//返回值: 成功则返回0, 失败则返回-1.
//说明:  建立唯一临时文件名, template须以数组形式声明而非指针形式. 
  //      template格式为: template.XXXXXX. 最后6位必须为XXXXXX, 前缀随意
  
  int file_descriptor;
  void *address;
  int status;
 
  file_descriptor = mkstemp (path);
  if (file_descriptor < 0)
    report_exceptional_condition ();
 
  status = unlink (path);
  //int unlink(const char * pathname);
  //unlink()会删除参数pathname 指定的文件. 如果该文件名为最后连接点, 但有其他进程打开了此文件, 则在所有关于此文件的文件描述词皆关闭后才会删除. 如果参数pathname 为一符号连接, 则此连接会被删除
  //成功则返回0, 失败返回-1, 错误原因存于errno
  
  if (status)
    report_exceptional_condition ();
 
  buffer->count_bytes = 1UL << order;
  buffer->write_offset_bytes = 0;
  buffer->read_offset_bytes = 0;
 
  status = ftruncate (file_descriptor, buffer->count_bytes);
  //函数功能：改变文件大小,int ftruncate(int fd, off_t  length)
  //ftruncate()会将参数fd指定的文件大小改为参数length指定的大小。参数fd为已打开的文件描述词，而且必须是以写入模式打开的文件。如果原来的文件件大小比参数length大，则超过的部分会被删去
//返 回  值：0、-1
  
  if (status)
    report_exceptional_condition ();
 
  buffer->address = mmap (NULL, buffer->count_bytes << 1, PROT_NONE,
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
 
  if (buffer->address == MAP_FAILED)
    report_exceptional_condition ();
 
  address =
    mmap (buffer->address, buffer->count_bytes, PROT_READ | PROT_WRITE,
          MAP_FIXED | MAP_SHARED, file_descriptor, 0);
  //void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);
  //该函数主要用途有三个：
//1、将一个普通文件映射到内存中，通常在需要对文件进行频繁读写时使用，这样用内存读写取代I/O读写，以获得较高的性能；
//2、将特殊文件进行匿名内存映射，可以为关联进程提供共享内存空间；
//3、为无关联的进程提供共享内存空间，一般也是将一个普通文件映射到内存中。
  //start：指向欲映射的内存起始地址，通常设为 NULL，代表让系统自动选定地址，映射成功后返回该地址
  //length：代表将文件中多大的部分映射到内存
  //prot：映射区域的保护方式。可以为以下几种方式:PROT_EXEC 映射区域可被执行  PROT_READ 映射区域可被读取  PROT_WRITE 映射区域可被写入  PROT_NONE 映射区域不能存取
//flags：影响映射区域的各种特性。在调用mmap()时必须要指定MAP_SHARED 或MAP_PRIVATE
//MAP_FIXED 如果参数start所指的地址无法成功建立映射时，则放弃映射，不对地址做修正。通常不鼓励用此旗标。
//MAP_SHARED对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
//MAP_PRIVATE 对映射区域的写入操作会产生一个映射文件的复制，即私人的“写入时复制”（copy on write）对此区域作的任何修改都不会写回原来的文件内容。
//MAP_ANONYMOUS建立匿名映射。此时会忽略参数fd，不涉及文件，而且映射区域无法和其他进程共享。
//MAP_DENYWRITE只允许对映射区域的写入操作，其他对文件直接写入的操作将会被拒绝。
//MAP_LOCKED 将映射区域锁定住，这表示该区域不会被置换（swap）。
//fd：要映射到内存中的文件描述符。如果使用匿名内存映射时，即flags中设置了MAP_ANONYMOUS，fd设为-1
  //offset：文件映射的偏移量，通常设置为0，代表从文件最前方开始对应，offset必须是分页大小的整数倍
 
  if (address != buffer->address)
    report_exceptional_condition ();
 
  address = mmap (buffer->address + buffer->count_bytes,
                  buffer->count_bytes, PROT_READ | PROT_WRITE,
                  MAP_FIXED | MAP_SHARED, file_descriptor, 0);
 
  if (address != buffer->address + buffer->count_bytes)
    report_exceptional_condition ();
 
  status = close (file_descriptor);
  if (status)
    report_exceptional_condition ();
}
 
void
ring_buffer_free (struct ring_buffer *buffer)
{
  int status;
 
  status = munmap (buffer->address, buffer->count_bytes << 1);
  //int munmap(void* addr, size_t len ) ;
  //成功执行时，munmap()返回0。失败时，munmap返回-1，error返回标志和mmap一致
  //该调用在进程地址空间中解除一个映射关系，addr是调用mmap()时返回的地址，len是映射区的大小

  
  if (status)
    report_exceptional_condition ();
}
 
void *
ring_buffer_write_address (struct ring_buffer *buffer)
{
  /*** void pointer arithmetic is a constraint violation. ***/
  return buffer->address + buffer->write_offset_bytes;
}
 
void
ring_buffer_write_advance (struct ring_buffer *buffer,
                           unsigned long count_bytes)
{
  buffer->write_offset_bytes += count_bytes;
}
 
void *
ring_buffer_read_address (struct ring_buffer *buffer)
{
  return buffer->address + buffer->read_offset_bytes;
}
 
void
ring_buffer_read_advance (struct ring_buffer *buffer,
                          unsigned long count_bytes)
{
  buffer->read_offset_bytes += count_bytes;
 
  if (buffer->read_offset_bytes >= buffer->count_bytes)
    { /*如果读指针大于等于缓冲区长度，那些读写指针同时折返回[0, buffer_size]范围内  */
      buffer->read_offset_bytes -= buffer->count_bytes;
      buffer->write_offset_bytes -= buffer->count_bytes;
    }
}
 
unsigned long
ring_buffer_count_bytes (struct ring_buffer *buffer)
{
  return buffer->write_offset_bytes - buffer->read_offset_bytes;
}
 
unsigned long
ring_buffer_count_free_bytes (struct ring_buffer *buffer)
{
  return buffer->count_bytes - ring_buffer_count_bytes (buffer);
}
 
void
ring_buffer_clear (struct ring_buffer *buffer)
{
  buffer->write_offset_bytes = 0;
  buffer->read_offset_bytes = 0;
}
