#ifndef MAP_H
#define MAP_H

#include "game_def.h"

//Map constants
#define MAP_TILE_DIM (16)

//Map types
typedef enum
{
	TilesetId_Grasslands,
	TilesetId_Num,
} TilesetId;

typedef enum
{
	MapId_Test,
	MapId_Num,
} MapId;

typedef struct
{
	TilesetId tileset;
	u8 *map;
} MapLayer;

typedef struct
{
	//Map information
	u16 pitch, width, height;
	u8 *colmap;
	
	//Layers
	u8 layers;
	MapLayer *layer;
} Map;

//Map interface
void Map_Init(Map *map);
void Map_Quit(Map *map);
void Map_Load(Map *map, MapId id);
void Map_Render(Map *map);

#endif
