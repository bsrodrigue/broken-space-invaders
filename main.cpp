#include <cstdio>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

//-------------[Broken Space Invaders]----------------------------//

//-------------[Window]-------------//

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

#define WINDOW_TITLE "Broken Space Invaders"

//-------------[Performance]-------------//
#define FPS 30

//-------------[Grid]-------------//
#define CELL_COUNT 100
#define CELL_SIZE WINDOW_WIDTH / CELL_COUNT

namespace grid {
void draw_cell(int x, int y, Color color = WHITE) {
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}
} // namespace grid

void init_game() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

  if (!IsWindowReady()) {
    exit(EXIT_FAILURE);
  }

  HideCursor();

  SetTargetFPS(FPS);
}

void close_game() {}

int player_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {1, 1, 1},
};

int projectile_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 0, 0},
};

// Initial position
Vector2 player_pos = {(float)CELL_COUNT / 2, CELL_COUNT - 5};

Vector2 projectile_pos = {player_pos.x, player_pos.y};

bool player_is_shooting = false;

void draw_shape(Vector2 pos, int shape[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int cell = shape[i][j];

      if (cell != 0) {
        grid::draw_cell(pos.x + j, pos.y + i);
      }
    }
  }
}

void draw_player() { draw_shape(player_pos, player_shape); }
void draw_projectile() { draw_shape(projectile_pos, projectile_shape); }

void render() {
  // Player
  draw_player();
  draw_projectile();
}

void update_pos() {
  // Projectile
  if (player_is_shooting) {
    projectile_pos = Vector2Add(projectile_pos, {0, -1});
  }
}

void get_player_input(int pressed_key) {

  switch (pressed_key) {
  case KEY_SPACE:
    player_is_shooting = true;
    break;
  case KEY_LEFT:
    player_pos = Vector2Add(player_pos, {-1, 0});
    break;
  case KEY_RIGHT:
    player_pos = Vector2Add(player_pos, {1, 0});
    break;
  default:
    break;
  }

  if (!player_is_shooting) {
    projectile_pos = player_pos;
  }
}

double last_update_time = 0;

bool can_update_pos() {
  double current_time = GetTime();

  if (current_time - last_update_time >= 0.1) {
    last_update_time = current_time;
    return true;
  }

  return false;
}

void game_loop() {
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    int pressed_key = GetKeyPressed();
    get_player_input(pressed_key);

    if (can_update_pos()) {
      update_pos();
    }

    render();
    EndDrawing();
  }
}

int main(int argc, char *argv[]) {
  init_game();

  game_loop();

  close_game();

  return EXIT_SUCCESS;
}
