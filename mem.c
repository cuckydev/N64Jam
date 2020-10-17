/*
Project: N64Jam

File: mem.c
Purpose: Basic memory heap allocator for N64 homebrew projects

Authors: Regan "cuckydev" Green
*/

#include "mem.h"

#define MEM_ALIGN(x) (((size_t)x + 0xF) & ~0xF)

typedef struct
{
	void *prev, *next;
	size_t size;
} Mem_Header;

static Mem_Header *mem = NULL;

u8 Mem_Init(void *ptr, size_t size)
{
	//Make sure there's enough space for mem header
	if (size < sizeof(Mem_Header))
		return 1;
	
	//Get mem pointer (16 byte alignment)
	mem = (Mem_Header*)MEM_ALIGN(ptr);
	size -= (size_t)mem - (size_t)ptr;
	
	//Initial mem header
	mem->prev = NULL;
	mem->next = NULL;
	mem->size = size - sizeof(Mem_Header);
	return 0;
}

static Mem_Header *Mem_GetHeader(void *ptr)
{
	if (ptr == NULL)
		return NULL;
	return (Mem_Header*)(ptr - sizeof(Mem_Header));
}

void *Mem_Alloc(size_t size)
{
	//Get block to allocate in front of
	Mem_Header *header = mem;
	if (header == NULL)
		return NULL;
	
	size_t header_size = 0;
	size_t next_pos = MEM_ALIGN((size_t)header + sizeof(Mem_Header));
	
	while (header != NULL)
	{
		//If there's no block to check up next, compare against the end of the heap
		if (header->next == NULL)
		{
			//Check if there's enough space to allocate
			size_t end = (size_t)mem + sizeof(Mem_Header) + mem->size;
			if ((next_pos + sizeof(Mem_Header) + size) > end)
				return NULL;
			break;
		}
		
		//Check if there's enough space to allocate
		if (((size_t)header->next - sizeof(Mem_Header)) >= (next_pos + sizeof(Mem_Header) + size))
			break;
		
		//Check next block
		if ((header = Mem_GetHeader(header->next)) == NULL)
			return NULL;
		header_size = header->size;
		next_pos = MEM_ALIGN((size_t)header + sizeof(Mem_Header) + header_size);
	}
	
	//Setup header
	Mem_Header *new_header = (Mem_Header*)next_pos;
	new_header->size = size;
	
	void *new_block = (void*)(next_pos + sizeof(Mem_Header));
	
	//Link header to previous and next headers
	new_header->prev = (void*)header + sizeof(Mem_Header);
	new_header->next = header->next;
	
	//Link next header to us
	if (header->next != NULL)
		Mem_GetHeader(header->next)->prev = new_block;
	
	//Link previous header to us
	header->next = new_block;
	return new_block;
}

void Mem_Free(void *ptr)
{
	//Get this block's header
	Mem_Header *header = Mem_GetHeader(ptr);
	if (header == NULL)
		return;
	
	//Unlink from previous block
	Mem_Header *prev_header = Mem_GetHeader(header->prev);
	if (prev_header != NULL)
		prev_header->next = header->next;
	
	//Unlink from next block
	Mem_Header *next_header = Mem_GetHeader(header->next);
	if (next_header != NULL)
		next_header->prev = header->prev;
}
