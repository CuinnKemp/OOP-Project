#ifndef BARRIER_H
#define BARRIER_H

#include <SFML/Graphics.hpp>
class Obstacle {
 public:
  double* coordinates = new double[2];
  sf::Sprite sprite;
  sf::Texture grass, desert, snow, dirt;

  Obstacle(int type, float posX,
           float posY);  // 1 - grass, 2 - desert, 3 - snow, 4 - dirt
  ~Obstacle();
};
#endif