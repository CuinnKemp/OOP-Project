#include "ExpSpawner.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <random>

#include "ExpBall.h"

extern double xpos, ypos;

extern sf::RenderWindow window;

ExpSpawner::ExpSpawner() {
  expBallsCounter = 0;
  expBalls = new ExpBall*[0];
  spawnRate = 5;
  timer = 0;
  baseExpPoints = 10;
}

void ExpSpawner::spawnNewExp() {
  ExpBall** holdExpSpawner = this->expBalls;
  expBalls = new ExpBall*[expBallsCounter + 1];
  for (int i = 0; i < expBallsCounter; i++) {
    expBalls[i] = holdExpSpawner[i];
  }
  delete[] holdExpSpawner;

  expBalls[expBallsCounter] = new ExpBall;
  expBalls[expBallsCounter]->expPoints = baseExpPoints;
  expBalls[expBallsCounter]->sprite.setPosition(rand() % 2900 - 1450,
                                                rand() % 2900 - 1450);
  expBallsCounter++;
}

void ExpSpawner::spawnNewExp(int expPoint, double xpos, double ypos) {
  ExpBall** holdExpSpawner = this->expBalls;
  expBalls = new ExpBall*[expBallsCounter + 1];
  for (int i = 0; i < expBallsCounter; i++) {
    expBalls[i] = holdExpSpawner[i];
  }
  delete[] holdExpSpawner;

  expBalls[expBallsCounter] = new ExpBall;
  expBalls[expBallsCounter]->expPoints = expPoint;
  expBalls[expBallsCounter]->sprite.setPosition(xpos, ypos);
  expBallsCounter++;
}

void ExpSpawner::checkAlives() {
  for (int i = 0; i < expBallsCounter; i++) {
    if (!((*(expBalls[i])).updateExp())) {
      for (int j = i + 1; j < expBallsCounter; j++) {
        expBalls[j - 1] = expBalls[j];
      }
      expBallsCounter--;
    }
  }
}

void ExpSpawner::updateExps() {
  this->checkAlives();

  timer++;
  if (timer == 50) {
    this->spawnNewExp();
    timer = 0;
  }
}

ExpSpawner::~ExpSpawner() { delete[] expBalls; }