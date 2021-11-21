#include "game.h"
#include <iostream>
#include <mutex>
#include "SDL.h"
#include <thread>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      auto_snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
  PlaceBombs();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  bool auto_update = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, this->snake);

    std::thread t1 = std::thread(&Game::UpdateAuto, this);
    std::thread t2 = std::thread(&Game::Update, this);

    t1.join();
    t2.join();
   
    renderer.Render(snake, food, bombs, auto_snake);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::PlaceBombs() {
  int x, y;
  if (score % 2 == 0){
    SDL_Point bomb;
    bombs.emplace_back(bomb);
  }

  for (auto &bomb: bombs){
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item, or occupied by food,
    // before placing food.
    if (!snake.SnakeCell(x, y) && x!=food.x && y!=food.y) {
      bomb.x = x;
      bomb.y = y;
    }
  }

}

void Game::Update() {
  std::lock_guard<std::mutex> lck(_mtx);
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if snake meets bomb
  for (auto &bomb: bombs){
    if (bomb.x == new_x && bomb.y == new_y) {
      snake.alive = false;
      return;
    }
  }

  // Check if snake meet another snake
  if (static_cast<int>(auto_snake.head_x) == new_x &&
      static_cast<int>(auto_snake.head_y) == new_y) {
      snake.alive = false;
      return;
  }

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    PlaceBombs();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

void Game::UpdateAuto() {
  std::lock_guard<std::mutex> lck(_mtx);
  auto_snake.Update();

  int new_x = static_cast<int>(auto_snake.head_x);
  int new_y = static_cast<int>(auto_snake.head_y);

  // Check if snake meets bomb
  for (auto &bomb: bombs){
    if (bomb.x == new_x && bomb.y == new_y) {
      PlaceBombs();
      score--;
    }
  }

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score--;
    PlaceFood();
    
    // Grow snake and increase speed.
    auto_snake.GrowBody();
    auto_snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }