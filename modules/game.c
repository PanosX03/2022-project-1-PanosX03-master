//////////////////////////////////////////////////////////////////////////////
//
// Παράδειγμα δημιουργίας ενός παιχνιδιού χρησιμοποιώντας τη βιβλιοθήκη raylib
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "raylib.h"


#include "state.h"

State state;

// void update_and_draw() {
// 	state_update(state);
// 	interface_draw_frame(state);
// }

int main() {
	state = state_create();
	StateInfo stateinf = state_info(state);
	printf("%d\n", stateinf->score);
	List list = state_objects(state, -15400, -800);
	KeyState keys = { false, false, false, false, false, true, false, false };
	state_update(state, keys);
	
	//interface_init();

	//Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	//start_main_loop(update_and_draw);

	//interface_close();

	return 0;
}