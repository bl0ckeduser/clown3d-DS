#include "headers.h"
#include <stdlib.h>

/*
 * Generate solid bounding boxes for each
 * of the groups in a model
 */

void genBoxes(model* theModel, game_obj* obj_list)
{
	game_obj* curr = obj_list;
	game_obj* prev;
	float minx, miny, minz, maxx, maxy, maxz;
	float vx, vy, vz;
	model_group* currentGroup;
	face* currentFace;
	int vertex_id;

	while(curr->next != NULL)
		curr = curr->next;

	currentGroup = theModel -> g;

	while(currentGroup != NULL) {
		curr->next = (game_obj *)malloc(sizeof(game_obj));
		prev = curr;
		curr = curr->next;
		curr->prev = prev;
		curr->next = NULL;

		currentFace = currentGroup -> faces;

		/* hack, beware */
		minx = miny = minz = 10000.0f;
		maxx = maxy = maxz = -10000.0f;

		while(currentFace->next != NULL) {
			vertex_id = 0;
			while(currentFace -> vertnum[vertex_id] != -1) {
				vx = theModel -> vertices[currentFace -> vertnum[vertex_id]-1].x;
				vy = theModel -> vertices[currentFace -> vertnum[vertex_id]-1].y;
				vz = theModel -> vertices[currentFace -> vertnum[vertex_id]-1].z;

				if(vx < minx)	minx = vx;
				if(vy < miny)	miny = vy;
				if(vz < minz)	minz = vz;

				if(vx > maxx)	maxx = vx;
				if(vy > maxy)	maxy = vy;
				if(vz > maxz)	maxz = vz;

				vertex_id++;
			}
			currentFace = currentFace -> next;
		}

		/* create bounding box for this model group */

		curr->data = NULL;
		curr->type = SOLID;
		curr->box.min.x = minx;
		curr->box.min.y = miny;
		curr->box.min.z = minz;
		curr->box.max.x = maxx;
		curr->box.max.y = maxy;
		curr->box.max.z = maxz;
		curr->box.move.x = 0.0f;
		curr->box.move.y = 0.0f;
		curr->box.move.z = 0.0f;

		currentGroup = currentGroup -> next;
	}

}
