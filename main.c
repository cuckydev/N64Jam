/*
Project: N64Jam

File: main.c
Purpose: Provide the entry point

Authors: Regan "cuckydev" Green
*/

#include <nusys.h>

#include "mem.h"
#include "render.h"
#include "game.h"
#include "math_util.h"
#include "input.h"

void VBlankCallback(int pending)
{
	GetInput();
	UpdateGame();
	if (pending < 1)
	{
		StartRenderFrame();
		RenderGame();
		EndRenderFrame();
	}
}

static u8 mem_heap[0x80000];
void mainproc(void)
{
	//Initialize memory heap
	if (Mem_Init((void*)mem_heap, sizeof(mem_heap)))
		return;
	
	//Initialize sub-systems
	InitRender((NUGfxFunc)VBlankCallback);
	InitInput();
	
	//Initialize game
	InitMathUtil();
	StartGame();
	
	//Start game
	StartRender();
	
	while (1);
}
