#include "malloc.h"

//#include "tasks.c"
//#include "heap_2.c"
#define USE_FREERTOS

void* operator new(std::size_t size)
{	
	#ifdef USE_FREERTOS
    void* p;
	if(uxTaskGetNumberOfTasks())
	{
		p = pvPortMalloc(size);
	}	
	else
	{
		p = malloc(size);
	}	
	return p;
	#endif
	return malloc(size);	
}
void operator delete( void *p)
{
	#ifdef USE_FREERTOS
	if(uxTaskGetNumberOfTasks())
	{
		vPortFree(p);
	}	
	else
	{
		free(p);
	}return;
	#endif	
 	free(p);
}
void* operator new[](std::size_t size)
{
	#ifdef USE_FREERTOS
    void* p;	
	if(uxTaskGetNumberOfTasks())
	{
		p = pvPortMalloc(size);
	}	
	else
	{
		p = malloc(size);
	}	
	return p;
	#endif
	return malloc(size);	
}
void operator delete[]( void *p)
{
	#ifdef USE_FREERTOS
	if(uxTaskGetNumberOfTasks())
	{
		vPortFree(p);
	}	
	else
	{
		free(p);
	}return;
	#endif	
 	free(p);
}
void operator delete(void* p, unsigned int x)
{
    #ifdef USE_FREERTOS
	if(uxTaskGetNumberOfTasks())
	{
		vPortFree(p);
	}	
	else
	{
		free(p);
	}return;
	#endif	
 	free(p);
}

namespace std
{	
	void __throw_length_error(const char *x)
	{
		while(true){}
	}
	void __throw_bad_alloc()
	{
		while(true){}
	}
}

    extern"C" uint8_t align(uint16_t x)
    {
        switch(x%4) //align on 4 bytes
        {
            case 1: return 3;break;
            case 2: return 2;break;
            case 3: return 1;break;
            default: return 0;break;
        }
    }
    extern caddr_t _sbrk(int32_t incr)
    {        
        extern int errno;
            
        uint8_t* current_block_address;
        if(current_ends == 0)
        {
            current_ends = &_heap_start;
        }
        current_block_address = current_ends;
        // Need to align heap to word boundary, else will get
        // hard faults on Cortex-M0. So we assume that heap starts on
        // word boundary, hence make sure we always add a multiple of
        // 4 to it.
        //incr = (incr+3)&(~3);  //allign value to 4
        uint16_t z=align(incr);
        incr=incr+z;
        if(current_ends + incr >&_heap_end) //if heap size was overfloated
        {
            errno=ENOMEM;
            return (caddr_t)(-1); 
        }
        current_ends += incr; // increasing current heap end pointer;
        return (caddr_t) current_block_address;  //return new heap address
    }
//}

//extern "C" 
//{   
    //#pragma pack (push,1)
    //typedef struct MemBlocks
    //{
    //    uint8_t av;
    //    uint8_t zero;
    //    uint16_t size;        
    //}MemBlock;
    //#pragma pack(pop)
    typedef struct MemBlocks
    {
        uint16_t av;
        uint16_t size;        
    }MemBlock;
    MemBlock* mem;
    //struct MemBlock* mem;
    void* init_malloc(uint32_t bytes)
    {
        return _sbrk(bytes+sizeof(MemBlock)); //when first call its gave mems        
    }

    void gluing_free_blocks(void* start, void* end)
    {
        volatile void* curr = start;
        mem=(MemBlock*)curr; //in all cases in that function curr=mem
        while(curr < end)
        {
            if(mem->av==0)  // first block and nexteses
            {
                void* initial_addr = mem;
                uint16_t sizeses=0; //initial adding size
                curr = mem; 
                mem=(MemBlock*)(curr + mem->size + sizeof(MemBlock)+align(mem->size)); //go to next block
                curr = mem; 
                while(mem->av==0&&curr < end)
                {
                    sizeses+=mem->size+align(mem->size)+sizeof(MemBlock); //sum all sizes available blocks goes straight
                    curr=mem;
                    mem = (MemBlock*)(curr + mem->size + sizeof(MemBlock)); // go to next straight free block
                    curr=mem;                    
                }
                mem=(MemBlock*)initial_addr;                
                mem->size =mem->size + align(mem->size) + sizeses; //increasing mem size that decreasing blocks numbers;
                mem = (MemBlock*)curr; // again mem to curr to continue first while
                //curr = mem + sizeof(MemBlock); // initial curr
            }
            else //mem->av==1  -  go to next block
            {
                curr=mem;
                mem = (MemBlock*)(curr + mem->size+align(mem->size) + sizeof(MemBlock));
                curr = mem; //go to next block                
            }
        }
    }

    void* malloc(volatile size_t bytes)
    {
        //if(bytes==0){return NULL;}
        //extern uint8_t _heap_start; // Defined by the linker.
        //extern uint8_t _heap_end;   // Defined by the linker.        
        void* curr_end = _sbrk(0);
        void* curr=&_heap_start;       
        mem=(MemBlock*)curr;    // initial mem 
        if(curr==curr_end)  //if this condition => _sbrk was not called yet
        {
            curr=init_malloc(bytes);
            mem=(MemBlock*)curr;
            mem->av=1;
            mem->size=bytes;
            curr=curr+sizeof(MemBlock); //increase on struct size for first shift on memory start
        }
        else  //search first good condition
        {                                   
            curr=curr+sizeof(MemBlock); //for returning pointer on an allocated memory
            while(curr<=curr_end)  //go while current_end
            {                
                if((mem->av==0)&&(mem->size >= bytes)) // first suitable condition
                {
                    mem->av=1;
                    uint16_t incr_new=align(bytes); //need for address alignment
                    uint16_t incr_old=align(mem->size); //need for address alignment
                    // if not first allocation in this area need to split cells on two
                    uint16_t old_size = mem->size; //for next countings
                    uint16_t new_size=(old_size+incr_old)-(bytes+incr_new)+sizeof(MemBlock); //for next countings                   
                    // need condition to count free space and 
                    // to initiate this place (av=0 size=count-sizeof)  
                    mem->size=bytes;   
                    //if enough space than:               
                    if(((old_size+incr_old) - (bytes+incr_new)) > sizeof(MemBlock)+3) //adding new block
                    {   
                        void* new_curr = curr + mem->size + incr_new; //start address of new block (alignment)
                        mem=(MemBlock*)new_curr;
                        mem->av=0; //available
                        mem->size = new_size; //инициализируем незаполненую область для дальнейшего заполнения
                    }
                    //break;    
                    return curr;   
                }                 
                curr=curr+mem->size+sizeof(MemBlock)+align(mem->size); //go to next block    
                mem=(MemBlock*)(curr - sizeof(MemBlock)); // mem = curr<<3        
            }          
            curr=init_malloc(bytes); // if was not any available blocks
            mem=(MemBlock*)curr;
            mem->av=1;
            mem->size=bytes;
            curr=curr+sizeof(MemBlock); //increase on struct size for pointer on allocated memory             
        }        
        return curr;        
    }
    void free(void* firstbyte) //+ gluing free blocks
    {        
        mem=(MemBlock*)(firstbyte-sizeof(MemBlock));        
        mem->av=0;        
        void* curr_end = _sbrk(0);
        void* curr=&_heap_start;  
        gluing_free_blocks(curr,curr_end); // if available blocks go straight its gluing it 
    }
//}