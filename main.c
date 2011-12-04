
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
extern char maze_model[];	/* maze world */
extern char world_model[];	/* default world */
extern char turtle_model[];
extern char bullet_model[];

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "headers.h"

#ifdef PC_TARGET
/* vertex count checking */
int vc;

/* lighting and materials for PC version */
GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat ambient_light[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };

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

/* Here, everything runs at a constant 30 FPS,
   however the clown3d engine is designed 
   for variable FPS */
float dtime = (1000.0f / 30.0f) * TIME_UNIT / 1000.0;

/* Camera angle, offset */
int cam_angle = 1;
float yoffs = /* 83.0 */20.0;

void runGameFrame(void);

/* This addresses a quirk in libnds.
   The official GL version of glPopMatrix
   requires no parameters, but the libnds
   version has one parameter */
void PopMatrix(void)
{
#ifdef PC_TARGET
	glPopMatrix();
#else
	glPopMatrix(1);	/* copied from Dovoto */
#endif
}

int main(int argc, char* argv[])
{
	int chosen_world = 1;

#ifndef PC_TARGET
	/* Apparently, this will halt execution
	   if something illegal is done */
	defaultExceptionHandler();

	/* Irq init code based on eKid's example.
	   Added this in with hopes of increasing
	   No$GBA's code quality rating. */
	irqInit();
	irqEnable(IRQ_VBLANK | IRQ_KEYS);
#endif

#ifdef PC_TARGET
	/* SDL video stuff */
	SDL_Surface* display;
	SDL_VideoInfo* video_info;
#else
	/* allow text on lower screen */
	consoleDemoInit();	
	videoSetMode(MODE_FB0);
	vramSetBankA(VRAM_A_LCD);

	printf("clown3d demo by bl0ckeduser\n");
	printf("DS port based on 3d example code written by Dovoto (thanks !)\n");
	printf("Made with devKitPro and libnds\n");
	printf("built %s %s\n\n", __DATE__, __TIME__);

	/* let the player choose one of two worlds */
	printf("Press A to play in the default world; press B to play in the maze world\n\n");
	while(1){
		scanKeys();
		if(keysHeld() & KEY_A){
			chosen_world = 1;
			break;
		}

		if(keysHeld() & KEY_B){
			chosen_world = 2;
			break;
		}

		swiWaitForVBlank();
	}
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

	printf("Creating world objects... ");
	objs = newList();
	player = newPlayer(objs, 0, 20, 0);
	if(chosen_world == 2){	
		/* in the maze world, there are keys and doors */
		(void)newDoor(objs, -7.456112, -2.289094, -124.128632);
		(void)newDoor(objs, -85.255035,-2.289177,12.114304);
		(void)newKey(objs, -44.039379, 2.111141, -40.276863);
		(void)newKey(objs, -73.642761, -2.289178, -166.881195);
	}
	printf("done.\n");

	printf("Loading world model... ");
	if(chosen_world == 1){
		worldModel = loadModel(world_model);
	} else {
		worldModel = loadModel(maze_model);
	}
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
		          SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);

		require(display != NULL);
	#else
	if(1){
	#endif

		#ifndef PC_TARGET
		/* 
		   NDS 3D mode init code copied from
		   nehe3.cpp by Dovoto
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
	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		glEnable(GL_NORMALIZE);

		light_position[0] = light_position[2] = 0.0f;
		light_position[1] = 60.0f;

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light);

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
			/* framerate sync code based on Dovoto's examples */
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
		/* freeModel(targetModel); */

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
	/* 
	 * Tick game objects and resolve collisions 
	 */

	for(node = objs; node; node = node->next)
		tickFunction(node);

	resolveCollisions(objs, collisionFunction);


	/*
	 * Drawing code follows 
	 */

	#ifdef PC_TARGET
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	#endif

	/* Setup the camera */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef PC_TARGET
	gluPerspective(60.0f, 640.0f/480.0f, 0.1f, 100.0f);
#else
	gluPerspective(60.0f, 255.0f/192.0f, 0.1f, 100.0f);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);	/* copied from Dovoto */
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch(cam_angle) {
	case 1:		/* camera placed at constant distance of player.
			   coords are calculated from the player by adding
			   a scaled version of the opposite of the X, Z direction
			   vector */
		gluLookAt((float)(player->data[PLAYER_X] - player->data[PLAYER_DIRX]*(60))/10.0,
		          (float)(player->data[PLAYER_Y]+yoffs)/10.0,
		          (float)(player->data[PLAYER_Z]-player->data[PLAYER_DIRZ]*(60))/10.0,
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
	/* the original lighting code.
	   doesn't build with libnds */
	light_position[0] = (GLfloat)(player->data[PLAYER_X] - player->data[PLAYER_DIRX]*5)/10.0;
	light_position[1] = (GLfloat)(player->data[PLAYER_Y]+10)/10.0;
	light_position[2] = (GLfloat)(player->data[PLAYER_Z]-player->data[PLAYER_DIRZ]*5)/10.0;
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	vc = 0;		/* vertex count checking on PC */
#endif

	/* draw the world */
	glPushMatrix();
	glScalef(0.1f, 0.1f, 0.1f);
	drawModelWithGL_big(worldModel);	/* see model-draw.c for _big explanation */
	PopMatrix();

	/* draw objects */
	for(node = objs; node; node = node->next)
	{
		glPushMatrix();
		drawFunction(node);
		PopMatrix();
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

	gc_collect();
}

