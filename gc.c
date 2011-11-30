#include <stdlib.h>
#include "headers.h"

game_obj** garbage;
int count = 0;
int alloc = 0;

void gc_push(game_obj* ptr)
{
	if(++count > alloc)
	{
		alloc += 10;
		garbage = (game_obj **)realloc(garbage, alloc * sizeof(game_obj*));
	}
	garbage[count] = ptr;
}

void gc_collect(void)
{
	int i;

	for(i = count; i > 0; i--)
	{
		if(garbage[i]->data != NULL)
		{
			free(garbage[i]->data);
			garbage[i]->data = NULL;
		}
	}

	while(count > 0)
	{
		if(garbage[count] != NULL)
		{
			free(garbage[count]);
			garbage[count] = NULL;
		}
		--count;
	}
}

void gc_stop(void)
{
	gc_collect();
	free(garbage);
}
