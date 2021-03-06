#include "headers.h"
#include <math.h>

char jump_clean = 0;

game_obj* newPlayer(game_obj* list, float x, float y, float z)
{
	game_obj* play = newListNode(list);

	play->type = PLAYER;
	play->data = (float *)malloc(26 * sizeof(float));
	play->data[PLAYER_X] = x;
	play->data[PLAYER_Y] = y;
	play->data[PLAYER_Z] = z;
	playerInit(play);

	return play;
}

void playerInit(game_obj* a)
{
	a->data[PLAYER_YVEL] = 0.0;
	a->data[PLAYER_ANGLE] = 180.0;
	a->data[PLAYER_MOVEX] = 0.0;
	a->data[PLAYER_MOVEY] = 20.0;
	a->data[PLAYER_MOVEZ] = 0.0;
	a->data[PLAYER_DIRX] = 0.0;
	a->data[PLAYER_DIRZ] = 0.0;
	a->data[PLAYER_KEYS] = 0;
	a->data[PLAYER_BULLET_TIMER] = 0.0f;
	a->ee_targ = NULL;
	a->ee_bits = 0;
}

void playerTick(game_obj* player)
{
	/* platform independent controller bits */
	int up, down, left, right, shoot, jump;

	player->data[PLAYER_MOVEX] = 0.0;
	player->data[PLAYER_MOVEY] = 0.0;
	player->data[PLAYER_MOVEZ] = 0.0;

	if(player->data[PLAYER_YVEL] < -MAX_YVEL)
		player->data[PLAYER_YVEL] = -MAX_YVEL;

	/* dX = Vi * dT + 1/2(A * (dT)^2) */
	player->data[PLAYER_MOVEY] = 
		player->data[PLAYER_YVEL] * 
			dtime + (ACCEL * dtime * dtime) / 2.0;

	/* Vf = Vi + A * dT */
	player->data[PLAYER_YVEL] += ACCEL * dtime;

	/* This makes the game look & play better */
	if(player->data[PLAYER_ON_PLATFORM]) {
		if(player->data[PLAYER_YVEL] < 0) {
			player->data[PLAYER_YVEL] = 0;
		}
		player->data[PLAYER_MOVEY] = 0;
	}

	/* Convert the player angle to a unit vector */
#ifdef PC_TARGET
	player->data[PLAYER_DIRX] = 
		(float)sin(player->data[PLAYER_ANGLE] / 360.0 * (2*3.14));
	player->data[PLAYER_DIRZ] = 
		(float)cos(player->data[PLAYER_ANGLE] / 360.0 * (2*3.14));
#else
	player->data[PLAYER_DIRX] = my_sin(player->data[PLAYER_ANGLE]);
	player->data[PLAYER_DIRZ] = my_cos(player->data[PLAYER_ANGLE]);
#endif

	/* Make sure everything is within expected range */
	if(player->data[PLAYER_ANGLE] < 0)
		player->data[PLAYER_ANGLE] = 360;
	if(player->data[PLAYER_ANGLE] > 360)
		player->data[PLAYER_ANGLE] = 0;

	/* Limit the frequency at which the player
	   can shoot bullets */
	if(player->data[PLAYER_BULLET_TIMER] >= 0.0f)
		player->data[PLAYER_BULLET_TIMER] -= 15 * dtime;

#ifdef PC_TARGET
	shoot = keystate[SDLK_s];
	jump = keystate[SDLK_z];
	left = keystate[SDLK_LEFT];
	right = keystate[SDLK_RIGHT];
	down = keystate[SDLK_DOWN];
	up = keystate[SDLK_UP];
#else
	/* NDS key handling code below copied and adapted
	   from an example by Dovoto */
	scanKeys();
	u16 keys = keysHeld();
	jump = (keys & KEY_A);
	shoot = (keys & KEY_B);
	left = (keys & KEY_LEFT);
	right = (keys & KEY_RIGHT);
	down = (keys & KEY_DOWN);
	up = (keys & KEY_UP);
#endif

	/*
	 * Player rotation and
	 * walking are linear with
	 * respect to time, so
	 * we just multiply by
	 * delta time to
	 * time-correct.
	 */

	if(shoot && player->data[PLAYER_BULLET_TIMER] <= 0.1f)
	{
		player->data[PLAYER_BULLET_TIMER] = 25.0;
		newBullet(player, player->data[PLAYER_X] + player->data[PLAYER_DIRX] * 15,
			player->data[PLAYER_Y],
			player->data[PLAYER_Z] + player->data[PLAYER_DIRZ] * 15,
			player->data[PLAYER_ANGLE]);
	}

	if(left)
		player->data[PLAYER_ANGLE] += 9.0 * dtime;

	if(right)
		player->data[PLAYER_ANGLE] -= 9.0 * dtime;

	if(up) {
		player->data[PLAYER_MOVEX] = 7.5 * 
			player->data[PLAYER_DIRX] * dtime;
		player->data[PLAYER_MOVEZ] = 7.5 * 
			player->data[PLAYER_DIRZ] * dtime;
	}

	if(down) {
		player->data[PLAYER_MOVEX] = -7.5 * 
			player->data[PLAYER_DIRX] * dtime;
		player->data[PLAYER_MOVEZ] = -7.5 * 
			player->data[PLAYER_DIRZ] * dtime;
	}

	if(jump) {
		if(jump_clean && player->data[PLAYER_ON_PLATFORM]) {
				player->data[PLAYER_YVEL] = MAX_YVEL;
				jump_clean = 0;
		}
	} else jump_clean = 1;

	player->data[PLAYER_ON_PLATFORM]  = 0.0;

	player->data[PLAYER_X] += player->data[PLAYER_MOVEX];
	player->data[PLAYER_Y] += player->data[PLAYER_MOVEY];
	player->data[PLAYER_Z] += player->data[PLAYER_MOVEZ];

	/* Player collision box and movement vector */
	player->box.min.x = (float)(player->data[PLAYER_X] - 10);
	player->box.min.y = (float)(player->data[PLAYER_Y] - 10);
	player->box.min.z = (float)(player->data[PLAYER_Z] - 10);
	player->box.max.x = (float)(player->data[PLAYER_X] + 10);
	player->box.max.y = (float)(player->data[PLAYER_Y] + 10);
	player->box.max.z = (float)(player->data[PLAYER_Z] + 10);
	player->box.move.x = (float)player->data[PLAYER_MOVEX];
	player->box.move.y = (float)player->data[PLAYER_MOVEY];
	player->box.move.z = (float)player->data[PLAYER_MOVEZ];

}

