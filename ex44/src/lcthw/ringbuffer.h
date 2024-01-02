#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <lcthw/bstrlib.h>

//定义一个ringbuffer结构体，他有两个指针，一个读一个写，二者分开，假若一个数据长度过长超过剩余内存长度，指针就会先读正好到最后长度的数据，随后将指针指向开头，再读剩下的。
typedef struct{
    char* buffer;
    int length;
    int *start;
    int *end;
    int *read;
    int *write;
} RingBuffer;

RingBuffer* RingBuffer_create(int length);

void RingBuffer_destroy(RingBuffer* buffer);

int RingBuffer_read(RingBuffer* buffer, char* target, int amount);

int RingBuffer_write(RingBuffer* buffer, char* data, int length);

int RingBuffer_empty(RingBuffer* buffer);

int RingBuffer_full(RingBuffer* buffer);

int RingBuffer_available_data(RingBuffer* buffer);

int RingBuffer_available_space(RingBuffer* buffer);

bstring RingBuffer_gets(RingBuffer* buffer, int amount);

// intentionally wrong definition ?
// #define RingBuffer_available_data(B)(\
// ((B)->end + 1) % (B)->length - (B)->start - 1)

#define RingBuffer_available_data(B)(\
((B)->end) % (B)->length - (B)->start)

#define RingBuffer_available_space(B)(\
(B)->length - (B)->end - 1)

#define RingBuffer_full(B) (RingBuffer_available_data((B))\
- (B)->length == 0)

#define RingBuffer_empty(B)(\
RingBuffer_available_data((B)) == 0)

#define RingBuffer_puts(B,D) RingBuffer_write(\
(B), bdata((D)), blength((D)))

#define RingBuffer_get_all(B) RingBuffer_gets(\
(B), RingBuffer_available_data((B)))

#define RingBuffer_starts_at(B)(\
(B)->buffer + (B)->start)

#define RingBuffer_ends_at(B)(\
(B)->buffer + (B)->end)

#define RingBuffer_commit_read(B,A)(\
(B)->start = ((B)->start + (A)) % (B)->length)

#define RingBuffer_commit_write(B,A)(\
(B)->end = ((B)->end + (A)) % (B)->length)

#endif // _LCTHW_RINGBUFFER_H_
