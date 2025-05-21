#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

const int GRID_SIZE = 8;
const int MAP_WIDTH = 600; // Ancho del mapa
const int CELL_SIZE = MAP_WIDTH / GRID_SIZE; // 600 / 8 = 75 p�xeles por celda
const int MAP_HEIGHT = GRID_SIZE * CELL_SIZE; // 8 * 75 = 600 p�xeles
const int UI_WIDTH = 270;
const int WINDOW_WIDTH = MAP_WIDTH + UI_WIDTH; // 600 + 350 = 950 p�xeles
const int WINDOW_HEIGHT = MAP_HEIGHT; // 600 p�xeles

#endif