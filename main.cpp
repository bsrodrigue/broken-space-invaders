#include <cstdint>
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
#define CELL_COUNT 50
#define CELL_SIZE WINDOW_WIDTH / CELL_COUNT

#define GET_INGAME_POS(ABS_POS) ABS_POS *CELL_SIZE

//-------------[Entities]-------------//
#define ENEMY_COLS 5
#define ENEMY_SPACING 5

namespace grid {
void draw_cell(int x, int y, Color color = WHITE) {
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}
} // namespace grid

uint8_t player_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {1, 1, 1},
};

uint8_t projectile_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 1, 0},
};

// Enemy Shapes
uint8_t alpha_enemy_shape[3][3] = {
    {1, 1, 1},
    {0, 1, 0},
    {1, 1, 1},
};

uint8_t beta_enemy_shape[3][3] = {
    {0, 1, 0},
    {1, 1, 1},
    {0, 1, 0},
};

uint8_t zeta_enemy_shape[3][3] = {
    {1, 0, 1},
    {0, 1, 0},
    {1, 0, 1},
};

// Enemy Placement
uint8_t enemy_placement[3][5] = {
    {1, 1, 1, 1, 1},
    {2, 2, 2, 2, 2},
    {3, 3, 3, 3, 3},
};

// Initial positions
Vector2 player_pos = {(float)CELL_COUNT / 2, CELL_COUNT - 5};

Vector2 projectile_pos = {player_pos.x, player_pos.y + 1};

void init_enemies_pos(Vector2 *pos_arr, float y) {
  Vector2 base_pos = {3, y};
  for (uint8_t i = 0; i < ENEMY_COLS; i++) {
    pos_arr[i] = Vector2Add(base_pos, {(float)(i * ENEMY_SPACING), 0});
  }
}

bool player_is_shooting = false;

void draw_shape(Vector2 pos, uint8_t shape[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int cell = shape[i][j];

      if (cell != 0) {
        grid::draw_cell(pos.x + j, pos.y + i);
      }
    }
  }
}

enum EnemyType {
  ALPHA,
  BETA,
  ZETA,
};

void draw_player() { draw_shape(player_pos, player_shape); }
void draw_projectile() { draw_shape(projectile_pos, projectile_shape); }

// Terrible code
void draw_enemies(Vector2 *positions, EnemyType type) {
  for (uint8_t i = 0; i < ENEMY_COLS; i++) {
    switch (type) {
    case ALPHA:
      draw_shape(positions[i], alpha_enemy_shape);
      break;
    case BETA:
      draw_shape(positions[i], beta_enemy_shape);
      break;
    case ZETA:
      draw_shape(positions[i], zeta_enemy_shape);
      break;
    }
  }
}

Rectangle get_rect_from_enemy(Vector2 pos) {
  Rectangle rect;
  rect.x = GET_INGAME_POS(pos.x);
  rect.y = GET_INGAME_POS(pos.y);

  rect.width = ((float)CELL_SIZE * 3);
  rect.height = rect.width;

  return rect;
}

Rectangle get_rect_from_projectile(Vector2 pos) {
  Rectangle rect;
  rect.x = GET_INGAME_POS((pos.x + 1));
  rect.y = GET_INGAME_POS((pos.y + 1));

  rect.width = ((float)CELL_SIZE);
  rect.height = ((float)CELL_SIZE * 2);

  return rect;
}

void draw_collision_box(Rectangle rect) { DrawRectangleRec(rect, RED); }

void draw_collision_boxes() {
  Rectangle enemy_rect = get_rect_from_enemy({3, 3});
  Rectangle projectile_rect = get_rect_from_projectile(projectile_pos);
  draw_collision_box(enemy_rect);
  draw_collision_box(projectile_rect);
}

// How to handle this shit?
bool check_enemy_collision(Vector2 enemy_pos, Vector2 projectile_pos) {
  Rectangle enemy_rect = get_rect_from_enemy(enemy_pos);
  Rectangle projectile_rect = get_rect_from_projectile(projectile_pos);

  return CheckCollisionRecs(enemy_rect, projectile_rect);
}

void kill_enemy() {}

void reset_projectile(Vector2 player_pos, Vector2 *projectile_pos) {
  projectile_pos->x = player_pos.x;
  projectile_pos->y = player_pos.y;

  player_is_shooting = false;
}

bool check_out_of_bounds(Vector2 pos) { return false; }

void render() {
  // Player
  draw_player();
  // DEBUG: Collision Boxes
  draw_collision_boxes();
  draw_projectile();

  // Enemies
  draw_shape({3, 3}, alpha_enemy_shape);
}

void update_pos() {
  // Projectile
  if (player_is_shooting) {
    projectile_pos = Vector2Add(projectile_pos, {0, -1});
  }

  // Collisions
  if (check_enemy_collision({3, 3}, projectile_pos)) {
    // DEBUG: Enemy - Projectile Collision
    TraceLog(LOG_INFO, "Enemy Hit");

    reset_projectile(player_pos, &projectile_pos);
  }

  // Out of Bounds
  if (projectile_pos.y <= -2) {
    reset_projectile(player_pos, &projectile_pos);
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

void init_game() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

  if (!IsWindowReady()) {
    exit(EXIT_FAILURE);
  }

  HideCursor();

  SetTargetFPS(FPS);

  // Game Setup
}

void close_game() {}

int main(int argc, char *argv[]) {
  init_game();

  game_loop();

  close_game();

  return EXIT_SUCCESS;
}
