
/*
 *	The Clown3D game demo
 *
 *   My first attempt at a 3D
 *   platformer. Very, very
 *   simple.
 */

/*
 * 	This is a port to libnds
 *	of clown3d-20111112.
 *
 *	Note that it can also
 *	compile with SDL + GL
 *	like the usual clown3d.
 *
 *	The ported code is a bit
 *	messy, but not too much.
 */

extern char door_model[];
extern char key_model[];
extern char maze_model[];
extern char turtle_model[];
extern char bullet_model[];

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "headers.h"

int vc;
int bullets = 0;

#ifdef PC_TARGET
/* SDL event structures */
SDL_Event event;
Uint8 *keystate;
#endif

/* the game's models */
model* turtleModel;
model* worldModel;
model* keyModel;
model* doorModel;
model* bulletModel;
model* targetModel;

/* game objects */
game_obj *objs = NULL, *node = NULL, *player = NULL;

/* the game's lighting */
GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat ambient_light[] = { 0.5, 0.5, 0.5, 1.0 };

/* materials */
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

/* variable FPS physics -- time corrector */
float dtime;

/* Intro camera movement offset */
float intro_offset = 0.0/* 100 */;

/* Camera angle, offset */
int cam_angle = 1;
float yoffs = /* 83.0 */20.0;


void runGameFrame(void);

/*
 * bunch of pseudo-OOP stuff follows
 */

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

int main(int argc, char* argv[])
{
	#ifdef PC_TARGET
	/* SDL video stuff */
	SDL_Surface* display;
	SDL_VideoInfo* video_info;
	#else
	/* NDS: setup text logging */
	consoleDemoInit();		
	videoSetMode(MODE_FB0);
	vramSetBankA(VRAM_A_LCD);

	/* Credits */
	printf("clown3d demo by bl0ckeduser\n");
	printf("DS port based on 3d example code written by Dovoto (thanks !)\n");
	printf("Made with devKitPro and libnds\n\n");
	#endif

	printf("Loading turtle model...");	
	turtleModel = loadModel(turtle_model);		/* player model */
	printf("done.\n");

	printf("Loading bullet model...");	
	bulletModel = loadModel(bullet_model);
	printf("done.\n");

	printf("Loading door model...");
	doorModel = loadModel(door_model);
	printf("done.\n");

	printf("Loading key model...");
	keyModel = loadModel(key_model);
	printf("done.\n");

	printf("Creating world... ");
	objs = newList();
	player = newPlayer(objs, 0, 20, 0);
	(void)newDoor(objs, -7.456112, -2.289094, -124.128632);
	(void)newDoor(objs, -85.255035,-2.289177,12.114304);
	(void)newKey(objs, -44.039379, 2.111141, -40.276863);
	(void)newKey(objs, -73.642761, -2.289178, -166.881195);
	printf("done.\n");

	printf("Loading world model... ");
	worldModel = loadModel(maze_model);
	printf("done.\n");

	printf("Making world bounding boxes...");
	genBoxes(worldModel, objs);
	printf("done.\n");

	printf("* STARTING GAME *\n");

	#ifdef PC_TARGET
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		video_info = (SDL_VideoInfo *)SDL_GetVideoInfo();

		display = SDL_SetVideoMode(
		              640, 480,
		              video_info -> vfmt -> BitsPerPixel,
		              SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL
				|
				(argc > 1 ?
					(argc > 2 ?
						(strcmp(argv[2], "-f")==0 ?
							SDL_FULLSCREEN
							: 0
						)
						: (strcmp(argv[1], "-f")==0 ?
							SDL_FULLSCREEN
							: 0
						)
					)
					: 0
				)
			);

		require(display != NULL);
	#else
	if(1){
	#endif

		#ifndef PC_TARGET
			/* 
			   initialization taken copied from
			   Graphics/3D/nehe/lesson03/source/nehe3.cpp
			   by Dovoto
			*/
				// Setup the Main screen for 3D 
				videoSetMode(MODE_0_3D);
    
				// initialize the geometry engine
				glInit();
    
				// enable antialiasing
				glEnable(GL_ANTIALIAS);
    
				// setup the rear plane
				glClearColor(0,0,0,31); // BG must be opaque for AA to work
				glClearPolyID(63); // BG must have a unique polygon ID for AA to work
				glClearDepth(GL_MAX_DEPTH);
    
				// Set our viewport to be the same size as the screen
				glViewport(0,0,255,191);
		#endif

		#ifdef PC_TARGET
		/* Lighting and materials for the PC version.
		   Won't build with libnds, because it has a
		   slightly different GL. Also, I'm not sure
		   whether the DS supports all these effects. */
	
		/* Enable Z-buffering, lighting */
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		/* Switch lights on */
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		/* Allow lit colors */
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		/* Avoid scaled normals pitfall */
		glEnable(GL_NORMALIZE);

		/* ================== Constant lighting and materials ==================== */

		light_position[0] = light_position[2] = 0.0f;
		light_position[1] = 60.0f;

		/* light 0 - ambient */
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);

		/* the material everything is made of */
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

		/* light 1 - diffuse */
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light);

		/* ======================================================================= */
		#endif

		while(1) {
			#ifdef PC_TARGET
			SDL_PollEvent(&event);
			keystate = SDL_GetKeyState(NULL);
			if(keystate[SDLK_ESCAPE] || keystate[SDLK_q]) break;
			if(event.type == SDL_QUIT) break;
			#endif

			runGameFrame();

			#ifdef PC_TARGET
			LimitFPS(30);
			SDL_GL_SwapBuffers();
			#else
				/* framerate code based on Dovoto's examples */
				// flush to screen  
        			glFlush(0);
				swiWaitForVBlank();
				swiWaitForVBlank();		/* 30 fps */
			#endif
		}

		freeObjs(objs);

		#ifdef PC_TARGET
		/* Free SDL display */
		SDL_FreeSurface(display);
		#endif

		/* Free game models */
		freeModel(turtleModel);
		freeModel(worldModel);
		freeModel(bulletModel);
		freeModel(doorModel);
		freeModel(keyModel);
		
		/*
			freeModel(targetModel);
		*/

		gc_stop();

		#ifdef PC_TARGET
		SDL_Quit();
		#endif
	}

	/* RAM usage estimation on PC */
	/* memcheck(); */

	return 0;
}

