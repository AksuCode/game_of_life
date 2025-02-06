#include <chrono>
#include <iostream>
#include <vector>

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
#ifdef __EMSCRIPTEN__
static Window window("Wasm Window", NULL);
#else
static Window window("Wasm Window", log_to_file);
#endif
static GridOfLife grid(100, 100);
static GameStateHandler state;
static std::chrono::steady_clock::time_point gol_iteration_tp = std::chrono::steady_clock::now();

void renderFrame(std::vector<std::pair<int, int>> &alive_cells) {
  window.clearWindow(0, 0, 0);
  int cell_width = 10;
  int cell_heigth = 10;
  for (auto it = alive_cells.begin(); it != alive_cells.end(); it++) {
    int grid_x_pos_to_window = cell_width * (*it).first;
    int grid_y_pos_to_window = cell_heigth * (*it).second;
    if (window.drawRectangle(255, 255, 255, grid_x_pos_to_window, grid_y_pos_to_window, cell_width,
                             cell_heigth) < 0) {
    }
  }
  window.updateWindow();
}

/*
static void mainloop() {
  std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();

  // Iterate game of life
  grid.iterateGridOfLife();

  // Handle frame rendering and events
  std::chrono::steady_clock::time_point tp2 = std::chrono::steady_clock::now();
  long long elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
  const long long delay_ms = state.getDelay();
  while (delay_ms - elapsed_ms > 0 || state.getPauseState()) {
    std::vector<std::pair<int, int>> alive_cells = grid.getAliveCells();
    renderFrame(alive_cells);
    state.pollUserInput();
    if (state.getMouseLeftClick()) {
      auto [x_pos, y_pos] = state.getMousePosition();
      grid.setSpecificCellAlive(x_pos / 10, y_pos / 10);
    } else if (state.getMouseRightClick()) {
      auto [x_pos, y_pos] = state.getMousePosition();
      grid.setSpecificCellDead(x_pos / 10, y_pos / 10);
    }

    tp2 = std::chrono::steady_clock::now();
    elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
  }
}
*/

static void mainloop() {
  std::chrono::steady_clock::time_point current_tp = std::chrono::steady_clock::now();
  long long elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_tp - gol_iteration_tp).count();
  const long long delay_ms = state.getDelay();

  if (!(delay_ms - elapsed_ms > 0 || state.getPauseState())) {
    gol_iteration_tp = std::chrono::steady_clock::now();
    // Iterate game of life
    grid.iterateGridOfLife();
  }

  // Handle frame rendering and events
  std::vector<std::pair<int, int>> alive_cells = grid.getAliveCells();
  renderFrame(alive_cells);
  state.pollUserInput();
  if (state.getMouseLeftClick()) {
    auto [x_pos, y_pos] = state.getMousePosition();
    grid.setSpecificCellAlive(x_pos / 10, y_pos / 10);
  } else if (state.getMouseRightClick()) {
    auto [x_pos, y_pos] = state.getMousePosition();
    grid.setSpecificCellDead(x_pos / 10, y_pos / 10);
  }
}

int main(int argc, char *argv[]) {
  /*
   * Check if compilation is done with emscripten.
   * If yes then use modified main loop configuration for the program.
   * Else use default main loop configuration.
   */
  window.createOrUpdateWindow(false, 1000, 1000);
  window.createRenderer();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainloop, 0, 1);
#else
  while (state.getRunningState()) {
    mainloop();
  }
#endif

  return 0;
}