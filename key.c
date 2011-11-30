#include "headers.h"

/*
 * Can be picked up by
 * player. Rotates when not picked
 * up.
 */

game_obj* newKey(game_obj* list, float x, float y, float z)
{
	game_obj* key = newListNode(list);

	key->type = KEY;
	key->data = (float *)malloc(sizeof(float));
	key->coords.x = x;
	key->coords.y = y;
	key->coords.z = z;
	keyInit(key);

	return key;
}

void keyInit(game_obj* a)
{
	a->rot = 0;
	a->data[KEY_EXISTS]  = 1;
}

void keyTick(game_obj* a)
{
	if(a->data[KEY_EXISTS]) {
		a->box.min.x = a->coords.x - 10;	/* write bounding box */
		a->box.min.y = a->coords.y - 10;
		a->box.min.z = a->coords.z - 10;
		a->box.max.x = a->coords.x + 10;
		a->box.max.y = a->coords.y + 10;
		a->box.max.z = a->coords.z + 10;

		a->rot += 10 * dtime;			/* rotate */

		if(a->rot>360)
			a->rot = 0;
	}
}

void keyDraw(game_obj* a)
{
	if(a->data[KEY_EXISTS]) {
		glTranslatef(a->coords.x / 10.0,
		             a->coords.y / 10.0, a->coords.z / 10.0);
		glRotatef(a->rot, 0.0f, 1.0f, 0.0f);
		glScalef(0.1f, 0.1f, 0.1f);
		drawModelWithGL(keyModel);
	}
}
