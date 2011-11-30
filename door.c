#include "headers.h"

game_obj* newDoor(game_obj* list, float x, float y, float z)
{
	game_obj* door = newListNode(list);

	door->type = DOOR;
	door->data = (float *)malloc(sizeof(float));
	door->coords.x = x;
	door->coords.y = y;
	door->coords.z = z;
	doorInit(door);

	return door;
}

void doorInit(game_obj* a)
{
	a->data[DOOR_CLOSED]  = 1;
}

void doorTick(game_obj* a)
{
	if(a->data[DOOR_CLOSED]) {
		a->box.min.x = a->coords.x - 20;	/* write bounding box */
		a->box.min.y = a->coords.y - 20;
		a->box.min.z = a->coords.z - 20;
		a->box.max.x = a->coords.x + 20;
		a->box.max.y = a->coords.y + 20;
		a->box.max.z = a->coords.z + 20;
	} else {
		a->box.min.x = 0;	/* empty bounding box */
		a->box.min.y = 0;
		a->box.min.z = 0;
		a->box.max.x = 0;
		a->box.max.y = 0;
		a->box.max.z = 0;
	}
}

void doorDraw(game_obj* a)
{
        if(a->data[DOOR_CLOSED]) {
                glTranslatef(a->coords.x / 10.0,
                             a->coords.y / 10.0, a->coords.z / 10.0);
	        drawModelWithGL(doorModel);
        }
}

