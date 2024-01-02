#include <stdlib.h>
#include "ringbuffer.h"
#include "dbg.h"
/*
char *phead=NULL;
char *ptail =NULL;
char* pwrite= NULL;
char * pread =NULL;*/
RingBuffer *RingBuffer_create(int length)
{
    RingBuffer *buffer = NULL;
    check(length > 0, "Length must be positive");

    buffer = calloc(1, sizeof(RingBuffer));
    check_mem(buffer);

    buffer->buffer = calloc(length, sizeof(char*));
    check_mem(buffer->buffer);

	buffer->start=0;
	buffer->end = buffer->start+length+1;
	buffer->write=buffer->start;
	buffer->read=buffer->start;
	

    return buffer;

error:
    if (buffer) RingBuffer_destroy(buffer);

    return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer)
{
    if (buffer)
    {
        if (buffer->buffer) free(buffer->buffer);
        free(buffer);
    }
}

int RingBuffer_read(RingBuffer *buffer, char *target, int amount)
{
    check(buffer != NULL, "RingBuffer cannot be NULL");
    check(target != NULL, "Target cannot be NULL");
    check(amount > 0, "Amount must be greater than 0");
	check(amount < buffer->end - buffer->start,"too much length to read");
    //check(amount <= RingBuffer_available_data(buffer), "Cannot read more than available data");

	if(amount>RingBuffer_available_data)
	{
		int pre_len = (buffer->end-1) - buffer->read;
		int next_len = amount - (unsigned int)RingBuffer_available_data;
		memcpy(target,buffer->buffer+buffer->read,pre_len);
		memcpy(target+pre_len,buffer->buffer+buffer->start,next_len);
		buffer->read = buffer->start+next_len;
	}
else{

	memcpy(target,buffer->buffer+buffer->read,amount);
	buffer->read+=amount;
}

    return 0;

error:
    return -1;
}

int RingBuffer_peek(RingBuffer *buffer, char *target, int amount)
{
    check(buffer != NULL, "RingBuffer cannot be NULL");
    check(target != NULL, "Target cannot be NULL");
    check(amount > 0, "Amount must be greater than 0");
    check(amount <= RingBuffer_available_data(buffer), "Cannot peek more than available data");

    for (uint32_t index = 0; index < amount; index++)
    {
        uint32_t bufferIndex = (buffer->start + index) ;
        target[index] = buffer->buffer[bufferIndex];
    }

    target[amount] = 0;

    return 0;

error:
    return -1;
}

int RingBuffer_write(RingBuffer *buffer, char *data,int length)
{
    check(buffer != NULL, "RingBuffer cannot be NULL");
    check(data != NULL, "Data cannot be NULL");
    check(length > 0, "Length must be greater than 0");
	check(length <( buffer->end-1) - buffer->start,"too much length to write");

    if(length>RingBuffer_available_space)
	{
		int pre_len = (buffer->end-1) - buffer->write;
		int next_len = length -(unsigned int) RingBuffer_available_space;
		memcpy(buffer->buffer+buffer->write,data,pre_len);
		memcpy(buffer->buffer+buffer->start,data+pre_len,next_len);
		buffer->write = buffer->start+next_len;
	}
	else{
		memcpy(buffer->buffer+buffer->write,data,length);
		buffer->write += length;
	}

    return 0;

error:
    return -1;
}

bool RingBuffer_empty(RingBuffer *buffer)
{
    check(buffer != NULL, "Buffer cannot be NULL");

    return buffer->write == buffer->start&&buffer->read ==buffer->start;

error:
    return false;
}

bool RingBuffer_full(RingBuffer *buffer)
{
    check(buffer != NULL, "Buffer cannot be NULL");

    return buffer->write == buffer->end&&buffer->read ==buffer->end;

error:
    return false;
}

int RingBuffer_available_data(RingBuffer *buffer)
{
    check(buffer != NULL, "Buffer cannot be NULL");

    return buffer->read-buffer->start;

error:
    return 0;
}

int RingBuffer_available_space(RingBuffer *buffer)
{
    check(buffer != NULL, "Buffer cannot be NULL");

    return (buffer->end-1) - buffer->write ;

error:
    return 0;
}
