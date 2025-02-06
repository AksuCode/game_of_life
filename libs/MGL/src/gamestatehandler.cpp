#include "../include/gamestatehandler.hpp"

GameStateHandler::GameStateHandler() : running_(true), pause_(false), restart_(false), delay_increment_(50), delay_(200), mouse_pos_x_(0), mouse_pos_y_(0), left_click_on_(false), right_click_on_(false) {}

  void GameStateHandler::pollUserInput() {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running_ = false;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_p) {
          pause_ = !pause_;
        }
        if (event.key.keysym.sym == SDLK_r) {
          restart_ = !restart_;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          if (50 < delay_) {
            delay_ -= delay_increment_;
          }
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          if (delay_ < 1000) {
            delay_ += delay_increment_;
          }
        }
      }
      if (event.type == SDL_MOUSEMOTION) {
        mouse_pos_x_ = event.motion.x;
        mouse_pos_y_ = event.motion.y;
      }
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          left_click_on_ = true;
        }
        if (event.button.button == SDL_BUTTON_RIGHT) {
          right_click_on_ = true;
        }
      } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          left_click_on_ = false;
        }
        if (event.button.button == SDL_BUTTON_RIGHT) {
          right_click_on_ = false;
        }
      }
    }
  }

long long GameStateHandler::getDelay() { return delay_; }
bool GameStateHandler::getPauseState() { return pause_; }
bool GameStateHandler::getRunningState() { return running_; }
bool GameStateHandler::getMouseLeftClick() { return left_click_on_; }
bool GameStateHandler::getMouseRightClick() { return right_click_on_; }
std::pair<int, int> GameStateHandler::getMousePosition() { return std::pair(mouse_pos_x_, mouse_pos_y_); }