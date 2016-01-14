#include "Globals.h"
#include "Application.h"
#include "ModuleCars.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "ModuleRoad.h"
#include "ModuleParticles.h"
#include "ModuleCollision.h"
#include "ModuleDriver.h"
#include "ModulePlayer.h"
#include "SDL/include/SDL.h"

ModuleCars::ModuleCars(CARS car_type, int gear, bool start_enabled)
	: Module(start_enabled),
	gear(gear),
	dist(0),
	car_type(car_type),
	weapon(NONE),
	to_delete(false)
{


	position.x = RTILE_WIDTH * 7;
	position.y = RTILE_HEIGHT * 6.5;

	if (car_type != PLAYER){
		RoadLine * startLine = *App->road->screen.begin();
		if (App->player->gear < 5){
			position.y = RTILE_HEIGHT * 9;

		}
		else{
			startLine = *App->road->screen.begin()+6;
			position.y = RTILE_HEIGHT * 0;
		}
		int left=SCREEN_WIDTH, right=0;
		
		position.y -= App->player->pos;

		left = 4 * RTILE_WIDTH;
		right = 11 * RTILE_WIDTH;

		/*
		// Random 
		for (vector<Collider*>::iterator it = startLine->mask->begin(); it != startLine->mask->end(); ){
			if ((*it)->type == COL_ROAD_BORDER){
				if ((*it)->rect.x < left)
					left = (*it)->rect.x;
				if ((*it)->rect.x > right)
					right = (*it)->rect.x;
			}
			++it;
		}*/

		left += RTILE_WIDTH;
		right -= RTILE_WIDTH;

		if (left == right)
			right += RTILE_WIDTH;

		position.x = rand() % (right - left) + left;
	}

	last_position = position;


	if (car_type != MAD_BOMBER){
		idle.frames.push_back({ MTILE_SIZE * 0, MTILE_SIZE * car_type, MTILE_SIZE, MTILE_SIZE });
		idle.speed = 0.2f;

		right.x = MTILE_SIZE * 2;
		right.y = MTILE_SIZE * car_type;
		right.w = MTILE_SIZE;
		right.h = MTILE_SIZE;

		left.x = MTILE_SIZE * 3;
		left.y = MTILE_SIZE * car_type;
		left.w = MTILE_SIZE;
		left.h = MTILE_SIZE;

		for (int i = 0; i < 8; ++i)
			crash.frames.push_back({ MTILE_SIZE * 9, MTILE_SIZE * i, MTILE_SIZE, MTILE_SIZE });
	}
	crash.speed = 0.02f;

	switch (car_type){
	case RED_CAR:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 20, position.y, 25, 40 }, COL_CAR, this);
		break;
	case BLUE_CAR:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 20, position.y, 23, 32 }, COL_CAR, this);
		break;
	case MOTO:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 23, position.y, 19, 32 }, COL_CAR, this);
		break;
	case TRUCK:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 17, position.y, 32, 64 }, COL_TRUCK, this);
		break;
	case ROAD_LORD:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 18, position.y, 28, 41 }, COL_ROAD_LORD, this);
		break;
	case SWITCH_BLADE:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 19, position.y, 24, 41 }, COL_CAR, this);
		break;
	case ENFORCER:
		mask = App->masks->AddCollider(SDL_Rect{ position.x + 17, position.y, 28, 58 }, COL_CAR, this);
		break;
	}


}

ModuleCars::~ModuleCars()
{
	// Homework : check for memory leaks
}

bool ModuleCars::Start()
{
	LOG("Loading Cars");
	return true;
}

// Load assets
// Load Colliders
bool ModuleCars::Resume()
{
	LOG("Init Cars");

	return true;
}

// Unload assets
bool ModuleCars::CleanUp()
{
	LOG("Unloading Cars");

	App->masks->colliders.remove(mask);

	return true;
}


update_status ModuleCars::PreUpdate()
{
	if (last_position.x == position.x)
		moving = STRAIGHT;
	else if (last_position.x < position.x)
		moving = RIGHT;
	else if (last_position.x > position.x)
		moving = LEFT;

	if (SDL_GetTicks() % 2000 > 0 && SDL_GetTicks() % 2000 < 20)
		TurnRandom();
	else
		moving = STRAIGHT;

	dist += (gear - App->player->gear);
	mask->rect.y -= gear;/// +App->player->pos;

	last_position = position;

	return UPDATE_CONTINUE;
}

