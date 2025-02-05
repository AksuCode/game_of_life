#ifndef MGL_WINDOW_H
#define MGL_WINDOW_H

#include <vector>
#include <iostream>

#include <SDL2/SDL.h>

class Window {
 public:
  Window(const char *window_title);

  ~Window();

  int getWindowWidth();

  int getWindowHeigth();

  /*
   * Creates a custom size window.
   * Parameters:
   * full_screen - set window to full screen mode
   * window_width - width of the window in screen coordinates
   * window_heigth - heigth of the window in screen coordinates
   * Returns 0 on succesful update and -1 on failure. Terminates program when window creation fails.
   */
  int createOrUpdateWindow(bool full_screen, int window_width, int window_heigth);

  /*
   * Clear window with the specified color.
   * Parameters:
   * r - red value of pixels
   * g - green value of pixels
   * b - blue value of pixels
   * Returns 0 on success and -1 on failure.
   */
  int clearWindow(Uint8 r, Uint8 g, Uint8 b);

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
  int drawRectangle(Uint8 r, Uint8 g, Uint8 b, int x, int y, int w, int h);

  /*
   * Update the window with the present renderer.
   */
  void updateWindow();

  /*
   * Creates a renderer. Terminates program on failure.
   */
  void createRenderer();

 private:
  const char *window_title;
  SDL_Window *window;
  SDL_Renderer *renderer;
  int window_width;
  int window_heigth;
  std::vector<SDL_Rect> rectangles;

  /*
   * Creates a window. Terminates program on failure.
   */
  void createWindow(int width, int heigth, uint32_t flags);
};

#endif