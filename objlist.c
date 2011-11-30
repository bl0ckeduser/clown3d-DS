#include "headers.h"

void freeObjs(game_obj* objs)
{
        /* Free game objs */
	game_obj* node = objs;
                while(node->prev != NULL) {
                        if(node->prev != NULL)
                                free(node->prev);
                        if(node->data != NULL)
                                free(node->data);
                        node = node -> next;
                }

}

void deleteNode(game_obj* node)
{
	/*
	 * A <-> B <-> C
	 * A <-> C
	 */
	game_obj* prev = node->prev;
	game_obj* next = node->next;

	if(prev != NULL)
		if(prev->next != NULL)
			prev->next = next;
	if(next != NULL)
		if(next->prev != NULL)
			next->prev = prev;

	gc_push(node);
}

game_obj* newListNode(game_obj* list)
{
        game_obj *curr = list;
        game_obj* prev;

        while(curr->next)
                curr = curr->next;
        curr->next = (game_obj *)malloc(sizeof(game_obj));
        prev = curr;
        curr = curr->next;
        curr->prev = prev;
        curr->next = NULL;
	return curr;
}

game_obj* newList(void)
{
	game_obj* objs = (game_obj *)malloc(sizeof(game_obj));
	objs->type = NONE;	/* thanks valgrind */
        objs->prev = NULL;
        objs->data = NULL;
        objs->next = NULL;
	return objs;
}
