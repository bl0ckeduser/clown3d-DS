/*
 * The "Clown" 3D Game Engine
 * Designed, debugged and perfected
 *  by Bl0ckeduser, 2011
 *
 * (i.e. on paper it wasn't this
 *  complicated and full of workarounds)
 */

/*
 * Homegrown AABB collision detector and
 * resolver
 */

#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

/* Compares using absolute values, equivalent
   to measuring the magnitude of a 1-dimensional
   vector. Ignores zero-length values as
   result candidates. */
float smallestOfThree(float a, float b, float c)
{
	float aa, ab, ac;

	/* Absolute value */
	aa = a > 0.0 ? a : -a;
	ab = b > 0.0 ? b : -b;
	ac = c > 0.0 ? c : -c;

	/* Ignore zero length */
	aa = aa == 0.0 ? ab : aa;
	ab = ab == 0.0 ? aa : ab;
	ac = ac == 0.0 ? aa : ac;

	if(ab >= aa && ac >= aa)	return a;
	if(aa >= ab && ac >= ab)	return b;
	if(aa >= ac && ab >= ac)	return c;

	return 0.0;
}

/* this macro eliminates redundancies in resolveCollisions. */

#define resolveCollisions_checkVertex(XTYPE, YTYPE, ZTYPE, jump)				\
		if(node->box.XTYPE.x > node2->box.min.x &&					\
		   node->box.YTYPE.y > node2->box.min.y &&					\
		   node->box.ZTYPE.z > node2->box.min.z &&					\
		   node->box.XTYPE.x < node2->box.max.x &&					\
		   node->box.YTYPE.y < node2->box.max.y &&					\
		   node->box.ZTYPE.z < node2->box.max.z)					\
		{										\
			/* x_offs, y_offs, z_offs:						\
			   difference between this vertex's coords and				\
			   where it should be to resolve the 					\
			   collision. Destination vertices					\
			   are chosen based on movement						\
			   vector */								\
												\
			if(node->box.move.x < 0)						\
				x_offs = node2->box.max.x -					\
					node->box.XTYPE.x;	/* vertex x */			\
			else									\
				x_offs = node2->box.min.x - 					\
					node->box.XTYPE.x;	/* vertex x */			\
												\
			if(node->box.move.y < 0)						\
				y_offs = node2->box.max.y -					\
					node->box.YTYPE.y;	/* vertex y */			\
			else									\
				y_offs = node2->box.min.y - 	/* vertex y */			\
					node->box.YTYPE.y;					\
												\
			if(node->box.move.z < 0)						\
				z_offs = node2->box.max.z -					\
					node->box.ZTYPE.z;	/* vertex z */			\
			else									\
				z_offs = node2->box.min.z - 	/* vertex z */			\
					node->box.ZTYPE.z;					\
												\
												\
			/* The "reaction" vector translates the current object in reaction	\
			   to the collision, resolving the collision.				\
												\
			   It is equal to the vector amongst the following that has		\
			   the smallest magnitude ("move" is the current object's movement	\
			   vector):								\
												\
			   x_offs / move.x * move						\
			   y_offs / move.y * move						\
			   z_offs / move.z * move						\
												\
			   Of course it is only necessary to check which of			\
			   [x|y|z]_offs/move.[x|y|z] is smallest.				\
			 */									\
												\
			ratio = smallestOfThree							\
				(x_offs / node->box.move.x,					\
				y_offs / node->box.move.y,					\
				z_offs / node->box.move.z);					\
												\
			/* Cheap way to avoid glitches. Normally,				\
			   ratio should be between 0 and 1. */					\
			if(ratio < -2.0)  ratio = 0.0;						\
												\
			/* Cheap fix for another glitch. Avoid					\
			   inifinite collision-solving loops (player				\
			   gets stuck) */							\
			ratio *= 1.01;								\
												\
			react.x = node->box.move.x * ratio;					\
			react.y = node->box.move.y * ratio;					\
			react.z = node->box.move.z * ratio;					\
												\
			goto jump;								\
		}										\

#define resolveCollisions_checkVertex_stair(XTYPE, YTYPE, ZTYPE, jump)				\
		if(node->box.XTYPE.x > node2->box.min.x &&					\
		   node->box.YTYPE.y > node2->box.min.y &&					\
		   node->box.ZTYPE.z > node2->box.min.z &&					\
		   node->box.XTYPE.x < node2->box.max.x &&					\
		   node->box.YTYPE.y < node2->box.max.y &&					\
		   node->box.ZTYPE.z < node2->box.max.z)					\
		{										\
			y_offs = node2->box.max.y - node->box.YTYPE.y >				\
				node2->box.min.y - node->box.YTYPE.y ?				\
				node2->box.max.y - node->box.YTYPE.y :				\
				node2->box.min.y - node->box.YTYPE.y;				\
												\
			react.y = y_offs;							\
												\
			goto jump;								\
		}										\


