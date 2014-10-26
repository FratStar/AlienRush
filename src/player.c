/*
*  Player handling functions and constructor
*/
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "projectiles.h"/*the player won't need this when the game is done*/
#include "particle.h"
#include "sprite.h"
#include "weapons.h"

#define  JT_Button  0
#define  JT_Axis    1
#define  JT_Inverse 2
#define  Wiggle     6000
#define  AWiggle    -6000

extern arSurface *screen;
extern arJoystick *joy;
extern arRect Camera;

enum PlayerInputs {PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight,
	PI_FireDown,PI_FireDownLeft,PI_FireLeft,PI_FireUpLeft,PI_FireUp,PI_FireUpRight,PI_FireRight,PI_FireDownRight,
	PI_FireUltimate,PI_NextWeapon,PI_PreviousWeapon,PI_Pause,PI_NULL};

int   JoyButtons[PI_NULL][2];
Uint32 KeyButtons[PI_NULL];


int PlayerCommands[3]; 
Entity *ThePlayer = NULL;
SDL_Surface *titlebar = NULL;
int attackleft = SDLK_LEFT;
void UpdatePlayer(Entity *self);
void PlayerThink(Entity *self);
void FinalOutput();
void UpdateInput();

PlayerStats ThisPlayer_Stats;


void SpawnPlayer(int x,int y)
{
	Entity *newent = NULL;
	newent = NewEntity();
	if(newent == NULL)
	{
		fprintf(stderr,"Unable to generate player entity; %s",SDL_GetError());
		exit(0);
	}
	strcpy(newent->EntName,"Player\0");
	newent->sprite = LoadSwappedSprite("images/playersprites/playersheet.png",55,69);
	/*ThisPlayer_Stats.color[PC_Core1] = Blue;
	ThisPlayer_Stats.color[PC_Core2] = DarkRed;
	ThisPlayer_Stats.color[PC_Leg1] = Blue;
	ThisPlayer_Stats.color[PC_Leg2] = DarkRed;
	ThisPlayer_Stats.color[PC_Leg3] = Silver;
	ThisPlayer_Stats.color[PC_Gun] = DarkGrey;
	ThisPlayer_Stats.color[PC_Trail] = LightBlue;
	SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
	SDL_SetColorKey(newent->legs->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->legs->image->format, 0,0,0));*/
	newent->size.x = newent->sprite->w;
	newent->size.y = newent->sprite->h;
	newent->update = UpdatePlayer;
	newent->think = PlayerThink;
	newent->UpdateRate = 35;
	newent->ThinkRate = 35;
	/* newent->legstate = 1;*/
	newent->takedamage = 1;
	newent->Score = 0;
	newent->Unit_Type = ET_Player;
	newent->switchdelay = 0;
	newent->healthmax = 100;
	newent->health = 100;
	newent->frame = 0;
	newent->Color = LightBlue;
	newent->fcount = B_Tiny;
	newent->frate = 1;
	newent->jump = 1; 
	/*newent->legframe = 0;*/
	newent->state = ST_IDLE;
	newent->aimdir = F_East;
	newent->thick = 1;
	newent->trailhead = -1;
	newent->s.x = x;
	newent->s.y = y;
	newent->v.x = 0;
	newent->v.y = 0;
	/* newent->Ls.x = -6;
	newent->Ls.y = -16;*/
	newent->maxspeed = 4;
	newent->movespeed = 0;
	newent->accel = 1;
	newent->totaloffset = 24;
	newent->Boundingbox.x = 10;
	newent->Boundingbox.y = 10;
	newent->Boundingbox.w = 55;
	newent->Boundingbox.h = 67;  
	newent->origin.x = 24;
	newent->origin.y = 32;
	newent->currentweapon = 0;
	UpdatePlayer(newent);
	ThePlayer = newent;
	atexit(FinalOutput);
}

/*
* Think Function handles giving the entity new orders and changing states
*/