void runGameFrame(void)
{
	static int trace = 0;

	/****************** COMPUTE DTIME ***************/

	/* assume 30 FPS */
	dtime = (1000.0f / 30.0f) * TIME_UNIT / 1000.0;

	/************************************************/

	/***** TICK OBJECTS *********/

	for(node = objs; node; node = node->next)
		tickFunction(node);

	/****************************/

	/***** RESOLVE ALL COLLISIONS *******/

	resolveCollisions(objs, collisionFunction);

	/***********************************/

	/********* DRAW EVERYTHING ON THE SCREEN *********/

	#ifdef PC_TARGET
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	/* on NDS, glClear() is used earlier on */
	#endif

	/* Setup the camera */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	#ifdef PC_TARGET
	gluPerspective(60.0f, (GLdouble)640.0f/480.0f, 0.1f, 100.0f);
	#else
	gluPerspective(60.0f, 255.0f/192.0f, 0.1f, 100.0f);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);	/* again, strongly based on Dovoto's code */
	#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch(cam_angle) {
	case 1:		/* camera placed at constant distance of player.
				   coords are calculated from the player by adding
				   a scaled version of the opposite of the X, Z direction
				   vector */
		gluLookAt((float)(player->data[PLAYER_X] - player->data[PLAYER_DIRX]*(60+intro_offset))/10.0,
		          (float)(player->data[PLAYER_Y]+yoffs)/10.0,
		          (float)(player->data[PLAYER_Z]-player->data[PLAYER_DIRZ]*(60+intro_offset))/10.0,
		          (float)(player->data[PLAYER_X]/10.0),
		          (float)(player->data[PLAYER_Y]/10.0),
		          (float)(player->data[PLAYER_Z]/10.0),
		          0.0f, 1.0f,0.0f);
		break;

	case 2:		/* first person view */
		gluLookAt((float)(player->data[PLAYER_X] - player->data[PLAYER_DIRX]*20)/10.0,
		          (float)(player->data[PLAYER_Y])/10.0,
		          (float)(player->data[PLAYER_Z]-player->data[PLAYER_DIRZ]*20)/10.0,
		          (float)(player->data[PLAYER_X]/10.0),
		          (float)(player->data[PLAYER_Y]/10.0),
		          (float)(player->data[PLAYER_Z]/10.0),
		          0.0f, 1.0f,0.0f);
		break;
	}


	#ifdef PC_TARGET
	/*** LIGHTING ************/

	/* position light 1 - diffuse */
	light_position[0] = (GLfloat)(player->data[PLAYER_X] - player->data[PLAYER_DIRX]*5)/10.0;
	light_position[1] = (GLfloat)(player->data[PLAYER_Y]+10)/10.0;
	light_position[2] = (GLfloat)(player->data[PLAYER_Z]-player->data[PLAYER_DIRZ]*5)/10.0;
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	/**************************/
	#endif

#ifdef PC_TARGET
	vc = 0;		/* vertex count checking on PC */
#endif

	/* Draw the world model */
	glPushMatrix();
	glScalef(0.1f, 0.1f, 0.1f);
	drawModelWithGL_big(worldModel);	/* see model-draw.c for _big explanation */
#ifdef PC_TARGET
	glPopMatrix();
#else
	glPopMatrix(1);
#endif

	/* Draw objects */
	for(node = objs; node; node = node->next)
	{
		glPushMatrix();
		drawFunction(node);
#ifdef PC_TARGET
		glPopMatrix();
#else
		glPopMatrix(1);
#endif
	}



#ifdef PC_TARGET
	/* vertex count checking on PC */

	char buf[256];
	sprintf(buf, "clown3d-DS :: PC build :: %d vertices", vc);
	SDL_WM_SetCaption(buf, NULL);	
	if(vc > 6144){
		printf("Vertex count too high: %d\n", vc);
	}
#endif

	/*******************************************/

	gc_collect();
}

#ifndef PC_TARGET

/* The following routines were copied from
   nehe11.cpp, written by Dovoto, and renamed
   from sin, cos to my_sin, my_cos because
   it didn't seem to build otherwise */

float my_sin(float angle)
{
	int32 s = sinLerp((int)((angle * DEGREES_IN_CIRCLE) / 360.0));

	return f32tofloat(s);
}

float my_cos(float angle)
{
	int32 c = cosLerp((int)((angle * DEGREES_IN_CIRCLE) / 360.0));

	return f32tofloat(c);
}

#endif
