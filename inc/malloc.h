#ifndef MALLOC_H_
#define MALLOC_H_

#include "stdlib.h"
#include "errno.h"
#include "sys/stat.h"
#include "stdint.h"

//#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
 
void* operator new(size_t size);
void operator delete( void *p);
void* operator new[](size_t size);
void operator delete[]( void *p);
void operator delete(void*, unsigned int);

extern "C" void __cxa_pure_virtual() { while (1); }


namespace std
{	
	void __throw_length_error(const char *);	
	void __throw_bad_alloc();	
}

extern "C" 
{
	extern uint8_t _heap_start; // Defined by the linker.
    extern uint8_t _heap_end;   // Defined by the linker.
    
    uint8_t* current_ends=0;
    extern caddr_t _sbrk(int32_t );

	void* init_malloc(uint32_t bytes);
	void gluing_free_blocks(void* start, void* end);

	void* malloc(volatile size_t bytes);
	void free(void* firstbyte);
}

#endif //MALLOC_H