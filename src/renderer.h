#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "SDL.h"
#include "snake.h"
#include <SDL_image.h>

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(Snake const snake, SDL_Point const &food, std::vector<SDL_Point> const &bombs, AutoSnake const auto_snake);
  void UpdateWindowTitle(int score, int fps);

 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;

  SDL_Surface* food_sur = IMG_Load("../image/burger.bmp");
  SDL_Surface* bomb_sur = IMG_Load("../image/bomb.jpeg");

  SDL_Texture* food_tex;
  SDL_Texture* bomb_tex;

};

#endif