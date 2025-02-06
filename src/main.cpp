#include <chrono>
#include <iostream>
#include <vector>

#include "MGL/include/window.hpp"
#include "MGL/include/gamestatehandler.hpp"

// If compilation is done with emscripten
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

class Cell {
 public:
  Cell() {}

  void setCellAlive() { alive_ = true; }
  void setCellDead() { alive_ = false; }
  bool cellIsAlive() { return alive_; }

 private:
  bool alive_ = false;
};

class GridOfLife {
 public:
  GridOfLife(int width, int heigth) : width_(width), heigth_(heigth) {
    grid_ = std::vector<std::vector<Cell>>(heigth, std::vector<Cell>(width));
  }

  int getGridWidth() { return width_; }
  int getGridHeigth() { return heigth_; }

  void setAliveCells(std::vector<std::pair<int, int>> &alive_cells) {
    for (auto it = alive_cells.begin(); it != alive_cells.end(); it++) {
      int i = (*it).first;
      int j = (*it).second;
      grid_[j][i].setCellAlive();
    }
  }

  std::vector<std::pair<int, int>> getAliveCells() {
    std::vector<std::pair<int, int>> alive_cells;
    for (int j = 0; j < heigth_; j++) {
      for (int i = 0; i < width_; i++) {
        if (grid_[j][i].cellIsAlive()) {
          alive_cells.push_back(std::pair(i, j));
        }
      }
    }
    return alive_cells;
  }

  void setDeadCells(std::vector<std::pair<int, int>> &dead_cells) {
    for (auto it = dead_cells.begin(); it != dead_cells.end(); it++) {
      int i = (*it).first;
      int j = (*it).second;
      grid_[j][i].setCellDead();
    }
  }

  void killAllCells() {
    for (int j = 0; j < heigth_; j++) {
      for (int i = 0; i < width_; i++) {
        grid_[j][i].setCellDead();
      }
    }
  }

  void setSpecificCellAlive(int x_pos, int y_pos) { grid_[y_pos][x_pos].setCellAlive(); }
  void setSpecificCellDead(int x_pos, int y_pos) { grid_[y_pos][x_pos].setCellDead(); }

  void iterateGridOfLife() {
    std::vector<std::pair<int, int>> to_be_alive_cells;
    std::vector<std::pair<int, int>> to_be_dead_cells;
    for (int j = 1; j < heigth_ - 1; j++) {
      for (int i = 1; i < width_ - 1; i++) {
        bool cell_is_alive = grid_[j][i].cellIsAlive();
        int alive_neigbors_count = 0;
        for (int k = j - 1; k <= j + 1; k++) {
          for (int l = i - 1; l <= i + 1; l++) {
            if (k == j && l == i) {
              continue;
            }
            alive_neigbors_count += grid_[k][l].cellIsAlive();
          }
        }
        if (!cell_is_alive) {
          if (3 == alive_neigbors_count) {
            to_be_alive_cells.push_back(std::pair(i, j));
          }
          continue;
        }
        std::cout << "(" << i << ", " << j << ") alive neighbors: " << alive_neigbors_count
                  << std::endl;
        if (alive_neigbors_count < 2) {
          to_be_dead_cells.push_back(std::pair(i, j));
        } else if ((2 == alive_neigbors_count || 3 == alive_neigbors_count)) {
          to_be_alive_cells.push_back(std::pair(i, j));
        } else if (3 < alive_neigbors_count) {
          to_be_dead_cells.push_back(std::pair(i, j));
        }
      }
    }

    /// TODO ///
    // BORDERS NEED TO BE CHECKED///
    /// ATM JUST IGNORED ///

    setAliveCells(to_be_alive_cells);
    setDeadCells(to_be_dead_cells);

    for (auto it = to_be_alive_cells.begin(); it != to_be_alive_cells.end(); it++) {
      std::cout << "(" << (*it).first << ", " << (*it).second << ") ";
    }
    std::cout << std::endl;
  }

 private:
  int width_;
  int heigth_;
  std::vector<std::vector<Cell>> grid_;
};

// Local global variables
static Window window("Wasm Window");
static GridOfLife grid(100, 100);
static GameStateHandler state;

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