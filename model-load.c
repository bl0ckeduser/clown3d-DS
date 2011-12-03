/*
                3D model file loader
                        v1.2.5

                written by Bl0ckeduser
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>	/* fabs */
#include "headers.h"

/* On the DS, loadModel() is slow.
   Must be because we only have 30 MHz 
   of CPU power */
model* loadModel(char* data)
{
	model* ModelPtr;

	/* syntax sugar */
	model_group* currentGroup;
	face* currentFace;

	/* temporary variables */
	char* buffer;
	int r, g, b;
	int vert_count;
	float x, y, z;
	int vertex_id;

	/* reset this everytime a modelfile is read */
	int first_group = 1;

	/***********************************************************************/

	/* allocate model strucutre, group structure (linked list) */
	ModelPtr = (model *)malloc(sizeof(model));
	if(ModelPtr == NULL) {
		printf("loadModel: could not create model structure\n");
		return NULL;
	}
	ModelPtr -> g = (model_group *)malloc(sizeof(model_group));
	ModelPtr -> x_scale = ModelPtr -> y_scale = ModelPtr -> z_scale = 0.0f;

	currentGroup = ModelPtr -> g;		/* syntax sugar */
	if(currentGroup == NULL) {
		printf("loadModel: could not create model groups\n");
		return NULL;
	}
	currentGroup->next = NULL;

	/* allocate vertices structure (array) */
	ModelPtr -> vertices = (vert *)malloc(VERTEX_ARRAY_INCREMENT*sizeof(vert));
	ModelPtr -> allocated_vert = VERTEX_ARRAY_INCREMENT;
	if(ModelPtr -> vertices == NULL) {
		printf("loadModel: could not create model vertices storage\n");
		return NULL;
	}
	vert_count = 0;

	/* allocate faces structure (linked list) */
	currentGroup -> faces = (face *)malloc(sizeof(face));
	if(currentGroup -> faces == NULL) {
		printf("loadModel: could not create model faces storage\n");
		return NULL;
	}
	currentGroup -> faces -> next = NULL;

	/***********************************************************************/

	/* read model file to model and children strucutres */

	while(1) {
		buffer = read_str(&data);
hack:
		if(!*data)	break;

		if(strcmp(buffer, "newgroup")==0) {
			if(first_group) {
				first_group = 0;
			} else {
				currentGroup -> next = (model_group *)malloc(sizeof(model_group));
				currentGroup = currentGroup -> next;

				if(currentGroup == NULL) {
					printf("loadModel: could not allocate new model group\n");
					return NULL;
				}

				currentGroup -> faces = (face *)malloc(sizeof(face));
				currentGroup -> next = NULL;

				if(currentGroup -> faces == NULL) {
					printf("loadModel: could not allocate group faces\n");
					return NULL;
				}
			}


			/* the default color is white -- imitation of C4D */
			currentGroup -> color[0] = 1.0f;
			currentGroup -> color[1] = 1.0f;
			currentGroup -> color[2] = 1.0f;

			currentFace = currentGroup -> faces;
		}

		if(strcmp(buffer, "color")==0) {
			r = read_int(&data);
			g = read_int(&data);
			b = read_int(&data);
			currentGroup -> color[0] = (float)r / 255.0f;
			currentGroup -> color[1] = (float)g / 255.0f;
			currentGroup -> color[2] = (float)b / 255.0f;
		}

		if(strcmp(buffer, "vertex")==0) {
			x = read_float(&data);
			y = read_float(&data);
			z = read_float(&data);

			ModelPtr -> vertices[vert_count].x = x;
			ModelPtr -> vertices[vert_count].y = y;
			ModelPtr -> vertices[vert_count].z = z;
			ModelPtr -> vertices[vert_count].exists = 1;

			if(fabs(x) > ModelPtr -> x_scale)	ModelPtr -> x_scale = x;
			if(fabs(y) > ModelPtr -> y_scale)	ModelPtr -> y_scale = y;
			if(fabs(z) > ModelPtr -> z_scale)	ModelPtr -> z_scale = z;

			if(++vert_count >= ModelPtr -> allocated_vert) {
				ModelPtr->allocated_vert += VERTEX_ARRAY_INCREMENT;
				ModelPtr->vertices = (vert *)realloc(
				                         ModelPtr->vertices,
				                         ModelPtr->allocated_vert * sizeof(vert));

				if(ModelPtr->vertices==NULL) {
					printf("loadModel: could not resize vertices array\n");
					return NULL;
				}
			}

			ModelPtr -> vertices[vert_count].exists = 0;
		}

		if(strcmp(buffer, "face")==0) {
			if(currentFace == NULL) {
				printf("loadModel: face allocation failed\n");
				return NULL;
			}

			currentFace -> vertnum = (int *)malloc(8 * sizeof(int));
			vertex_id = 0;

			if(currentFace -> vertnum == NULL) {
				printf("loadModel: could not allocate face vertex array\n");
				return NULL;
			}

			while(1) {
				buffer = read_str(&data);

				/* end of this face */
				if(!*data || !strcmp(buffer, "face") || !strcmp(buffer, "newgroup")) {
					currentFace -> vertnum[vertex_id++] = -1;  /* null vertex number */
					currentFace -> next = (face *)malloc(sizeof(face));

					currentFace = currentFace -> next;
					currentFace -> next = NULL;
					currentFace -> point_count = 0;

					goto hack;
				}

				currentFace -> vertnum[vertex_id++] = (int)atoi(buffer);
				currentFace -> point_count++;
			}
		}
	}

	/***********************************************************************/

	return ModelPtr;
}

void listModel(model* model)			/* not tested in a while. maybe broken */
{
	model_group* currentGroup = model -> g;
	face* currentFace;
	int vertex_id;

	while(currentGroup != NULL) {
		printf("\n new group \n");

		currentFace = currentGroup -> faces;

		/* group color */
		printf("color: %f %f %f\n",
		       currentGroup -> color[0],
		       currentGroup -> color[1],
		       currentGroup -> color[2]);

		/* vertices */
		vertex_id = 0;
		while(model -> vertices[vertex_id].exists) {
			printf("vertex: %f %f %f\n",
			       model -> vertices[vertex_id].x,
			       model -> vertices[vertex_id].y,
			       model -> vertices[vertex_id].z);
			++vertex_id;
		}


		/* faces */
		while(currentFace->next != NULL) {
			printf("face: ");

			vertex_id = 0;

			while(currentFace -> vertnum[vertex_id] != -1) {
				printf(" %d", currentFace -> vertnum[vertex_id]);
				printf(" (%f %f %f)\n",
				       model -> vertices[currentFace -> vertnum[vertex_id]-1].x,
				       model -> vertices[currentFace -> vertnum[vertex_id]-1].y,
				       model -> vertices[currentFace -> vertnum[vertex_id]-1].z);

				vertex_id++;
			}

			printf("\n");
			currentFace = currentFace -> next;
		}

		currentGroup = currentGroup -> next;
	}
}

void freeModel(model* model)
{
	model_group* currentGroup = model -> g;
	face* currentFace;
	void* temp;

	while(currentGroup != NULL) {
		/* free the faces */
		currentFace = currentGroup -> faces;
		while(currentFace != NULL) {
			if(currentFace -> next != NULL)
				free(currentFace -> vertnum);
			temp = currentFace;
			currentFace = currentFace -> next;
			free(temp);
		}

		/* next group */
		temp = currentGroup;
		currentGroup = currentGroup -> next;
		free(temp);
	}

	/* free the model vertices */
	free(model -> vertices);

	/* free the model structure */
	free(model);
}

