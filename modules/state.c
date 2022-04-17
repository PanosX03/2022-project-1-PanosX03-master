
#include <stdlib.h>
#include <stdio.h>
#include "ADTList.h"
#include "state.h"

const char* get_type (int num){
	switch(num){
		case 0:
			return "Terrain";
		case 1:
			return "Hellicopter";
		case 2:
			return "Warship";
		case 3:
			return "Jet";
		case 4:
			return "Missile";
		case 5:
			return "Bridge";
	}
}

// Ενα List είναι pointer σε αυτό το struct
struct list {
	ListNode dummy;				// χρησιμοποιούμε dummy κόμβο, ώστε ακόμα και η κενή λίστα να έχει έναν κόμβο.
	ListNode last;				// δείκτης στον τελευταίο κόμβο, ή στον dummy (αν η λίστα είναι κενή)
	int size;					// μέγεθος, ώστε η list_size να είναι Ο(1)
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο της λίστας.
};

struct list_node {
	ListNode next;		// Δείκτης στον επόμενο
	Pointer value;		// Η τιμή που αποθηκεύουμε στον κόμβο
};

Pointer list_get_at(List list, int pos) {
    int i = 0;
    for(ListNode node = list->dummy->next; node != NULL; node = node->next ){
        if (i == pos){
            return node->value;
        }
        i++;
    }
	// return NULL;
}

