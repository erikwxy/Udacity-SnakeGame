#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include <mutex>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  AutoSnake auto_snake;
  SDL_Point food;
  std::vector<SDL_Point> bombs;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  std::mutex _mtx;

  void PlaceFood();
  void PlaceBombs();
  void Update();
  void UpdateAuto();
};

#endif