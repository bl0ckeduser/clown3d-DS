#include "headers.h"
#include <math.h>

game_obj* newTarget(game_obj* list, float x, float y, float z, float angle)
{
	game_obj* targ = newListNode(list);

	targ->type = TARGET;
	targ->data = (float *)malloc(5 * sizeof(float));
	targ->data[TARGET_X] = x;
	targ->data[TARGET_Y] = y;
	targ->data[TARGET_Z] = z;
	targ->data[TARGET_ANGLE] = angle;
	targ->data[TARGET_EXPLODED] = 0;

	return targ;
}

void targetInit(game_obj* targ)
{
	targ->data[TARGET_EXPLODED] = 0;
}

void targetTick(game_obj* targ)
{
        /* target collision box and movement vector */
        targ->box.min.x = (float)(targ->data[TARGET_X] - 10);
        targ->box.min.y = (float)(targ->data[TARGET_Y] - 10);
        targ->box.min.z = (float)(targ->data[TARGET_Z] - 10);
        targ->box.max.x = (float)(targ->data[TARGET_X] + 10);
        targ->box.max.y = (float)(targ->data[TARGET_Y] + 10);
        targ->box.max.z = (float)(targ->data[TARGET_Z] + 10);
        targ->box.move.x = (float)0;
        targ->box.move.y = (float)0;
        targ->box.move.z = (float)0;
}

void targetCollide(game_obj* a, game_obj* b)
{
	if(a->type==TARGET && b->type==BULLET)
		if(!a->data[TARGET_EXPLODED])
			a->data[TARGET_EXPLODED] = 1;

}

void targetDraw(game_obj* targ)
{
	if(targ->data[TARGET_EXPLODED])	return;
                glLoadIdentity();
                glTranslatef((GLfloat)targ->data[TARGET_X]/10.0f, (GLfloat)targ->data[TARGET_Y]/10.0f,
                             (GLfloat)targ->data[TARGET_Z]/10.0f);
		glScalef(0.1f, 0.1f, 0.1f);
                glRotatef(targ->data[TARGET_ANGLE], 0.0f, 1.0f, 0.0f);
                drawModelWithGL(targetModel);
}
