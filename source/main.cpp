#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>
#include <vector>

// If compilation is done with emscripten
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

void log_to_file(void *userdata, int category, SDL_LogPriority priority, const char *message) {
  FILE *logFile = fopen("SDL_log.log", "a");
  if (logFile) {
    fprintf(logFile, "[%d] %s\n", priority, message);
    fflush(logFile);  // Ensure data is written
    fclose(logFile);
  }
}

class Window {
 public:
  Window(const char *window_title) {
    // Initialize SDL window
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Video initialization error: %s\n", SDL_GetError());
      exit(-1);
    }

    this->window_title = window_title;

    // Configure logging
    SDL_LogSetOutputFunction(log_to_file, NULL);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
  }

  ~Window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  int getWindowWidth() { return window_width; }

  int getWindowHeigth() { return window_heigth; }

  /*
   * Creates a custom size window.
   * Parameters:
   * full_screen - set window to full screen mode
   * window_width - width of the window in screen coordinates
   * window_heigth - heigth of the window in screen coordinates
   * Returns 0 on succesful update and -1 on failure. Terminates program when window creation fails.
   */
  int createOrUpdateWindow(bool full_screen, int window_width, int window_heigth) {
    if (window == nullptr) {
      if (full_screen) {
        createWindow(window_width, window_heigth, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
      } else {
        createWindow(window_width, window_heigth, SDL_WINDOW_OPENGL);
      }
    } else {
      if (full_screen) {
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
          SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window modification error: %s\n", SDL_GetError());
          return -1;
        }
      } else {
        SDL_SetWindowSize(window, window_width, window_heigth);
      }
    }
    SDL_GetWindowSize(window, &window_width, &window_heigth);

    return 0;
  }

  /*
   * Clear window with the specified color.
   * Parameters:
   * r - red value of pixels
   * g - green value of pixels
   * b - blue value of pixels
   * Returns 0 on success and -1 on failure.
   */
  int clearWindow(Uint8 r, Uint8 g, Uint8 b) {
    if (SDL_SetRenderDrawColor(renderer, r, g, b, 255) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer color setting error: %s\n", SDL_GetError());
      return -1;
    }
    if (SDL_RenderClear(renderer) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer clear error: %s\n", SDL_GetError());
      return -1;
    }

    return 0;
  }

  /*
   * Draw a rectangle on the window.
   * Parameters:
   * r - red value of rectangle pixels
   * g - green value of rectangle pixels
   * b - blue value of rectangle pixels
   * x - horizontal position of rectangle in window coordinates
   * y - vertical position of rectangle in window coordinates
   * w - width of rectangle in window coordinates
   * h - heigth of rectangle in window coordinates
   * Returns 0 on success and -1 on failure.
   */
  int drawRectangle(Uint8 r, Uint8 g, Uint8 b, int x, int y, int w, int h) {
    SDL_Rect new_rectangle = {x, y, w, h};
    rectangles.push_back(new_rectangle);
    if (SDL_SetRenderDrawColor(renderer, r, g, b, 255) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer color setting error: %s\n", SDL_GetError());
      return -1;
    }
    if (SDL_RenderFillRect(renderer, &rectangles.back()) < 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer fill error: %s\n", SDL_GetError());
      return -1;
    }

    return 0;
  }

  /*
   * Update the window with the present renderer.
   */
  void updateWindow() {
    SDL_RenderPresent(renderer);
    rectangles.clear();
  }

  /*
   * Creates a renderer. Terminates program on failure.
   */
  void createRenderer() {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create a renderer: %s\n", SDL_GetError());
      exit(-1);
    }
  }

 private:
  const char *window_title;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  int window_width = 0;
  int window_heigth = 0;
  std::vector<SDL_Rect> rectangles;

  /*
   * Creates a window. Terminates program on failure.
   */
  void createWindow(int width, int heigth, uint32_t flags) {
    window = SDL_CreateWindow(window_title,             // window title
                              SDL_WINDOWPOS_UNDEFINED,  // Window position x
                              SDL_WINDOWPOS_UNDEFINED,  // Window position y
                              width,                    // width, in pixels
                              heigth,                   // height, in pixels
                              flags                     // flags - see below
    );
    if (window == NULL) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
      exit(-1);
    }
  }
};

class GameStateHandler {
 public:
  GameStateHandler() {}

  void pollUserInput() {
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

  long long getDelay() { return delay_; }
  bool getPauseState() { return pause_; }
  bool getRunningState() { return running_; }
  bool getMouseLeftClick() { return left_click_on_; }
  bool getMouseRightClick() { return right_click_on_; }
  std::pair<int, int> getMousePosition() { return std::pair(mouse_pos_x_, mouse_pos_y_); }

 private:
  SDL_Event event;
  bool running_ = true;
  bool pause_ = false;
  bool restart_ = false;
  const long long delay_increment_ = 50;
  long long delay_ = 200;
  int mouse_pos_x_ = 0;
  int mouse_pos_y_ = 0;
  bool left_click_on_ = false;
  bool right_click_on_ = false;
};

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
  std::vector<std::pair<int, int>> initial_live_cells = {std::pair(50, 50), std::pair(51, 49),
                                                         std::pair(51, 48), std::pair(50, 48),
                                                         std::pair(49, 48)};
  grid.setAliveCells(initial_live_cells);
  while (state.getRunningState()) {
    mainloop();
  }
#endif

  return 0;
}