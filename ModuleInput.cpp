#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleRoad.h"
#include "ModuleWindow.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCars.h"
#include "ModuleAudio.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({0, 0}), mouse_motion({0,0})
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool ModuleInput::Start()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool ModuleInput::Resume()
{
	return true;
}

// Called each loop iteration
update_status ModuleInput::PreUpdate()
{
	static SDL_Event event;

	mouse_motion = {0, 0};
	memset(windowEvents, false, WE_COUNT * sizeof(bool));
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	bool w_pulsed = false;

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE){
				keyboard[i] = KEY_DOWN;
				if (App->road->road_state == G_PLAY){
					if (keyboard[SDL_SCANCODE_Q] || keyboard[SDL_SCANCODE_RCTRL]) {
						//up velocity
						App->player->DownTurboGear();
					}
					if (keyboard[SDL_SCANCODE_E] || keyboard[SDL_SCANCODE_RSHIFT]) {
						//up velocity
						App->player->UpTurboGear();
					}
					else if (keyboard[SDL_SCANCODE_L] && App->player->velocity != 0) {
						App->player->SetWeapon(ROCKET);
					}
					else if (keyboard[SDL_SCANCODE_Y]) {
						App->player->GetRandWeapon();
					}
					else if (keyboard[SDL_SCANCODE_F1]) {
						//DEBUG
						App->masks->debug_mode = !App->masks->debug_mode;
					}
					else if (keyboard[SDL_SCANCODE_F2]) {
						//NO_RENDER
						App->renderer->printer_mode = !App->renderer->printer_mode;
					}
					else if (keyboard[SDL_SCANCODE_F3]) {
						//GOD MODE
						App->player->GodMode();
					}
					else if (keyboard[SDL_SCANCODE_F4]) {
						//FULL WEAPON MODE
						App->player->oil = 99;
						App->player->spray = 49;
						App->player->rocket = 9;
					}
					else if (keyboard[SDL_SCANCODE_F5]) {
						//SET AMBIEN FOREST
						App->road->AmbientChange(A_FOREST);
					}
					else if (keyboard[SDL_SCANCODE_F6]) {
						//SET AMBIEN DESERT
						App->road->AmbientChange(A_DESERT);
					}
					else if (keyboard[SDL_SCANCODE_F7]) {
						//SET AMBIEN GREEN
						App->road->AmbientChange(A_GREEN);
					}
					else if (keyboard[SDL_SCANCODE_F8]) {
						//SET AMBIEN GREY
						App->road->AmbientChange(A_GREY);
					}
					else if (keyboard[SDL_SCANCODE_F9]) {
						//SET AMBIEN SNOW
						App->road->AmbientChange(A_SNOW);
					}
					else if (keyboard[SDL_SCANCODE_F10]) {
						//LIVES ++
						if (App->player->lives < 3){
							++App->player->lives;
							App->audio->PlayFx(AUD_LIVE_UP);
						}
					}
					else if (keyboard[SDL_SCANCODE_F11]) {
						//LIVES --
						if (App->player->lives > 0)
							--App->player->lives;
					}
				}
				if (keyboard[SDL_SCANCODE_SPACE]) {
					//START
					if (App->road->road_state == G_PAUSE)
						App->player->Alive();
					App->road->SetGameState(G_PLAY);
				}
			}
			else{
				keyboard[i] = KEY_REPEAT;

				if (App->road->road_state == G_PLAY){
					if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]) {
						//up velocity
						App->player->UpGear();
						w_pulsed = true;
					}
					if (keyboard[SDL_SCANCODE_H] && App->player->velocity != 0) {
						App->player->SetWeapon(GUN);
					}
					else if (keyboard[SDL_SCANCODE_J] && App->player->velocity != 0) {
						App->player->SetWeapon(OIL);
					}
					else if (keyboard[SDL_SCANCODE_K] && App->player->velocity != 0) {
						App->player->SetWeapon(SPRAY);
					}
					if (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT]) {
						App->player->SetMovement(RIGHT);
					}
					if (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT]) {
						App->player->SetMovement(LEFT);
					}
				}
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN){
				keyboard[i] = KEY_UP;
				if (App->player->velocity != 0 && (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D])) {
					App->audio->PlayFx(AUD_TURN);
				}
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}


	if (!w_pulsed) {
		App->player->DownGear();
	}


	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
			break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = event.motion.xrel / SCREEN_SIZE;
				mouse_motion.y = event.motion.yrel / SCREEN_SIZE;
				mouse.x = event.motion.x / SCREEN_SIZE;
				mouse.y = event.motion.y / SCREEN_SIZE;
			break;
		}
	}

	if(GetWindowEvent(EventWindow::WE_QUIT) == true || GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::Stop()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	delete keyboard;
	return true;
}

// ---------
bool ModuleInput::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[ev];
}

const iPoint& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const iPoint& ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}

KeyState ModuleInput::GetKey(int id) const
{
	return keyboard[id];
}

KeyState ModuleInput::GetMouseButtonDown(int id) const
{
	return mouse_buttons[id - 1];
}