void playerCollide(game_obj* a, game_obj* b)
{
	/* If the player hit something solid,
	   update its coordinates and "physics state"
	   in consequence */
	if(b->type == SOLID || b->type==DOOR) {

		/* If we hit a solid object when moving down 
		   and were moved up, we're on a platform */
		if(a->box.move.y != 0 
		 && (a->box.min.y + 10 - a->data[PLAYER_Y] > 0.0))
			a->data[PLAYER_ON_PLATFORM] = 1;
	
		/* Update the player's coordinates to the
		   collision-resolved ones. 
		   Note that we only accept resolved 
		   collision data for the Y axis if 
		   it results from movement on the 
		   Y axis */

		if(a->box.move.y == 0)
			a->data[PLAYER_X] = a->box.min.x + 10;
		if(a->box.move.y != 0)
			a->data[PLAYER_Y] = a->box.min.y + 10;
		if(a->box.move.y == 0)
			a->data[PLAYER_Z] = a->box.min.z + 10;

	} else if(b->type == KEY && b->data[KEY_EXISTS]) {
		/* Pick up a key */
		b->data[KEY_EXISTS] = 0;
		a->data[PLAYER_KEYS]++;
	} 

	/* If the player hits a door and has a key,
	   open the door and get rid of the key */
	if(b->type == DOOR && b->data[DOOR_CLOSED])
	{
		if(a->data[PLAYER_KEYS] > 0)
		{
			--a->data[PLAYER_KEYS];
			b->data[DOOR_CLOSED] = 0;
		}
	}
}

void playerDraw(game_obj* player)
{
        if(cam_angle != 2) {
                /* Draw the player model */
                glTranslatef((GLfloat)player->data[PLAYER_X]/10.0f, (GLfloat)player->data[PLAYER_Y]/10.0f,
                             (GLfloat)player->data[PLAYER_Z]/10.0f);
                glScalef(2, 2, 2);              /* older rescale model */
                glRotatef(player->data[PLAYER_ANGLE], 0.0f, 1.0f, 0.0f);
                drawModelWithGL(turtleModel);
        }
}

void playerEdgeEvade(game_obj* player)
{
	if(player->ee_targ && player->ee_bits != 6) {
		switch(player->ee_dir) {		
			case 0: player->data[PLAYER_X] += player->data[PLAYER_MOVEX]; break;
			case 1: player->data[PLAYER_Z] += player->data[PLAYER_MOVEZ]; break;
		}
	}
}
