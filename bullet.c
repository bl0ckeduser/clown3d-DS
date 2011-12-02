#include "headers.h"
#include <math.h>

game_obj* newBullet(game_obj* list, float x, float y, float z, float angle)
{
	game_obj* bul = newListNode(list);

	bul->type = BULLET;
	bul->data = (float *)malloc(9 * sizeof(float));
	bul->data[BULLET_X] = x;
	bul->data[BULLET_Y] = y;
	bul->data[BULLET_Z] = z;
	bul->data[BULLET_ANGLE] = angle;
	bul->data[BULLET_YVEL] = 0;
	bul->data[BULLET_TIMER] = 45;
	bul->data[BULLET_EXPLODED] = 0;

#ifdef PC_TARGET
	bul->data[BULLET_XDIR] = (float)sin(bul->data[BULLET_ANGLE] / 360.0 * (2*3.14));
	bul->data[BULLET_ZDIR] = (float)cos(bul->data[BULLET_ANGLE] / 360.0 * (2*3.14));
#else
	bul->data[BULLET_XDIR] = my_sin(bul->data[BULLET_ANGLE]);
	bul->data[BULLET_ZDIR] = my_cos(bul->data[BULLET_ANGLE]);
#endif

	/* collision system movement vector */
	bul->box.move.x = (float)10*bul->data[BULLET_XDIR];
	bul->box.move.y = (float)0;
	bul->box.move.z = (float)10*bul->data[BULLET_ZDIR];

	return bul;
}

void bulletTick(game_obj* bul)
{
	if(bul->data[BULLET_EXPLODED]==1)
	{
		++bul->data[BULLET_EXPLODED];
		deleteNode(bul);
	}

	bul->data[BULLET_X] += 20 * bul->data[BULLET_XDIR] * dtime;
	bul->data[BULLET_Z] += 20 * bul->data[BULLET_ZDIR] * dtime;

	/* bullet collision box */
	bul->box.min.x = (float)(bul->data[BULLET_X] - 5);
	bul->box.min.y = (float)(bul->data[BULLET_Y] - 5);
	bul->box.min.z = (float)(bul->data[BULLET_Z] - 5);
	bul->box.max.x = (float)(bul->data[BULLET_X] + 5);
	bul->box.max.y = (float)(bul->data[BULLET_Y] + 5);
	bul->box.max.z = (float)(bul->data[BULLET_Z] + 5);

	if((bul->data[BULLET_TIMER] -= 3*dtime) < 0.0)
		if(!bul->data[BULLET_EXPLODED])
			bul->data[BULLET_EXPLODED] = 1;
}

void bulletCollide(game_obj* a, game_obj* b)
{
	if(a->type==BULLET || (b->type==TARGET && !b->data[TARGET_EXPLODED]))
		if(!a->data[BULLET_EXPLODED])
			a->data[BULLET_EXPLODED] = 1;

}

void bulletDraw(game_obj* bul)
{
                glTranslatef((GLfloat)bul->data[BULLET_X]/10.0f, (GLfloat)bul->data[BULLET_Y]/10.0f,
                             (GLfloat)bul->data[BULLET_Z]/10.0f);
		glScalef(0.1f, 0.1f, 0.1f);
                glRotatef(bul->data[BULLET_ANGLE], 0.0f, 1.0f, 0.0f);
                drawModelWithGL(bulletModel);
}