void resolveCollisions(game_obj* objs, void (*handler)(void*, void*))
{
	/* collision resolver computation variables */
	vector move, react;
	float x_offs, y_offs, z_offs;
	float ratio;

	/*
	 * Objects marked as "stairs" have normal 
	 * Y-axis (vertical) collisions,
	 * but move objects strictly on the Y axis
	 * when X, Z collisions occur. The desired
	 * effect is that the player can "walk up
	 * stairs".
	 *
	 * Currently, there is no system for flagging
	 * specific objects as stairs; setting stair = 1
	 * below will make the collision code treat
	 * everything as a stair.
	 */
	int stair = 0;

	game_obj *node, *node2;

	for(node = objs; node != NULL; node = node->next) {

		if(node->type == SOLID)  continue;	/* heuristic: SOLIDs don't move ! */

		for(node2 = objs; node2 != NULL; node2 = node2->next) {
			if(node != node2 && node->type != NONE && node2->type != NONE) {
				/* Check if any of the current boxe's
				   vertices are within any other box.
				   The collision engine stores only
				   two vertices: minimum and maximum.
				   The other six vertice's coordinates are
				   combinations of these two vertice's
				   x,y,z coordinates. */

				/* Initialize reaction vector */
				react.x = react.y = react.z = 0.0;

				move.x = node->box.move.x;
				move.y = node->box.move.y;
				move.z = node->box.move.z;

				/*
				 * We first resolve collisions
				 * resulting from movement
				 * on the Y axis, setting move.x = move.z = 0
				 * to ensure the reaction vector is strictly Y-axis
				 */
				node->box.min.x -= move.x;	/* cancel out x, z movement */
				node->box.max.x -= move.x;
				node->box.min.z -= move.z;
				node->box.max.z -= move.z;
				node->box.move.x = 0.0;
				node->box.move.z = 0.0;

				resolveCollisions_checkVertex(min, min, min, collision_found);
				resolveCollisions_checkVertex(min, min, max, collision_found);
				resolveCollisions_checkVertex(min, max, min, collision_found);
				resolveCollisions_checkVertex(min, max, max, collision_found);
				resolveCollisions_checkVertex(max, min, min, collision_found);
				resolveCollisions_checkVertex(max, min, max, collision_found);
				resolveCollisions_checkVertex(max, max, min, collision_found);
				resolveCollisions_checkVertex(max, max, max, collision_found);

collision_found:
				node->box.min.y += react.y;
				node->box.max.y += react.y;

				if(react.x || react.y || react.z) {
					(*handler)(node, node2);
					(*handler)(node2, node);
				}

				/*
				 * Now we solve collisions resulting from
				 * movement on the X and Z axes. Notice that
				 * we keep the collision-resolved Y coordinate
				 * we just obtained and set move.y = 0, so that the 
				 * reaction vector is strictly in the X and Z axes.
				 */

				react.x = react.y = react.z = 0.0;

				/* restore x,z displacement we cancelled out
				   earlier on */
				node->box.min.x += move.x;
				node->box.max.x += move.x;
				node->box.min.z += move.z;
				node->box.max.z += move.z;
				node->box.move.x = move.x;
				node->box.move.z = move.z;

				node->box.move.y = 0.0;

				if(stair){
					/* stairs mode */
					resolveCollisions_checkVertex_stair(min, min, min, collision_found2);
					resolveCollisions_checkVertex_stair(min, min, max, collision_found2);
					resolveCollisions_checkVertex_stair(min, max, min, collision_found2);
					resolveCollisions_checkVertex_stair(min, max, max, collision_found2);
					resolveCollisions_checkVertex_stair(max, min, min, collision_found2);
					resolveCollisions_checkVertex_stair(max, min, max, collision_found2);
					resolveCollisions_checkVertex_stair(max, max, min, collision_found2);
					resolveCollisions_checkVertex_stair(max, max, max, collision_found2);
				} else {
					resolveCollisions_checkVertex(min, min, min, collision_found2);
					resolveCollisions_checkVertex(min, min, max, collision_found2);
					resolveCollisions_checkVertex(min, max, min, collision_found2);
					resolveCollisions_checkVertex(min, max, max, collision_found2);
					resolveCollisions_checkVertex(max, min, min, collision_found2);
					resolveCollisions_checkVertex(max, min, max, collision_found2);
					resolveCollisions_checkVertex(max, max, min, collision_found2);
					resolveCollisions_checkVertex(max, max, max, collision_found2);
				}
collision_found2:

				node->box.min.x += react.x;
				node->box.max.x += react.x;

				node->box.min.z += react.z;
				node->box.max.z += react.z;

				if(node->type==PLAYER && stair){
					/* player hit a stair-mode object while
					   moving on X and Z axes; walk the player 
					   up the stair */
					node->box.move.y = 1.0f;
					node->box.min.y += react.y;
					node->box.max.y += react.y;
				}

				if(react.x || react.y || react.z) {
					(*handler)(node, node2);
					(*handler)(node2, node);
				}

				/*
				 * Restore the move vector as
				 * it was prior to the Y axis / X, Z axes
				 * separation for further use
				 */
				node->box.move.x = move.x;
				node->box.move.y = move.y;
				node->box.move.z = move.z;
			}
		}
	}
}


