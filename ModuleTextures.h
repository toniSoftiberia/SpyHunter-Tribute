#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include<list>
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
using namespace std;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Start();
	bool Stop();

	SDL_Texture* const Load(const char* path);
	void Unload(SDL_Texture* texture);

private:
	list<SDL_Texture*> textures;
};

#endif // __MODULETEXTURES_H__