update_status ModuleCars::Update()
{
	switch (moving){
	case STRAIGHT:
		App->renderer->Blit(App->driver->graphics, position.x, position.y - gear, &(idle.GetFrame(0)), 1.0f, RENDER_OTHER, dist);
		break;
	case RIGHT:
		App->renderer->Blit(App->driver->graphics, position.x, position.y - gear, &right, 1.0f, RENDER_OTHER, dist);
		break;
	case LEFT:
		App->renderer->Blit(App->driver->graphics, position.x, position.y - gear, &left, 1.0f, RENDER_OTHER, dist);
		break;
	}
	last_position = position;
	return UPDATE_CONTINUE;
}


void ModuleCars::SetState(int new_state){
	/*if (state != HADOUKEN && state != JUMP && state != WON){
	state = new_state;
	if (state == JUMP)
	jump_live.Start();
	if (state == HADOUKEN)
	hado_live.Start();
	if (state == WON)
	won_live.Start();
	}*/

}

void ModuleCars::SetMovement(Movement new_state){
	moving = new_state;

	int dif = position.x - mask->rect.x;

	if (moving == RIGHT)
		mask->rect.x += gear * 2;
	else if (moving == LEFT)
		mask->rect.x -= gear * 2;

	if (mask->rect.x < 0)
		mask->rect.x = 0;

	if (mask->rect.x >(App->window->screen_surface->w - STILE_SIZE))
		mask->rect.x = (App->window->screen_surface->w - STILE_SIZE);

	position.x = mask->rect.x + dif;
}

bool ModuleCars::OnColision(Collider* a, Collider *b, COLISION_STATE status)
{
	LOG("Collision Car");
	if (status == COL_START){
		if (a->type == COL_CAR && b->type == COL_CAR){
			if (a->rect.x > b->rect.x)
				SetMovement(LEFT);
			else
				SetMovement(RIGHT);
		}
		if (a->type == COL_CAR && b->type == COL_ROAD_OUT){
			App->particles->addParticle(mask->rect.x, mask->rect.y + mask->rect.h, ANIM_EXPLOTE);
			gear = 0;
		}
		if (a->type == COL_CAR && b->type == COL_SPRAY){
			App->particles->addParticle(mask->rect.x, mask->rect.y + mask->rect.h, ANIM_EXPLOTE);
			gear = 0;
		}
		if (a->type == COL_CAR && b->type == COL_OIL){
			App->particles->addParticle(mask->rect.x, mask->rect.y + mask->rect.h, ANIM_EXPLOTE);
			gear = 0;
		}
		if (a->type == COL_CAR && b->type == COL_BOMB){
			App->particles->addParticle(mask->rect.x, mask->rect.y + mask->rect.h, ANIM_EXPLOTE);
			gear = 0;
		}
		if (a->type == COL_CAR && b->type == COL_BULLET){
			App->particles->addParticle(mask->rect.x, mask->rect.y + mask->rect.h, ANIM_EXPLOTE);
			gear = 0;
		}

	}
	if (status == COL_DURING){
		if (a->type == COL_CAR && b->type == COL_PLAYER){
			if (a->rect.x > b->rect.x)
				SetMovement(LEFT);
			else
				SetMovement(RIGHT);
		}
		if (a->type == COL_CAR && b->type == COL_ROAD_BORDER){
			if (a->rect.x > b->rect.x)
				SetMovement(LEFT);
			else
				SetMovement(RIGHT);
		}
	}
	return true;
}
void ModuleCars::UpGear(){
	if (gear != 8)
		gear += 4;
}

void ModuleCars::DownGear(){
	if (gear != 0)
		gear -= 4;
}

fPoint ModuleCars::GetPivot(){
	fPoint res;
	res.x = mask->rect.x + mask->rect.w / 2;
	res.y = mask->rect.y;
	return res;
}

void ModuleCars::SetWeapon(Weapon new_weapon){
	if (weapon != WORKING || new_weapon == NONE)
		weapon = new_weapon;
}

void ModuleCars::TurnRandom(){
	int random = rand() % 10 + 1;
	if (random < 5)
		SetMovement(RIGHT);
	else
		SetMovement(LEFT);
}