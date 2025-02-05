#include "../include/window.hpp"

void log_to_file(void *userdata, int category, SDL_LogPriority priority, const char *message) {
  FILE *logFile = fopen("./log/SDL_log.log", "a");
  if (logFile) {
    fprintf(logFile, "[%d] %s\n", priority, message);
    fflush(logFile);  // Ensure data is written
    fclose(logFile);
  } else {
    std::cerr << "SDL_log.log file failed to open for write." << std::endl;
    std::cerr << "Perhaps ./log directory missing." << std::endl;
    exit(-1);
  }
}

Window::Window(const char *window_title): window(nullptr), renderer(nullptr), window_width(0), window_heigth(0) {
  // Initialize SDL library
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Video initialization error: %s\n", SDL_GetError());
    exit(-1);
  }

  this->window_title = window_title;

  // Configure logging
  SDL_LogSetOutputFunction(log_to_file, NULL);
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
}

Window::~Window() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int Window::getWindowWidth() { return window_width; }

int Window::getWindowHeigth() { return window_heigth; }

int Window::createOrUpdateWindow(bool full_screen, int window_width, int window_heigth) {
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

int Window::clearWindow(Uint8 r, Uint8 g, Uint8 b) {
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

int Window::drawRectangle(Uint8 r, Uint8 g, Uint8 b, int x, int y, int w, int h) {
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


void Window::updateWindow() {
  SDL_RenderPresent(renderer);
  rectangles.clear();
}

void Window::createRenderer() {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create a renderer: %s\n", SDL_GetError());
    exit(-1);
  }
}

void Window::createWindow(int width, int heigth, uint32_t flags) {
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