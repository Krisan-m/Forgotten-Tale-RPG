#pragma once
#include <string>

class Map
{
public:

	Map(std::string tID, int ms, int ts);
	~Map();

	void AddTile(int srcX, int srcY, int xpos, int ypos);
	void LoadMap(std::string path, int sizeX, int sizeY);
	std::string mapName() { return texID; };



private:
	std::string texID;
	int mapScale;
	int tileSize;
	int scaledSize;

};