void list_remove(List list, ListNode node) {
		ListNode prev = list->dummy;
		ListNode temp =NULL;
		for(ListNode new = list->dummy; new != LIST_EOF; new = new->next){
			if(new == node){
				temp = new->next;
				list->destroy_value(new->value);
				free(new);
				prev->next = temp;
				break;
			}
			prev = new;
		}
		list->size--;
}

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	List objects;			// περιέχει στοιχεία Object (Εδαφος / Ελικόπτερα / Πλοία/ Γέφυρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από y = start_y, και επεκτείνονται προς τα πάνω.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι το αριστερό μέρος της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά. Η πίστα έχει σταθερό πλάτος, οπότε όλες οι
//   συντεταγμένες x είναι ανάμεσα στο 0 και το SCREEN_WIDTH.
//
// - Στον άξονα y το 0 είναι η αρχή της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα αντιθέτως επεκτείνεται προς τα πάνω, οπότε όλες οι
//   συντεταγμένες των αντικειμένων είναι _αρνητικές_.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_y) {
	// Προσθέτουμε BRIDGE_NUM γέφυρες.
	// Στο διάστημα ανάμεσα σε δύο διαδοχικές γέφυρες προσθέτουμε:
	// - Εδαφος, αριστερά και δεξιά της οθόνης (με μεταβαλλόμενο πλάτος).
	// - 3 εχθρούς (ελικόπτερα και πλοία)
	// Τα αντικείμενα έχουν SPACING pixels απόσταση μεταξύ τους.

	for (int i = 0; i < BRIDGE_NUM; i++) {
		// Δημιουργία γέφυρας
		Object bridge = create_object(
			BRIDGE,
			0,								// x στο αριστερό άκρο της οθόνης
			start_y - 4 * (i+1) * SPACING,	// Η γέφυρα i έχει y = 4 * (i+1) * SPACING
			SCREEN_WIDTH,					// Πλάτος ολόκληρη η οθόνη
			20								// Υψος
		);

		// Δημιουργία εδάφους
		Object terrain_left = create_object(
			TERRAIN,
			0,								// Αριστερό έδαφος, x = 0
			bridge->rect.y,					// y ίδιο με την γέφυρα
			rand() % (SCREEN_WIDTH/3),		// Πλάτος επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		int width = rand() % (SCREEN_WIDTH/2);
		Object terrain_right = create_object(
			TERRAIN,
			SCREEN_WIDTH - width,			// Δεξί έδαφος, x = <οθόνη> - <πλάτος εδάφους>
			bridge->rect.y,					// y ίδιο με τη γέφυρα
			width,							// Πλάτος, επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);

		list_insert_next(state->objects, list_last(state->objects), terrain_left);
		list_insert_next(state->objects, list_last(state->objects), terrain_right);
		list_insert_next(state->objects, list_last(state->objects), bridge);

		// Προσθήκη 3 εχθρών πριν από τη γέφυρα.
		for (int j = 0; j < 3; j++) {
			// Ο πρώτος εχθρός βρίσκεται SPACING pixels κάτω από τη γέφυρα, ο δεύτερος 2*SPACING pixels κάτω από τη γέφυρα, κλπ.
			float y = bridge->rect.y + (j+1)*SPACING;

			Object enemy = rand() % 2 == 0		// Τυχαία επιλογή ανάμεσα σε πλοίο και ελικόπτερο
				? create_object(WARSHIP,    (SCREEN_WIDTH - 83)/2, y, 83, 30)		// οριζόντιο κεντράρισμα
				: create_object(HELICOPTER, (SCREEN_WIDTH - 66)/2, y, 66, 25);
			enemy->forward = rand() % 2 == 0;	// Τυχαία αρχική κατεύθυνση

			list_insert_next(state->objects, list_last(state->objects), enemy);
		}
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->info.score = 0;					// Αρχικό σκορ 0
	state->info.missile = NULL;				// Αρχικά δεν υπάρχει πύραυλος
	state->speed_factor = 1;				// Κανονική ταχύτητα

	// Δημιουργία του αεροσκάφους, κεντραρισμένο οριζόντια και με y = 0
	state->info.jet = create_object(JET, (SCREEN_WIDTH - 35)/2,  0, 35, 40);

	// Δημιουργούμε τη λίστα των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_y = 0.
	state->objects = list_create(NULL);
	add_objects(state, 0);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	StateInfo si = &(state->info);
	return si;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο y_from και y_to.

List state_objects(State state, float y_from, float y_to) {
	struct state s = *state;
	List obj = s.objects;
	for(ListNode node = list_first(obj); node != LIST_EOF; node = list_next(obj, node)){
		Object antikimeno = list_node_value(obj, node);
		if (antikimeno->rect.y >= y_from && antikimeno->rect.y <= y_to){
			printf("antikimeno type %d with y %f\n", antikimeno->type , antikimeno->rect.y);
		}
		//printf("antikimeno %.1f\n", antikimeno->rect.y);
	}
	return NULL;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	while (state->info.playing){

		//paused statements
		while(state->info.paused){//<<<-------------------------------------------------------------------------|
			if(IsKeyPressed(KEY_P)){//																			|
				state->info.paused = false;//																	|
			}						//																			|
			if(IsKeyPressed(KEY_N)){//																			|
				break;			//if we press n then the game is still paused, does a whole loop and comes back here
			}
		}
		if(IsKeyPressed(KEY_P)){
			state->info.paused = true;
			continue;
		}

		

		//jet movement
	 	printf("X and Y Was %.1f, %.1f and became ", state->info.jet->rect.x , state->info.jet->rect.y);
		if (IsKeyDown(KEY_UP))
			state->info.jet->rect.y -= 6;
		else if (IsKeyDown(KEY_DOWN))
			state->info.jet->rect.y -= 2;
		else 
			state->info.jet->rect.y -= 3;
		if (IsKeyDown(KEY_LEFT))
			state->info.jet->rect.x -= 3;
		else if (IsKeyDown(KEY_RIGHT))
			state->info.jet->rect.x += 3;
		printf("%.1f, %.1f\n", state->info.jet->rect.x, state->info.jet->rect.y);

			if(state->info.missile){
				state->info.missile->rect.y -= 10;
				//printf("missile moved (%.1f %.1f)\n", state->info.missile->rect.x, state->info.missile->rect.y);
				for(int j = 0; j < list_size(state->objects) ; j++){
					Object objeEnem = list_get_at(state->objects, j);
					if(objeEnem->type == WARSHIP || objeEnem->type == HELICOPTER || objeEnem->type == BRIDGE){
						//printf("(%.1f %1.f) ,(%.1f %1.f)) \n", state->info.missile->rect.x,state->info.missile->rect.y,  objeEnem->rect.x, objeEnem->rect.y);
						if(CheckCollisionRecs(state->info.missile->rect, objeEnem->rect)){
							list_remove(state->objects, objeEnem);
							state->info.score += 10;
							//printf("score %d\n", state->info.score);
							state->info.missile = NULL;
							break;
						}
					}
				}
			}

		//checking for collision with jet
		for(int i = 0 ; i < list_size(state->objects) ; i++){
			Object obje = list_get_at(state->objects, i);
			//printf("%d object type\n", obje->type);
			if(obje->type == TERRAIN || obje->type == HELICOPTER || obje->type == WARSHIP || obje->type == BRIDGE){
				if(CheckCollisionRecs(state->info.jet->rect, obje->rect)){
					state->info.playing = false;
					printf("collision with %s, at (%.1f %.1f)\n", get_type(obje->type), state->info.jet->rect.x, state->info.jet->rect.y);
					break;
				}
			}
			//!IsKeyPressed(KEY_SPACE);
			if(!IsKeyPressed(KEY_SPACE) && state->info.missile == NULL){
				printf("missile launched\n");
				state->info.missile = create_object(MISSILE, state->info.jet->rect.x, state->info.jet->rect.y, 100, 100);
				//state->info.missile = true;
				printf("list size = %d\n", list_size(state->objects));
			}

			
			

			


			// moving warships
			if(obje->type == WARSHIP){
				if(obje->forward){
					obje->rect.x += 3;
				}
				else
					obje->rect.x -= 3;
					

				//checking warship collision with walls
				for(int j = 0; j < list_size(state->objects) ; j++){
					Object objeTER = list_get_at(state->objects, j);
					if(objeTER->type == TERRAIN){
						if(CheckCollisionRecs(obje->rect, objeTER->rect)){
							obje->forward = !(obje->forward);
						}
					}
				}
			} //warship loop end


			//moving helicopters
			if(obje->type == HELICOPTER){
				if(obje->forward)
					obje->rect.x += 4;
			 	else
			 		obje->rect.x -= 4;


				//checking for helicopter collision with walls	 
				for(int j = 0; j < list_size(state->objects) ; j++){
					Object objeTER = list_get_at(state->objects, j);
					if(objeTER->type == TERRAIN){
						if(CheckCollisionRecs(obje->rect, objeTER->rect)){
							obje->forward = !(obje->forward);
						}
					}
				}
			}	//helicopter loop end
		}
	}

	while (state->info.playing == false){
		sleep(2); //gia user input
		if(!IsKeyPressed(KEY_ENTER)){
			state->info.playing = true;
			state_destroy(state);
			state = state_create();
			//printf("number %d\n", list_size(state->objects));
			state_update(state, keys);
		}
	}
	
}
// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	free(state);
}