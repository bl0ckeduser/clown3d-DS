#include "headers.h"

#ifdef PC_TARGET
int loadWorldFile(char* nam, game_obj* list, model** worldModel);

int loadWorldFile(char* nam, game_obj* list, model** worldModel)
{
	FILE* wfil = NULL;
	char str[1024];
	float x, y, z, a;
	char c;

	wfil = fopen(nam, "r");
	if(!wfil)
	{
		printf("failed to read worldfile '%s'\n", nam);
		return 0;
	}

	while(1)
	{
		fscanf(wfil, "%s", str);
		if(feof(wfil))	break;
		if(!strcmp(str, "world-model")) {
			fscanf(wfil, "%s", str);
			if(worldModel)
				*worldModel = loadModel(str);
		} else if(!strcmp(str, "key")) {
			fscanf(wfil, "%f %f %f", &x, &y, &z);
			(void)newKey(list, x, y, z);
		} else if(!strcmp(str, "door")) {
			fscanf(wfil, "%f %f %f", &x, &y, &z);
			(void)newDoor(list, x, y, z);
		} else if(!strcmp(str, "target")) {
			fscanf(wfil, "%f %f %f %f", &x, &y, &z, &a);
			(void)newTarget(list, x, y, z, a);
		} else if(*str=='#'){
			while((c = fgetc(wfil)) != '\n' && !feof(wfil));
			ungetc(c, wfil);
		} else {
			printf("illegal command in worldfile: '%s'\n", str);
			return 0;
		}
	}

	fclose(wfil);
	return 1;
}

#endif
