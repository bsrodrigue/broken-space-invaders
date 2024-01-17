#include <cstdint>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <vector>

//-------------[Broken Space Invaders]----------------------------//

//-------------[Window]-------------//
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

#define WINDOW_TITLE "Broken Space Invaders"

//-------------[Performance]-------------//
#define FPS 30

//-------------[Grid]-------------//
#define CELL_COUNT 60
#define CELL_SIZE WINDOW_WIDTH / CELL_COUNT

#define GET_INGAME_POS(ABS_POS) ABS_POS *CELL_SIZE

//-------------[Entities]-------------//
#define ENEMY_H_SPACING 5
#define ENEMY_V_SPACING (ENEMY_H_SPACING - 1)

#define GUARD_H_SPACING 10

namespace grid {
void draw_cell(int x, int y, Color color = WHITE) {
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}
} // namespace grid

// Player Shape
uint8_t player_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {1, 1, 1},
};

// Projectile Shape
uint8_t projectile_shape[3][3] = {
    {0, 0, 0},
    {0, 1, 0},
    {0, 1, 0},
};

// Guard Shape
uint8_t guard_shape[3][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
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
// TODO: Find a way to use this
uint8_t enemy_placement[3][5] = {
    {1, 1, 1, 1, 1},
    {2, 2, 2, 2, 2},
    {3, 3, 3, 3, 3},
};

class Guard {
public:
  uint8_t life = 3;
  Vector2 pos;
};

// Initial positions
// Player Related Positions
Vector2 player_pos = {(float)CELL_COUNT / 2, CELL_COUNT - 5};
Vector2 projectile_pos = {player_pos.x, player_pos.y + 1};

// Guard
std::vector<Guard> guards;

// Enemy Related Positions
std::vector<Vector2> enemies_pos;

// TODO: Improve this code
void init_enemies_pos() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 5; j++) {
      float y = i * ENEMY_V_SPACING;
      float x = (j + 1) * ENEMY_H_SPACING;
      enemies_pos.push_back({x, y});
    }
  }
}

void init_guards() {
  for (int i = 0; i < 3; i++) {
    float y = CELL_COUNT - 10;
    float x = (i + 1) * GUARD_H_SPACING;
    Guard guard;
    guard.pos = {x, y};
    guards.push_back(guard);
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

bool enemy_to_right = true;

uint8_t enemy_mov_count = 0;

void draw_player() { draw_shape(player_pos, player_shape); }
void draw_projectile() { draw_shape(projectile_pos, projectile_shape); }

void draw_enemies() {
  for (uint8_t i = 0; i < enemies_pos.size(); i++) {
    draw_shape(enemies_pos[i], alpha_enemy_shape);
  }
}

void draw_guards() {
  for (uint8_t i = 0; i < guards.size(); i++) {
    draw_shape(guards[i].pos, guard_shape);
  }
}

Rectangle get_rect_from_guard(Vector2 pos) {
  Rectangle rect;
  rect.x = GET_INGAME_POS(pos.x);
  rect.y = GET_INGAME_POS(pos.y);

  rect.width = ((float)CELL_SIZE * 3);
  rect.height = rect.width;

  return rect;
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
  Rectangle projectile_rect = get_rect_from_projectile(projectile_pos);
  draw_collision_box(projectile_rect);

  for (uint8_t i = 0; i < enemies_pos.size(); i++) {
    Rectangle enemy_rect = get_rect_from_enemy(enemies_pos[i]);
    draw_collision_box(enemy_rect);
  }

  for (uint8_t i = 0; i < guards.size(); i++) {
    Rectangle guard_rect = get_rect_from_guard(guards[i].pos);
    draw_collision_box(guard_rect);
  }
}

bool check_enemy_collision(Vector2 enemy_pos, Vector2 projectile_pos) {
  Rectangle enemy_rect = get_rect_from_enemy(enemy_pos);
  Rectangle projectile_rect = get_rect_from_projectile(projectile_pos);

  return CheckCollisionRecs(enemy_rect, projectile_rect);
}

bool check_guard_collision(Vector2 guard_pos, Vector2 projectile_pos) {
  Rectangle guard_rect = get_rect_from_guard(guard_pos);
  Rectangle projectile_rect = get_rect_from_projectile(projectile_pos);

  return CheckCollisionRecs(guard_rect, projectile_rect);
}

void kill_enemy(uint8_t index) {
  enemies_pos.erase(enemies_pos.begin() + index);
}

void damage_guard(uint8_t index) {
  Guard *guard = &guards[index];
  if (guard->life <= 1) {
    // Kill
    guards.erase(guards.begin() + index);
    return;
  }

  guard->life--;
}

void reset_projectile(Vector2 player_pos, Vector2 *projectile_pos) {
  projectile_pos->x = player_pos.x;
  projectile_pos->y = player_pos.y;

  player_is_shooting = false;
}

bool check_out_of_bounds(Vector2 pos) { return false; }

void render() {
  draw_player();

  draw_guards();
  // draw_collision_boxes();
  draw_projectile();

  draw_enemies();
}

void handle_guard_hit() {
  for (uint8_t i = 0; i < guards.size(); i++) {
    if (check_guard_collision(guards[i].pos, projectile_pos)) {
      damage_guard(i);
      reset_projectile(player_pos, &projectile_pos);
    }
  }
}

void handle_enemy_hit() {
  for (uint8_t i = 0; i < enemies_pos.size(); i++) {
    if (check_enemy_collision(enemies_pos[i], projectile_pos)) {
      kill_enemy(i);
      reset_projectile(player_pos, &projectile_pos);
    }
  }
}

uint8_t enemy_mov_steps = 15;

void handle_enemy_movement() {
  if (enemy_mov_count >= enemy_mov_steps) {
    enemy_mov_count = 0;
    enemy_to_right = !enemy_to_right;

    for (uint8_t i = 0; i < enemies_pos.size(); i++) {
      enemies_pos[i] = Vector2Add(enemies_pos[i], {0, 1});
    }

    return;
  }

  Vector2 mov = {0, 0};
  mov.x = enemy_to_right ? 1 : -1;
  for (uint8_t i = 0; i < enemies_pos.size(); i++) {
    enemies_pos[i] = Vector2Add(enemies_pos[i], mov);
  }

  enemy_mov_count++;
}

void update_pos() {
  // Projectile
  if (player_is_shooting) {
    projectile_pos = Vector2Add(projectile_pos, {0, -1});
  }

  // Collisions
  handle_enemy_hit();
  handle_guard_hit();

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
  default:
    break;
  }

  if (IsKeyDown(KEY_LEFT)) {
    player_pos = Vector2Add(player_pos, {-1, 0});
  } else if (IsKeyDown(KEY_RIGHT)) {
    player_pos = Vector2Add(player_pos, {1, 0});
  }

  if (!player_is_shooting) {
    projectile_pos = player_pos;
  }
}

double last_update_time = 0;
double last_enemy_fall = 0;

const double projectile_interval = 0.03;
const double enemy_fall_interval = 0.5;

bool can_update_pos(double *time, const double interval) {
  double current_time = GetTime();

  if (current_time - *time >= interval) {
    *time = current_time;
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

    if (can_update_pos(&last_update_time, projectile_interval)) {
      update_pos();
    }

    if (can_update_pos(&last_enemy_fall, enemy_fall_interval)) {
      handle_enemy_movement();
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
  init_enemies_pos();
  init_guards();
}

void close_game() {}

int main(int argc, char *argv[]) {
  init_game();

  game_loop();

  close_game();
  return EXIT_SUCCESS;
}
