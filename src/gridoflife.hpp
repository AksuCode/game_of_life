#ifndef GRIDOFLIFE_HPP
#define GRIDOFLIFE_HPP

#include <vector>
#include <iostream>

class Cell {
 public:
  Cell();

  void setCellAlive();
  void setCellDead();
  bool cellIsAlive();

 private:
  bool alive_;
};

class GridOfLife {
 public:
  GridOfLife(int width, int heigth);

  int getGridWidth();
  int getGridHeigth();

  void setAliveCells(std::vector<std::pair<int, int>> &alive_cells);

  std::vector<std::pair<int, int>> getAliveCells();

  void setDeadCells(std::vector<std::pair<int, int>> &dead_cells);

  void killAllCells();

  void setSpecificCellAlive(int x_pos, int y_pos);
  void setSpecificCellDead(int x_pos, int y_pos);

  void iterateGridOfLife();

 private:
  int width_;
  int heigth_;
  std::vector<std::vector<Cell>> grid_;
};

#endif