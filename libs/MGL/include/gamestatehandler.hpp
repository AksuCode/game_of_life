#ifndef MGL_GAMESTATEHANDLER_HPP
#define MGL_GAMESTATEHANDLER_HPP

#include <vector>

#include <SDL2/SDL.h>

class GameStateHandler {
 public:
  GameStateHandler();

  void pollUserInput();

  long long getDelay();
  bool getPauseState();
  bool getRunningState();
  bool getMouseLeftClick();
  bool getMouseRightClick();
  std::pair<int, int> getMousePosition();

 private:
  SDL_Event event;
  bool running_;
  bool pause_;
  bool restart_;
  const long long delay_increment_;
  long long delay_;
  int mouse_pos_x_;
  int mouse_pos_y_;
  bool left_click_on_;
  bool right_click_on_ ;
};

#endif