#ifndef RENDER_H
#define RENDER_H

#include <nusys.h>
#include "game_def.h"
#include "config.h"

//Render types
typedef enum
{
	RS_Null,
	RS_Rect,
	RS_Tex,
} RenderState;

//Render constants
extern const Rect grect_full;

//Render macros
#define RGB(r, g, b) ((GPACK_RGBA5551((r), (g), (b), 1) << 16) | GPACK_RGBA5551((r), (g), (b), 1))

//Render globals
extern Gfx *glistp;

//Render interface
void InitRender(NUGfxFunc vblank_callback);
void StartRender();
void StartRenderFrame();
void EndRenderFrame();
void SetRenderState(RenderState next_render_state);
void RenderRect(const Rect *rect, u32 col);

#endif
