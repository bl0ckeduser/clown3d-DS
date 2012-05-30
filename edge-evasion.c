
#include "headers.h"

void edge_evade(game_obj* who, game_obj* what, int dir)
{
	if(what)
		who->ee_bits |= (2 << dir);

	if(!who->ee_targ) {
		who->ee_targ = what;
		who->ee_dir = dir;
	}

	if(!what) {
		who->ee_targ = NULL;
		who->ee_bits = 0;
	}
}
