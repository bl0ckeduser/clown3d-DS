/*
                3D model file loader
                        v1.2.5

                written by Bl0ckeduser
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

#ifndef PC_TARGET
	#define GLfloat float
#endif

#define RES 1.0f

int drawModelWithGL_big(model* model)
{
	model_group* currentGroup = model -> g;
	face* currentFace;
	int vertex_id;

	/* the scaling code hack here (and below) is due to a quirk in libnds:
	   glVertex3f only accepts values within a very limited
	   range; if this range is not respected, I get garbled
	   models. the value for the RES constant, which works on the NDS,
	   was found through guesswork... */
	glScalef(model->x_scale / RES, model->y_scale / RES, model->z_scale / RES);

	while(currentGroup != NULL) {
		currentFace = currentGroup -> faces;

		/* group color */
		glColor3f(currentGroup -> color[0], currentGroup -> color[1], currentGroup -> color[2]);

		/* draw the faces */
		while(currentFace->next != NULL) {

			/* one of the quirks of libnds is that there 
			   is no GL_POLYGON mode ! */
			if(currentFace->point_count==3){
				glBegin(GL_TRIANGLES);
			} else {
			 	glBegin(GL_QUADS);
			}

			vertex_id = 0;
			while(currentFace -> vertnum[vertex_id] != -1) {
				glVertex3f(
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].x / model->x_scale * RES,
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].y / model->y_scale * RES,
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].z / model->z_scale * RES);

				vc++;
				vertex_id++;
			}

			glEnd();
			currentFace = currentFace -> next;
		}

		currentGroup = currentGroup -> next;
	}

	return vc;
}

int drawModelWithGL(model* model)
{
	model_group* currentGroup = model -> g;
	face* currentFace;
	int vertex_id;

	while(currentGroup != NULL) {
		currentFace = currentGroup -> faces;

		/* group color */
		glColor3f(currentGroup -> color[0], currentGroup -> color[1], currentGroup -> color[2]);

		/* draw the faces */
		while(currentFace->next != NULL) {

			/* one of the quirks of libnds is that there 
			   is no GL_POLYGON mode ! */
			if(currentFace->point_count==3){
				glBegin(GL_TRIANGLES);
			} else {
			 	glBegin(GL_QUADS);
			}

			vertex_id = 0;
			while(currentFace -> vertnum[vertex_id] != -1) {
				glVertex3f(
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].x,
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].y,
				    model -> vertices[currentFace -> vertnum[vertex_id]-1].z);

				vc++;
				vertex_id++;
			}

			glEnd();
			currentFace = currentFace -> next;
		}

		currentGroup = currentGroup -> next;
	}

	return vc;
}

