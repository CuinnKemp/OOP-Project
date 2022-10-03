#ifndef BEAST_H
#define BEAST_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <random>

#include "Arrow.h"
#include "Enemy.h"

class Beast : public Enemy {
 public:
  int arrowCounter;
  int fireCounter;
  Arrow* Arrows;
  sf::RectangleShape sprite;
  sf::Texture walkDown1, walkDown2, walkDown3, walkDown4, walkUp1, walkUp2,
      walkUp3, walkUp4, walkLeft1, walkLeft2, walkLeft3, walkLeft4, walkRight1,
      walkRight2, walkRight3, walkRight4;

  int direction;  // 1 = left, 2 = right, 3 = up, 4 = down
  bool isMoving;
  int animationCount;

  Beast();

  ~Beast();

  void attack();

  void drawArrows();

  void movement();

  void updatePosition();

  bool updateEnemy();
};
#endif  // BEAST_H