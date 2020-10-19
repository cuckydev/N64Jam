#include "map.h"
#include "mem.h"
#include "render.h"

#include <string.h>

//Tilesets
#include "assets/tile/grasslands.inc.c"

static const struct
{
	u8 *tex;
	u16 *tlut;
} tilesets[TilesetId_Num] = {
	/* Tileset_Grasslands */ {tile_grasslands_tex, tile_grasslands_tlut},
};

//Map definitions
typedef struct
{
	TilesetId tileset;
	const u8 *map;
} MapDefLayer;

typedef struct
{
	u16 pitch, width, height;
	const u8 *colmap;
	u8 layers;
	const MapDefLayer *layer;
} MapDef;

#include "assets/map/test.inc.c"

static const MapDef *map_def[MapId_Num] = {
	&map_test_def,
};

//Internal map interface
static void Map_FreeMap(Map *map)
{
	//Free collision map
	if (map->colmap != NULL)
	{
		Mem_Free(map->colmap);
		map->colmap = NULL;
	}
	
	//Free layers
	if (map->layer != NULL)
	{
		for (u8 i = 0; i < map->layers; i++)
			if (map->layer[i].map != NULL)
				Mem_Free(map->layer[i].map);
		Mem_Free(map->layer);
		map->layer = NULL;
	}
}

//Map interface
void Map_Init(Map *map)
{
	//Initialize blank map
	map->width = 0;
	map->height = 0;
	map->colmap = NULL;
	map->layers = 0;
	map->layer = NULL;
}

void Map_Quit(Map *map)
{
	//Free map data
	Map_FreeMap(map);
}

void Map_Load(Map *map, MapId id)
{
	//Free previous map
	Map_FreeMap(map);
	
	//Load new map
	const MapDef *def = map_def[id];
	
	//Use map information
	map->pitch = def->pitch;
	map->width = def->width;
	map->height = def->height;
	u32 map_size = map->pitch * map->height;
	
	//Copy collision map
	if (def->colmap != NULL)
	{
		map->colmap = Mem_Alloc(map_size);
		memcpy(map->colmap, def->colmap, map_size);
	}
	
	//Copy layers
	map->layers = def->layers;
	if (map->layers != 0)
	{
		map->layer = Mem_Alloc(sizeof(MapLayer) * map->layers);
		
		const MapDefLayer *layer_src = def->layer;
		MapLayer *layer_dst = map->layer;
		for (u8 i = 0; i < map->layers; i++, layer_src++, layer_dst++)
		{
			layer_dst->tileset = layer_src->tileset;
			layer_dst->map = Mem_Alloc(map_size);
			memcpy(layer_dst->map, layer_src->map, map_size);
		}
	}
}

void Map_Render(Map *map)
{
	//Render map layers
	MapLayer *layer = map->layer;
	for (u8 i = 0; i < map->layers; i++, layer++)
	{
		//Get layer pointers
		u8 *mapp_row = layer->map;
		
		//Load layer texture
		LoadTex_CI4(64, 64, tilesets[layer->tileset].tex, tilesets[layer->tileset].tlut);
		
		//Render rows
		for (u32 y = 0; y < (SCREEN_HEIGHT + MAP_TILE_DIM - 1) / MAP_TILE_DIM; y++)
		{
			//Get row pointers
			u8 *mapp = mapp_row;
			mapp_row += map->pitch;
			
			//Render row
			for (u32 x = 0; x < (SCREEN_WIDTH + MAP_TILE_DIM - 1) / MAP_TILE_DIM; x++)
			{
				//Unpack two tiles
				u8 ln = *mapp >> 4;
				u8 rn = *mapp & 0xF;
				mapp++;
				
				//Render left tile
				if (ln != 0)
				{
					s32 left_x = (ln & 3) * MAP_TILE_DIM;
					s32 left_y = (ln / 4) * MAP_TILE_DIM;
					Rect left_tile = {
						left_x,
						left_y,
						left_x + MAP_TILE_DIM,
						left_y + MAP_TILE_DIM
					};
					RenderTex_Quick(&left_tile, (x << 1) * MAP_TILE_DIM, y * MAP_TILE_DIM);
				}
				
				//Render left tile
				if (rn != 0)
				{
					s32 right_x = (rn & 3) * MAP_TILE_DIM;
					s32 right_y = (rn / 4) * MAP_TILE_DIM;
					Rect right_tile = {
						right_x,
						right_y,
						right_x + MAP_TILE_DIM,
						right_y + MAP_TILE_DIM
					};
					RenderTex_Quick(&right_tile, ((x << 1) + 1) * MAP_TILE_DIM, y * MAP_TILE_DIM);
				}
			}
		}
	}
}
