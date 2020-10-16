/*
Project: N64Jam

File: main.c
Purpose: Provide the entry point

Authors: Regan "cuckydev" Green
*/

#include <nusys.h>

#include "mem.h"
#include "render.h"

void VBlankCallback(int pending)
{
	Rect test = {0, 0, 80, 80};
	if (pending < 1)
	{
		StartRenderFrame();
		RenderRect(&grect_full, RGB(0x80, 0x00, 0x00));
		RenderRect(&test, RGB(0x00, 0x80, 0x80));
		EndRenderFrame();
	}
}

void mainproc(void)
{
	//Initialize memory heap
	static u8 mem_heap[0x80000];
	if (Mem_Init((void*)mem_heap, sizeof(mem_heap)))
		return;
	
	//Initialize sub-systems
	InitRender((NUGfxFunc)VBlankCallback);
	
	//Start game
	StartRender();
	
	while (1);
}
