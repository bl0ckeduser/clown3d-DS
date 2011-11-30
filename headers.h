/*
 * GL headers
 */
#ifndef ALLHEADERS
#define ALLHEADERS

#define malloc malloc_check

extern int vc;

#ifndef PC_TARGET
	#define GLfloat float
	#define GLdouble double
#endif

/* game constants */
#define MAX_YVEL 16.99
#define TIME_UNIT 10.0f
#define ACCEL -6.5

#ifdef PC_TARGET

#ifdef __APPLE__
#define MACBUILD
#endif

#ifdef MACBUILD
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <SDL/SDL.h>

#else

#include <nds.h>

#endif

/* Point / vector */
typedef struct vector {
	float x;	
	float y;
	float z;
} vector;

typedef enum
{
	PLAYER,
	SOLID,
	KEY,
	BULLET,
	TARGET,
	DOOR,
	NONE
} obj_type;


/* Collision engine object */
typedef struct collisionBox {
	vector min;
	vector max;
	vector move;
} collisionBox;

/* doubly-linked list */
typedef struct game_obj
{
	obj_type type;
	vector coords;
	float rot;
	collisionBox box;
	float* data;
	void* prev;
	void* next;
} game_obj;

extern void resolveCollisions(game_obj* obj_list, void (*handler)(void*, void*));

/* field names for all-purpose data storage */

/* PLAYER */
enum
{
	PLAYER_X,
	PLAYER_Y,
	PLAYER_Z,
	PLAYER_ON_PLATFORM,
	PLAYER_YVEL,
	PLAYER_DIRX,
	PLAYER_DIRZ,
	PLAYER_ANGLE,
	PLAYER_MOVEX,
	PLAYER_MOVEY,
	PLAYER_MOVEZ,
	PLAYER_KEYS,
	PLAYER_BULLET_TIMER
};

/* KEY */
enum
{
	KEY_EXISTS
};

/* BULLET */
enum
{
	BULLET_X,
	BULLET_Y,
	BULLET_Z,
	BULLET_YVEL,
	BULLET_ANGLE,
	BULLET_TIMER,
	BULLET_EXPLODED
};

/* TARGET */
enum
{
	TARGET_X,
	TARGET_Y,
	TARGET_Z,
	TARGET_ANGLE,
	TARGET_EXPLODED
};

/* DOOR */
enum
{
	DOOR_CLOSED
};

/********************************************************************/

#define VERTEX_ARRAY_INCREMENT 32

/* Face */
typedef struct
{
	int* vertnum;				/* array of vertex numbers */
	int point_count;				/* triangle or quad ? the DS has to know ! */
	void* next;					/* pointer to next face */
} face;

/* Vertex */
typedef struct
{
	float x, y, z;				/* coordinates */
	char exists;
} vert;

/* Model sub-group */
typedef struct
{
	float color[3];				/* group color */
	face* faces;				/* faces linked list */
	void* next;					/* next group */
} model_group;

/* Model */
typedef struct
{
	model_group* g;				/* sub-groups linked list */
	vert* vertices;				/* vertices array */
	int allocated_vert;			/* vertices array allocation */

	/* fields below are required because of a workaround to a DS quirk. see
	   model-draw.c */
	float x_scale;				/* abs. val of biggest coords for each axis */
	float y_scale;
	float z_scale;
} model;

/********************************************************************/

int drawModelWithGL(model* model);
int drawModelWithGL_big(model* model);
extern model* loadModel(char* fileName);
extern void freeModel(model* model);
extern void listModel(model* model);

/********************************************************************/
 
extern float dtime;

extern model* turtleModel;
extern model* worldModel;
extern model* keyModel;
extern model* doorModel;
extern model* bulletModel;
extern model* targetModel;

extern float intro_offset;
extern float yoffs;
extern int cam_angle;

#ifdef PC_TARGET
extern Uint8 *keystate;
#endif

extern game_obj* newKey(game_obj* list, float x, float y, float z);
extern game_obj* newPlayer(game_obj* list, float x, float y, float z);
extern game_obj* newDoor(game_obj* list, float x, float y, float z);
extern game_obj* newBullet(game_obj* list, float x, float y, float z, float angle);
extern game_obj* newTarget(game_obj*, float, float, float, float);

extern void doorInit(game_obj*);
extern void playerInit(game_obj*);
extern void keyInit(game_obj*);

extern void playerTick(game_obj*);
extern void keyTick(game_obj*);
extern void doorTick(game_obj*);

extern void playerDraw(game_obj*);
extern void keyDraw(game_obj*);
extern void doorDraw(game_obj*);

extern int loadWorldFile(char* nam, game_obj* list, model** worldModel);

extern game_obj* newListNode(game_obj* list);
extern game_obj* newList(void);

extern void genBoxes(model* theModel, game_obj* obj_list);

extern void freeObjs(game_obj* objs);

extern void require(int);

extern void deleteNode(game_obj* node);

extern void gc_push(game_obj*);

extern game_obj* player;

extern void whitespace(char** data);
extern char* read_str(char** data);
extern int read_int(char **data);
extern float read_float(char **data);

#ifndef PC_TARGET
extern float my_sin(float angle);
extern float my_cos(float angle);
#endif

#endif


