#include "./gridoflife.hpp"

Cell::Cell() : alive_(false) {}

void Cell::setCellAlive() { alive_ = true; }
void Cell::setCellDead() { alive_ = false; }
bool Cell::cellIsAlive() { return alive_; }

GridOfLife::GridOfLife(int width, int heigth) : width_(width), heigth_(heigth) {
    grid_ = std::vector<std::vector<Cell>>(heigth, std::vector<Cell>(width));
}

int GridOfLife::getGridWidth() { return width_; }
int GridOfLife::getGridHeigth() { return heigth_; }

void GridOfLife::setAliveCells(std::vector<std::pair<int, int>> &alive_cells) {
    for (auto it = alive_cells.begin(); it != alive_cells.end(); it++) {
        int i = (*it).first;
        int j = (*it).second;
        grid_[j][i].setCellAlive();
    }
}

std::vector<std::pair<int, int>> GridOfLife::getAliveCells() {
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

void GridOfLife::setDeadCells(std::vector<std::pair<int, int>> &dead_cells) {
    for (auto it = dead_cells.begin(); it != dead_cells.end(); it++) {
        int i = (*it).first;
        int j = (*it).second;
        grid_[j][i].setCellDead();
    }
}

void GridOfLife::killAllCells() {
    for (int j = 0; j < heigth_; j++) {
        for (int i = 0; i < width_; i++) {
        grid_[j][i].setCellDead();
        }
    }
}

void GridOfLife::setSpecificCellAlive(int x_pos, int y_pos) { grid_[y_pos][x_pos].setCellAlive(); }
void GridOfLife::setSpecificCellDead(int x_pos, int y_pos) { grid_[y_pos][x_pos].setCellDead(); }

void GridOfLife::iterateGridOfLife() {
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
}