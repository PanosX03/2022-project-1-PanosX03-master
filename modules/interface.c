#include "raylib.h"

#include "state.h"
#include "interface.h"


// Assets
Texture bird_img;

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);

	// Φόρτωση εικόνων και ήχων
	bird_img = LoadTextureFromImage(LoadImage("assets/bird.png"));
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

	// Σχεδιάζουμε τον χαρακτήρα και τις 2 μπάλες
	DrawTexture(bird_img, state->character.x, state->character.y, WHITE);

	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", state->score), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!state->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}

	EndDrawing();
}