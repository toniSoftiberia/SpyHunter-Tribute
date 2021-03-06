#ifndef __MODULEPARTICULES_H__
#define __MODULEPARTICULES_H__
#include "Module.h"
#include "Animation.h"
#include "Timer.h"
#include "Globals.h"
#include "Point.h"
#include "ModuleCollision.h"

struct SDL_Texture;

/*struct particle{ 
	fPoint pos;
	Timer live;
	Animation anim;
	Vector scale;
	SDL_Texture tex;
	fPoint speed;
	status;
	Color tintado;
	fPoint accel;
	bool visible;
	fx start;
	fx update;
	float rotation;
	float rot_speed;
	float rot_accel;
};*/

struct Particle{
	fPoint pos;
	fPoint start;
	Timer live;
	Animation anim;
	SDL_Rect test;
	SDL_Texture* tex = nullptr;
	float speed;
	int killing = 0;
};

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled = true);
	~ModuleParticles();

	bool Start();
	bool Resume();
	update_status PreUpdate();
	update_status Update();
	bool Stop();
	bool addParticle(float x, float y, ANIMATION_TYPE type);
	bool addParticleBackground(float x, float y, ANIMATION_TYPE type);
	bool deleteParticle(int id);
	void runParticle(ANIMATION_TYPE animation);
	bool OnColision(Collider* a, Collider *b);
	void DeleteParticles();

	std::vector<pair<Particle*, Collider*>> particles;
	std::vector<pair<Particle*, Collider*>> background;
	SDL_Texture* graphics = nullptr;

private:
	pair<Particle*, Collider*> * add(float x, float y, ANIMATION_TYPE type);

};

#endif // __MODULEPARTICULES_H__