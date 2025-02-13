#include <chrono>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "MGL/include/window.hpp"
#include "MGL/include/gamestatehandler.hpp"

#include "gridoflife.hpp"

// If compilation is done with emscripten
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

// Logging function:
void log_to_file(void *userdata, int category, SDL_LogPriority priority, const char *message) {
  FILE *logFile = fopen("./log/SDL_log.log", "a");
  if (logFile) {
    fprintf(logFile, "[%d] %s\n", priority, message);
    fflush(logFile);  // Ensure data is written
    fclose(logFile);
  } else {
    std::cerr << "SDL_log.log file failed to open for write." << std::endl;
    std::cerr << "Perhaps ./log directory missing." << std::endl;
  }
}

// Local global variables
static int window_width = 1000;
static int window_heigth = 1000;
static int cell_width = 10;
static int cell_heigth = 10;
static Window *window;
static GridOfLife *grid;
static GameStateHandler *state;
static std::chrono::steady_clock::time_point gol_iteration_tp = std::chrono::steady_clock::now();

void renderFrame(std::vector<std::pair<int, int>> &alive_cells) {
  window->clearWindow(0, 0, 0);
  for (auto it = alive_cells.begin(); it != alive_cells.end(); it++) {
    int grid_x_pos_to_window = cell_width * (*it).first;
    int grid_y_pos_to_window = cell_heigth * (*it).second;
    if (window->drawRectangle(255, 255, 255, grid_x_pos_to_window, grid_y_pos_to_window, cell_width,
                             cell_heigth) < 0) {
    }
  }
  window->updateWindow();
}

static void mainloop() {
  std::chrono::steady_clock::time_point current_tp = std::chrono::steady_clock::now();
  long long elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_tp - gol_iteration_tp).count();
  const long long delay_ms = state->getDelay();

  if (!(delay_ms - elapsed_ms > 0 || state->getPauseState())) {
    gol_iteration_tp = std::chrono::steady_clock::now();
    // Iterate game of life
    grid->iterateGridOfLife();
  }

  // Handle frame rendering and events
  std::vector<std::pair<int, int>> alive_cells = grid->getAliveCells();
  renderFrame(alive_cells);
  state->pollUserInput();
  if (state->getMouseLeftClick()) {
    auto [x_pos, y_pos] = state->getMousePosition();
    int cell_x_pos = x_pos / cell_width;
    int cell_y_pos = y_pos / cell_heigth;
    if (cell_x_pos < grid->getGridWidth() && cell_y_pos < grid->getGridHeigth()) {
      grid->setSpecificCellAlive(x_pos / cell_width, y_pos / cell_heigth);
    }
  } else if (state->getMouseRightClick()) {
    auto [x_pos, y_pos] = state->getMousePosition();
    int cell_x_pos = x_pos / cell_width;
    int cell_y_pos = y_pos / cell_heigth;
    if (cell_x_pos < grid->getGridWidth() && cell_y_pos < grid->getGridHeigth()) {
      grid->setSpecificCellDead(x_pos / cell_width, y_pos / cell_heigth);
    }
  }
}

int main(int argc, char *argv[]) {
/*
  * Check if compilation is done with emscripten.
  * If yes then use modified main loop configuration for the program.
  * Else use default main loop configuration.
  */
#ifdef __EMSCRIPTEN__
  // Initialization:
  window = new Window("Game of life", NULL);
  grid = new GridOfLife(100, 100);
  state = new GameStateHandler();

  window->createOrUpdateWindow(false, 1000, 1000);
  window->createRenderer();

  // Game loop
  emscripten_set_main_loop(mainloop, 0, 1);
#else
  bool fullscreen = false;
  int width = window_width;
  int heigth = window_heigth;
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strncmp(argv[i], "--help", 20) == 0) {
        std::cout << "Here are the the possible command:" << std::endl;
        std::cout << "  --window-size <width> <heigth>: Make the application window specific size. Give <width> <heigth> in base 10." << std::endl;
        std::cout << "  --full: CURRENTLY BROKEN UNAVAILABLE! Make the application fullscreen. Prioritized over --window-size" << std::endl;
        return 1;
      }
      else if (strncmp(argv[i], "--full", 20) == 0) {
        //fullscreen = true;    // CURRENTLY BROKEN UNAVAILABLE!
        std::cout << "Fullscreen feature currently broken. Using default window configuration." << std::endl;
      }
      else if (strncmp(argv[i], "--window-size", 20) == 0) {
        if (i + 2 < argc) {
          width = strtol(argv[i + 1], NULL, 10);
          heigth = strtol(argv[i + 2], NULL, 10);
          if (width == 0 || heigth == 0) {return 1;}
          i += 2;
        } else {
          return 1;
        }
      }
      else {
        std::cout << "Wrong command. Use --help to see options." << std::endl;
        return 1;
      }
    }
  }

  // Initialization:
  window = new Window("Game of life", log_to_file);
  window->createOrUpdateWindow(fullscreen, width, heigth);
  window_width = window->getWindowWidth();
  window_heigth = window->getWindowHeigth();
  window->createRenderer();
  grid = new GridOfLife(window_width / cell_width, window_heigth / cell_heigth);
  state = new GameStateHandler();

  // Game loop:
  while (state->getRunningState()) {
    mainloop();
  }
#endif

  // Free memory
  delete window;
  delete grid;
  delete state;

  return 0;
}