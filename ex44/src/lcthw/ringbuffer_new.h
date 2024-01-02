#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include "bstrlib.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct
{
    char *buffer;
    int start;
	int end;
	int read;
	int write;
	
} RingBuffer;

RingBuffer *RingBuffer_create(int length);
void RingBuffer_destroy(RingBuffer *buffer);
int RingBuffer_read(RingBuffer *buffer, char *target, int amount);
int RingBuffer_peek(RingBuffer *buffer, char *target, int amount);
int RingBuffer_write(RingBuffer *buffer, char *data, int length);

bool RingBuffer_empty(RingBuffer *buffer);
bool RingBuffer_full(RingBuffer *buffer);
int RingBuffer_available_data(RingBuffer *buffer);
int RingBuffer_available_space(RingBuffer *buffer);

#endif
