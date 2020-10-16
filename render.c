/*
Project: N64Jam

File: render.c
Purpose: Provides a basic rendering interface

Authors: Regan "cuckydev" Green
*/

#include "render.h"
#include "config.h"

#include <assert.h>

//Current render state
static RenderState render_state;

//Render constants
const Rect grect_full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

//Gfx list
static Gfx glist[GLIST_LENGTH];
Gfx *glistp;

//RCP initialization DLs
static Vp viewport = {{
	{SCREEN_WIDTH << 1, SCREEN_HEIGHT << 1, G_MAXZ, 0},
	{SCREEN_WIDTH << 1, SCREEN_HEIGHT << 1, G_MAXZ, 0},
}};

static Gfx dl_init_rsp[] = {
	gsSPViewport(&viewport),
	gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
	                      G_FOG | G_LIGHTING | G_TEXTURE_GEN |
	                      G_TEXTURE_GEN_LINEAR | G_LOD),
	gsSPTexture(0, 0, 0, 0, G_OFF),
	gsSPEndDisplayList(),
};

static Gfx dl_init_rdp[] = {
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPPipeSync(),
	gsSPEndDisplayList(),
};

//Internal draw functions
static void PreNMICallback()
{
	//Reset VI to prevent a console crash on PAL systems
	nuGfxDisplayOff();
	osViSetYScale(1);
}

//Draw interface
void InitRender(NUGfxFunc vblank_callback)
{
	//Setup for target television
	switch (osTvType)
	{
		case OS_TV_PAL:
			osViSetMode(&osViModeTable[OS_VI_FPAL_LAN1]);
			osViSetYScale(0.833);
			nuPreNMIFuncSet((NUScPreNMIFunc)PreNMICallback);
			break;
		case OS_TV_MPAL:
			osViSetMode(&osViModeTable[OS_VI_MPAL_LAN1]);
			break;
		default:
			break;
	}
	
	//Initialize NuSys
	nuGfxInit();
	nuGfxFuncSet(vblank_callback);
}

void StartRender()
{
	//Turn on display
	nuGfxDisplayOn();
}

void StartRenderFrame()
{
	//Reset pointer to head
	glistp = glist;
	
	//Initialize RCP
	gSPSegment(glistp++, 0, 0);
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(dl_init_rsp));
	gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(dl_init_rdp));
	
	//Initialize frame
	gDPSetCycleType(glistp++, G_CYC_FILL);
	gDPSetDepthImage(glistp++, nuGfxZBuffer);
	gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, nuGfxZBuffer);
	gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
	gDPFillRectangle(glistp++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
	gDPPipeSync(glistp++);
	
	//Reset draw state
	render_state = RS_Null;
}

void EndRenderFrame()
{
	//End display list and execute
	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);
	assert(glistp - glist < GLIST_LENGTH);
	nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx), NU_GFX_UCODE_F3DEX2, NU_SC_SWAPBUFFER);
}

void SetRenderState(RenderState next_render_state)
{
	//Change render state
	if (next_render_state == render_state)
		return;
	render_state = next_render_state;
	
	//Prepare for given render state
	switch (render_state)
	{
		case RS_Rect:
			gDPSetCycleType(glistp++, G_CYC_FILL);
			gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, osVirtualToPhysical(nuGfxCfb_ptr));
			break;
		case RS_Tex:
			gDPSetCycleType(glistp++, G_CYC_1CYCLE);
			gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, osVirtualToPhysical(nuGfxCfb_ptr));
			
			gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
			gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
			
			gDPSetDepthSource(glistp++, G_ZS_PRIM);
			gDPSetPrimDepth(glistp++, 0, 0);
			gDPSetTexturePersp(glistp++, G_TP_NONE);
			break;
		default:
			break;
	}
}

void RenderRect(const Rect *rect, u32 col)
{
	//Clip rect against top left
	s32 left = rect->left, top = rect->top;
	if (left < 0)
		left = 0;
	if (top < 0)
		top = 0;
	if (rect->right <= left || rect->bottom <= top || left >= SCREEN_WIDTH || top >= SCREEN_HEIGHT)
		return;
	
	//Draw rect
	SetRenderState(RS_Rect);
	gDPSetFillColor(glistp++, col);
	gDPFillRectangle(glistp++, left, top, rect->right - 1, rect->bottom - 1);
	gDPPipeSync(glistp++);
}
