
#pragma once
#include "Game.h"
#include "Sprites.h"
#include<fstream>
#include "Utils.h"
#include <string>
class Map
{
	LPDIRECT3DTEXTURE9 tex;
	int TexRow, TexCol, MapRow, MapCol;
	UINT TexW, TexH;
	int mapId;
	int map[100][100];
	char* FileName;
	string MapLink;
	int nMap;
public:
	Map() {};
	~Map() {};
	void ReadMap();
	void Drawmap();
	void SetMap(int Id);
};
