/*
 * Pseudo-OOP
 */

#include "headers.h"

void initFunction(void *va)
{
	game_obj* a = va;

	switch(a->type) {
	case PLAYER:
		playerInit(a);
		break;
	case KEY:
		keyInit(a);
		break;
	case DOOR:
		doorInit(a);
		break;
	case TARGET:
		targetInit(a);
		break;
	}
}

void tickFunction(void *va)
{
	game_obj* a = (game_obj *)va;

	if(a==NULL)	return;

	switch(a->type) {
	case PLAYER:
		playerTick(a);
		break;
	case KEY:
		keyTick(a);
		break;
	case DOOR:
		doorTick(a);
		break;
	case BULLET:
		bulletTick(a);
		break;
	case TARGET:
		targetTick(a);
		break;
	}

}

void drawFunction(void* va)
{
	game_obj* a = va;

	if(a==NULL)	return;

	switch(a->type) {
	case KEY:
		keyDraw(a);
		break;
	case PLAYER:
		playerDraw(a);
		break;
	case DOOR:
		doorDraw(a);
		break;
	case BULLET:
		bulletDraw(a);
		break;
	case TARGET:
		targetDraw(a);
		break;
	}
}

void collisionFunction(void* va, void* vb)
{
	game_obj* a = va;
	game_obj* b = vb;

	if(a == NULL || b==NULL) return;

	switch(a->type) {
	case PLAYER:
		playerCollide(a, b);
		break;
	case BULLET:
		bulletCollide(a, b);
		break;
	case TARGET:
		targetCollide(a, b);
		break;
	}
}