void PlayerThink(Entity *self)
{
	float t;
	char text[40];
	Uint8 *keys = SDL_GetKeyState(NULL);
	UpdateInput();
	if(self->switchdelay <= 0)
	{
		if(PlayerCommands[1] == PI_NextWeapon)
		{
			self->currentweapon++;
			if(self->currentweapon >= NumWeapons)self->currentweapon = 0;
			self->switchdelay = 4;
			sprintf(text,"Next Weapon : %s",PrintWeaponName(self->currentweapon));
			NewMessage(text,IndexColor(LightBlue));
		}
		if(PlayerCommands[1] == PI_PreviousWeapon)
		{
			self->currentweapon--;
			if(self->currentweapon < 0)self->currentweapon = NumWeapons - 1;
			self->switchdelay = 4;
			sprintf(text,"Last Weapon : %s",PrintWeaponName(self->currentweapon));
			NewMessage(text,IndexColor(LightBlue));
		}    
	}
	else self->switchdelay--;
	if((self->state != ST_DEAD)&&(self->state != ST_DIE))
	{
		/*testing (cheating)*/
		if(keys[SDLK_1])self->PowerLevel = 0;
		else if(keys[SDLK_2])self->PowerLevel = 1;
		else if(keys[SDLK_3])self->PowerLevel = 2;
		else if(keys[SDLK_4])self->PowerLevel = 3;
		/*player movement follows*/
		else if(PlayerCommands[0] == PI_MovUpLeft)
		{ 
			if(UpdateEntityPosition(self,0)==0)self->s.y+=20;
			if(self->jump==1)
			{
				self->v.y=-30;
				self->jump=0;
				t = self->accel * 3/*1.41*/;
				self->v.x -= t;
			}
			if(self->jump==0)
			{
				self->s.y -=4;
			}

			if(self->frame < 15 || self->frame >= 19)
				self->frame = 15;
			self->frame++;
		}
		else if(PlayerCommands[0] == PI_MovUpRight)
		{     

			if(UpdateEntityPosition(self,0)!=1)self->s.y+=20;

			if(self->jump==1){
				self->v.y=-30;
				self->jump=0;
				t = self->accel * 3;
				self->v.x+=t;
			}
			if(self->jump==0)
			{
				self->s.y-=4;
			}
			if(self->frame < 10 || self->frame >= 14)
				self->frame = 10;
			self->frame++;
			/*SpawnThrust(IndexColor(self->Color),self->s.x + 24,self->s.y + 24,-1.4,1.4,self->movespeed,self->movespeed * 4);*/
		}
		else if(PlayerCommands[0] == PI_MovDownRight)
		{
			if(UpdateEntityPosition(self,0)!=1)self->s.y+=20;
			self->v.x += self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
				self->v.y = 0;
				self->v.x = self->maxspeed;
				self->movespeed = self->maxspeed;
			}	


			if(self->frame < 45 || self->frame >= 49)
				self->frame = 45;
			self->frame++;
		}
		else if(PlayerCommands[0] == PI_MovDownLeft)
		{    
			if(UpdateEntityPosition(self,0)!=1)self->s.y+=20;
			self->v.x -= self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
				self->v.y = 0;
				self->v.x = self->maxspeed * -1;
				self->movespeed = self->maxspeed;
			} 
			if(self->frame < 50 || self->frame >= 54)
				self->frame = 50;
			self->frame++;
		}
		else if(PlayerCommands[0] == PI_MovUp)
		{   
			if(self->jump == 1)
			{
				self->v.y -= 30;
				self->movespeed = VectorLength(self->v.x,self->v.y);
				self->jump = 0;
			}
			if(self->jump == 0)
			{
				self->s.y -=4;
			}
			/*self->v.y -= self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
			self->v.x = 0;
			self->v.y = self->maxspeed * -1;
			self->movespeed = self->maxspeed;
			}*/
			if(self->frame < 9 || self->frame > 14)
			{
				self->frame = 9;
			}
			self->frame++;
			/*SpawnThrust(IndexColor(self->Color),self->s.x + 24,self->s.y + 24,0,2,self->movespeed,self->movespeed * 4);*/
		}
		else if(PlayerCommands[0] == PI_MovDown)
		{      
			self->v.y += self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
				self->v.x = 0;
				self->v.y = self->maxspeed;
				self->movespeed = self->maxspeed;
			}
			/*SpawnThrust(IndexColor(self->Color),self->s.x + 24,self->s.y + 24,0,-1,self->movespeed,self->movespeed * 4);*/
		}
		else if(PlayerCommands[0] == PI_MovLeft)
		{      
			if(UpdateEntityPosition(self,0)!=1)self->s.y+=20;
			self->v.x -= self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
				self->v.y = 0;
				self->v.x = self->maxspeed * -1;
				self->movespeed = self->maxspeed;
			}
			if(self->frame < 5 || self->frame >= 9)
			{
				self->frame = 5;
			}
			self->frame++;
			/*SpawnThrust(IndexColor(self->Color),self->s.x + 24,self->s.y + 24,2,0,self->movespeed,self->movespeed * 4);*/
		}
		else if(PlayerCommands[0] == PI_MovRight)
		{      
			if(UpdateEntityPosition(self,0)!=1)self->s.y+=20;

			self->v.x += self->accel;
			self->movespeed = VectorLength(self->v.x,self->v.y);
			self->grounded = 0;
			if(self->movespeed > self->maxspeed)
			{
				self->v.y = 0;
				self->v.x = self->maxspeed;
				self->movespeed = self->maxspeed;
			}
				if(self->frame <= 0 || self->frame >= 4)
				{
					self->frame = 0;
				}
				self->frame++;

				/*SpawnThrust(IndexColor(self->Color),self->s.x + 24,self->s.y + 24,-2,0,self->movespeed,self->movespeed * 4);*/
			}

			switch(self->state)
			{
			case ST_IDLE: 
				if(PlayerCommands[1] == PI_FireUpLeft)
				{
					self->frame = 31;
					self->state = ST_ATTACK1;
					self->aimdir = F_NW;
					/*self->Ls.x = -6;
					self->Ls.y = -26;*/
					FireWeapon(self,F_NW);
				}
				else if(PlayerCommands[1] == PI_FireUp)
				{
					self->frame = 56;
					self->state = ST_ATTACK1;
					self->aimdir = F_North;
					/*self->Ls.x = -5;
					self->Ls.y = -26;*/
					FireWeapon(self,F_North);
				}    
				else if(PlayerCommands[1] == PI_FireUpRight)
				{
					self->frame = 36;
					self->state = ST_ATTACK1;
					self->aimdir = F_NE;
					/*self->Ls.x = 0;
					self->Ls.y = -26;*/
					FireWeapon(self,F_NE);
				}    
				else if(PlayerCommands[1] == PI_FireLeft)
				{
					self->frame = 25;
					self->state = ST_ATTACK1;
					self->aimdir = F_West;
					/*self->Ls.x = -8;
					self->Ls.y = -28;*/
					FireWeapon(self,F_West);
				}    
				else if(PlayerCommands[1] == PI_FireRight)
				{
					self->frame = 20;
					self->state = ST_ATTACK1;
					self->aimdir = F_East;
					/*self->Ls.x = 4;
					self->Ls.y = -28;*/
					FireWeapon(self,F_East);
				}    
				else if(PlayerCommands[1] == PI_FireDownLeft)
				{
					self->frame = 51;
					self->state = ST_ATTACK1;
					self->aimdir = F_SW;
					/*self->Ls.x = -5;
					self->Ls.y = -14;*/
					FireWeapon(self,F_SW);
				}    
				else if(PlayerCommands[1] == PI_FireDown)
				{
					self->frame = 0;
					self->state = ST_ATTACK1;
					self->aimdir = F_South;
					/*self->Ls.x = -6;
					self->Ls.y = -14;*/
					FireWeapon(self,F_South);
				}    
				else if(PlayerCommands[1] == PI_FireDownRight)
				{
					self->frame = 46;
					self->state = ST_ATTACK1;
					self->aimdir = F_SE;
					/*self->Ls.x = 4;
					self->Ls.y = -14;*/
					FireWeapon(self,F_SE);
				}    

				break;
			case ST_ATTACK1:
				break;
			}
		}
	}

	/*updates the entity's postition and handles animation*/

	void UpdatePlayer(Entity *self)
	{
		int Goframe = 0;
		UpdateEntityPosition(self,0);

		if(self->jump==0)
		{
			self->grounded = 0;
		}
		if(UpdateEntityPosition(self,0) == 1)
		{
			self->jump=1;
			self->v.x=0;
		}

		if(self->grounded == 0)
		{
			if(self->v.y < 20);
			self->v.y += 4;     /*gravity at work*/
		}
		ApplyFriction(self,0.05);
		if(self->state == ST_DIE)
		{
			self->fcount = 10;
			self->state = ST_DEAD;
			return;
		}
		else if(self->state == ST_DEAD)
		{
			self->fcount--;
			if(self->fcount <= 0)
			{
				FreeEntity(self);
				exit(0);
			}
			ExplodingParticle(self->s.x + self->origin.x,self->s.y + self->origin.y,crandom(),crandom());
			return;
		}
		else if(self->Cooldown > 0)self->Cooldown--;/*weapons cooloff*/
		else
		{
			self->state = ST_IDLE;
		}
		if(self->KillCount > 0)
		{
			self->Score += (self->KillCount * 10) * ((self->KillCount>>1) + 1);
			self->KillCount = 0;
		}
		if(self->fcount <= 0)
		{
			Goframe = 1;
			self->fcount = self->frate;
		}
		else
		{
			self->fcount--;
		}
		if(!Goframe)return;
		else GetFace(self);/*check out movement vector to see what direction we are facing.*/
		/*only frame animations should take place next*/
		switch(self->face)
			/*{
			case F_NULL:
			self->legframe = 0;
			break;
			case F_North:
			self->legframe++;
			if((self->legframe > 9)||(self->legframe < 7))self->legframe = 7;
			break;
			case F_South:
			self->legframe++;
			if((self->legframe > 3)||(self->legframe < 1))self->legframe = 1;
			break;
			case F_SW:
			case F_NW:
			case F_West:
			self->legframe++;
			if((self->legframe > 6)||(self->legframe < 4))self->legframe = 4;
			break;
			case F_SE:
			case F_NE:
			case F_East:
			self->legframe++;
			if((self->legframe > 12)||(self->legframe < 10))self->legframe = 10;
			break;


			}*/
			if(self->state == ST_ATTACK1)
			{
				self->frame++;
				switch(self->aimdir)
				{
				case F_South:
					if(self->frame > 2)self->frame = 0;
					break;
				case F_SW:
					if((self->frame > 5)||(self->frame < 3))self->frame = 3;
					break;
				case F_West:
					if((self->frame > 8)||(self->frame < 6))self->frame = 6;
					break;
				case F_NW:
					if((self->frame > 11)||(self->frame < 9))self->frame = 9;
					break;
				case F_North:
					if((self->frame > 14)||(self->frame < 12))self->frame = 12;
					break;
				case F_NE:
					if((self->frame > 17)||(self->frame < 15))self->frame = 15;
					break;
				case F_East:
					if((self->frame > 20)||(self->frame < 18))self->frame = 18;
					break;
				case F_SE:
					if((self->frame > 23)||(self->frame < 21))self->frame = 21;
					break;
				default:
					self->frame = 0;
				}
			}
			if(self->s.x > 800)
			{
				Camera.x++;
			}
	}

	int CheckCommand(int i)
	{
		Uint8 *keys = SDL_GetKeyState(NULL);
		if(((JoyButtons[i][0] == JT_Axis)&&(SDL_JoystickGetAxis(joy, JoyButtons[i][1]) > Wiggle))
			||((JoyButtons[i][0] == JT_Inverse)&&(SDL_JoystickGetAxis(joy, JoyButtons[i][1]) < AWiggle))
			||((JoyButtons[i][0] == JT_Button)&&(SDL_JoystickGetButton(joy, JoyButtons[i][1])))
			||(keys[KeyButtons[i]]))
			return 1;
		return 0;
	}

	void UpdateInput()
	{
		int i;
		for(i = 0;i < 3;i++)PlayerCommands[i] = -1;

		i = PI_MovDown;
		if(CheckCommand(i))
			PlayerCommands[0] = i;

		i = PI_MovUp;
		if(CheckCommand(i))
			PlayerCommands[0] = i;

		i = PI_MovLeft;
		if(CheckCommand(i))
		{
			if(PlayerCommands[0] == PI_MovUp)
			{
				PlayerCommands[0] = PI_MovUpLeft; 
			}
			else if(PlayerCommands[0] == PI_MovDown)
			{
				PlayerCommands[0] = PI_MovDownLeft; 
			}
			else PlayerCommands[0] = i;
		}

		i = PI_MovRight;
		if(CheckCommand(i))
		{
			if(PlayerCommands[0] == PI_MovUp)
			{
				PlayerCommands[0] = PI_MovUpRight; 
			}
			else if(PlayerCommands[0] == PI_MovDown)
			{
				PlayerCommands[0] = PI_MovDownRight; 
			}
			else PlayerCommands[0] = i;
		}


		if(CheckCommand(PI_FireDown))
			PlayerCommands[1] = PI_FireDown;

		if(CheckCommand(PI_FireUp))
			PlayerCommands[1] = PI_FireUp;

		i = PI_FireLeft;
		if(CheckCommand(i))
		{
			if(PlayerCommands[1] == PI_FireUp)
			{
				PlayerCommands[1] = PI_FireUpLeft; 
			}
			else if(PlayerCommands[1] == PI_FireDown)
			{
				PlayerCommands[1] = PI_FireDownLeft; 
			}
			else PlayerCommands[1] = i;
		}

		i = PI_FireRight;
		if(CheckCommand(i))
		{
			if(PlayerCommands[1] == PI_FireUp)
			{
				PlayerCommands[1] = PI_FireUpRight; 
			}
			else if(PlayerCommands[1] == PI_FireDown)
			{
				PlayerCommands[1] = PI_FireDownRight; 
			}
			else PlayerCommands[1] = i;
		}

		if(CheckCommand(PI_FireUltimate))
			PlayerCommands[1] = PI_FireUltimate;  

		if(CheckCommand(PI_NextWeapon))
			PlayerCommands[1] = PI_NextWeapon;  

		if(CheckCommand(PI_PreviousWeapon))
			PlayerCommands[1] = PI_PreviousWeapon;

		if(CheckCommand(PI_Pause))
			PlayerConfig(ThePlayer);

	}

	void DefaultConfig()
	{

		/*PI_MovDown,PI_MovDownLeft,PI_MovLeft,PI_MovUpLeft,PI_MovUp,PI_MovUpRight,PI_MovRight,PI_MovDownRight,
		PI_FireDown,PI_FireDownLeft,PI_FireLeft,PI_FireUpLeft,PI_FireUp,PI_FireUpRight,PI_FireRight,PI_FireDownRight,
		PI_FireUltimate,PI_NextWeapon,PI_PreviousWeapon,PI_Pause*/
		JoyButtons[PI_MovDown][0] = JT_Axis;
		JoyButtons[PI_MovDown][1] = 1;
		JoyButtons[PI_MovLeft][0] = JT_Inverse;
		JoyButtons[PI_MovLeft][1] = 0;
		JoyButtons[PI_MovUp][0] = JT_Inverse;
		JoyButtons[PI_MovUp][1] = 1;
		JoyButtons[PI_MovRight][0] = JT_Axis;
		JoyButtons[PI_MovRight][1] = 0;
		JoyButtons[PI_FireDown][0] = JT_Button;
		JoyButtons[PI_FireDown][1] = 0;  
		JoyButtons[PI_FireLeft][0] = JT_Button;
		JoyButtons[PI_FireLeft][1] = 2;  
		JoyButtons[PI_FireUp][0] = JT_Button;
		JoyButtons[PI_FireUp][1] = 3;  
		JoyButtons[PI_FireRight][0] = JT_Button;
		JoyButtons[PI_FireRight][1] = 1;  
		JoyButtons[PI_FireUltimate][0] = JT_Button;
		JoyButtons[PI_FireUltimate][1] = 7;  
		JoyButtons[PI_NextWeapon][0] = JT_Button;
		JoyButtons[PI_NextWeapon][1] = 6;  
		JoyButtons[PI_PreviousWeapon][0] = JT_Button;
		JoyButtons[PI_PreviousWeapon][1] = 4;  
		JoyButtons[PI_Pause][0] = JT_Button;
		JoyButtons[PI_Pause][1] = 9;  
		KeyButtons[PI_MovDown] = SDLK_s;
		KeyButtons[PI_MovLeft] = SDLK_a;
		KeyButtons[PI_MovUp] = SDLK_w;
		KeyButtons[PI_MovRight] = SDLK_d;
		KeyButtons[PI_FireDown] = SDLK_KP2;
		KeyButtons[PI_FireDownLeft] = SDLK_KP1;
		KeyButtons[PI_FireLeft] = SDLK_KP4;
		KeyButtons[PI_FireUpLeft] = SDLK_KP7;
		KeyButtons[PI_FireUp] = SDLK_KP8;
		KeyButtons[PI_FireUpRight] = SDLK_KP9;
		KeyButtons[PI_FireRight] = SDLK_KP6;
		KeyButtons[PI_FireDownRight] = SDLK_KP3;
		KeyButtons[PI_NextWeapon] = SDLK_f;
		KeyButtons[PI_PreviousWeapon] = SDLK_g;
		KeyButtons[PI_Pause] = SDLK_ESCAPE;
		KeyButtons[PI_FireUltimate] = SDLK_KP5;
	}

	void LoadKeyConfig()
	{
		FILE *file;
		file = fopen("saves/config.cfg","r");
		if(file == NULL)
		{
			fprintf(stderr,"unable to load key configuration, falling back on default\n");
			DefaultConfig();
			SaveKeyConfig();
			return;
		}
		if(fread(KeyButtons, sizeof(KeyButtons), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
			return;
		}
		if(fread(JoyButtons, sizeof(JoyButtons), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
			return;
		}
		fclose(file);
	}


	void SaveKeyConfig()
	{
		FILE *file;
		file = fopen("saves/config.cfg","w");
		if(file == NULL)
		{
			fprintf(stderr,"unable to save key configuration, error!\n");
			return;
		}
		if(fwrite(KeyButtons, sizeof(KeyButtons), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
			return;
		}
		if(fwrite(JoyButtons, sizeof(JoyButtons), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to write config data to file: %s",SDL_GetError());
			return;
		}
		fclose(file);
	}

	void CloseHUD()
	{
		if(titlebar != NULL)
		{
			SDL_FreeSurface(titlebar);
			titlebar = NULL;
		}
	}

	void LoadHUD()
	{
		SDL_Surface *temp;
		temp = IMG_Load("images/gamesprites/titlebar1.png");
		if(temp == NULL)
		{
			fprintf(stderr,"Unable to load title bar: %s\n",SDL_GetError());
			exit(0);
		}
		titlebar = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);
		SDL_SetColorKey(titlebar, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(titlebar->format,0,0,0));
		atexit(CloseHUD);
	}

	void DrawHUD(Entity *self)
	{
		SDL_Rect window;
		char text[60];
		window.x = 0;
		window.y = 0;
		if(titlebar == NULL)
		{
			window.w = screen->w;
			window.h = 32;
			SDL_FillRect(screen, &window, IndexColor(DarkBlue));
		}
		else
		{
			SDL_BlitSurface(titlebar,NULL,screen,NULL);
		}
		sprintf(text,"Score: %i",self->Score);
		DrawText(text,screen,5,5,IndexColor(self->Color),F_Medium);
		window.x = screen->w - 120;
		window.w = 100;
		window.y = 5;
		window.h = 16;
		SDL_FillRect(screen, &window, IndexColor(Red));
		if(self->health >= 0)
			window.w = ((float)self->health / (float)self->healthmax) * 100;
		else window.w = 0;
		SDL_FillRect(screen, &window, IndexColor(Green));
	}

	void FinalOutput()
	{
		fprintf(stdout,"Final Score : %i\n",ThePlayer->Score);
		SDL_SaveBMP(screen,"finale.bmp");
	}

	/*try to open a saved game and if we fail, then */
	int GetUnusedSaveIndex()
	{
		FILE *file;
		int i = 0;
		char filename[50];
		sprintf(filename,"saves/SAVE%04i.vwg",i);
		file = fopen(filename,"r");
		while(file != NULL)
		{
			i++;
			fclose(file);    
			sprintf(filename,"saves/SAVE%04i.vwg",i);
			file = fopen(filename,"r");    
		}
		fclose(file);    
		return i;
	}

	void SavePlayer(PlayerStats *player)
	{
		FILE *file;
		char filename[50];
		if(player->previoussave != -1)sprintf(filename,"saves/SAVE%04i.vwg",GetUnusedSaveIndex());
		else sprintf(filename,"saves/SAVE%04i.vwg",player->previoussave);
		file = fopen(filename,"wb");
		if(file == NULL)
		{
			fprintf(stderr,"unable to save game!\n");
			return;
		}
		if(fwrite(player, sizeof(PlayerStats), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to write game data to file: %s",SDL_GetError());
			return;
		}
		fclose(file);
	}

	void LoadPlayer(char filename[40],PlayerStats *player)
	{
		FILE *file;
		file = fopen(filename,"rb");
		if(file == NULL)
		{
			fprintf(stderr,"unable to open saved game!\n");
			return;
		}
		if(fread(player, sizeof(PlayerStats), 1, file)!= 1)
		{
			fprintf(stderr,"Unable to read game data from file: %s",SDL_GetError());
			return;
		}
		fclose(file);
	}


	/*end of line*/
