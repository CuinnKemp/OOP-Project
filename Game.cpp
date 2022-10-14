// g++ Game.cpp ResourceManager.cpp Player.cpp UIManager.cpp Enemy.cpp
// Enemies.cpp Beast.cpp Slime.cpp Obstacle.cpp ObstacleGenerator.cpp Arrow.cpp
// PowerUp.cpp SpinningBlade.cpp ExpBall.cpp ExpContainer.cpp ExpSpawner.cpp
// PlayerArrow.cpp PlayerArrowSpawner.cpp -lsfml-graphics -lsfml-window
// -lsfml-system

#include <stdlib.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "Arrow.h"
#include "Beast.h"
#include "Enemies.h"
#include "Enemy.h"
#include "ExpBall.h"
#include "ExpContainer.h"
#include "ExpSpawner.h"
#include "Obstacle.h"
#include "ObstacleGenerator.h"
#include "Player.h"
#include "PlayerArrow.h"
#include "PlayerArrowSpawner.h"
#include "ResourceManager.h"
#include "Slime.h"
#include "SpinningBlade.h"
#include "UIManager.h"
using namespace std;

// coordinates for the player
double xpos, ypos;
std ::string gameState;

// Window and Desktop Settings for the Viewport
double width = sf::VideoMode::getDesktopMode().width;
double height = sf::VideoMode::getDesktopMode().height;
sf::RenderWindow window(sf::VideoMode(width, height), "GAME");

// Initialising Objects
ObstacleGenerator og;
ExpSpawner E1;
ResourceManager resourceManager;
Player P1(0, 0, width / 2, height / 2, &window);
const sf::Time TimePerFrame = sf::seconds(1.f / 90.f);
bool showQuitGameDialouge;
bool showSettingsPage;
bool isGameChanging;
bool waiting;
bool shouldLoadGame;

int saveGame(int health, int level, int currentExp, float time) {
  ofstream saveFile("saveGame.save");
  if (saveFile.is_open()) {
    saveFile << health << " " << level << " " << currentExp << " " << time;
    saveFile.close();
  } else {
    return 0;
  }
  return 1;
}

int loadGame() {
  P1.resetPlayer();
  int number;
  ifstream saveFile("saveGame.save");
  if (saveFile.is_open()) {
    for (int i = 0; i < 4; i++) {
      saveFile >> number;
      if (i == 0) {
        P1.health = number;
      } else if (i == 1) {
        P1.level = number;
      } else if (i == 2) {
        P1.currentExp = number;
      } else {
        P1.savedTime = number;
      }
    }
    saveFile.close();
  } else {
    return 0;
  }
  return 1;
}

void encryptSaveGame() {
  char fileName[30], ch;
  std::fstream fps, fpt;

  std::ofstream myfile;
  fps.open("saveGame.save", std::fstream::in);
  fpt.open("tmpSave.save", std::fstream::out);
  while (fps >> std::noskipws >> ch) {
    ch = ch + 100;
    fpt << ch;
  }
  fps.close();
  fpt.close();
}
void decryptSaveGame() {
  char fileName[30], ch;
  std::fstream fps, fpt;

  std::ofstream myfile;
  fps.open("saveGame.save", std::fstream::out);
  fpt.open("tmpSave.save", std::fstream::in);
  while (fpt >> std::noskipws >> ch) fps << ch;
  fps.close();
  fpt.close();
  std::cout << "\nFile '"
            << "saveGame.save"
            << "' Encrypted Successfully!";
  std::cout << std::endl;
  fps.open("saveGame.save", std::fstream::out);
  fpt.open("tmpSave.save", std::fstream::in);
  while (fpt >> std::noskipws >> ch) {
    ch = ch - 100;
    fps << ch;
  }
  fps.close();
  fpt.close();
  std::cout << "\nFile '"
            << "saveGame.save"
            << "' Decrypted Successfully!";
  std::cout << std::endl;
}

void quitGameDialouge() {
  sf::Sprite dialougeBox, yesButton, noButton;
  dialougeBox.setTexture(resourceManager.dialougeBoxTex);
  dialougeBox.setScale(1, 1);

  yesButton.setTexture(resourceManager.yesButtonTex);
  yesButton.setScale(2, 2);

  noButton.setTexture(resourceManager.noButtonTex);
  noButton.setScale(2, 2);

  if (gameState == "gameLoop") {
    dialougeBox.setPosition(-285 + P1.sprite.getPosition().x,
                            -75 + P1.sprite.getPosition().y);
    yesButton.setPosition(-150 + P1.sprite.getPosition().x,
                          -10 + P1.sprite.getPosition().y);
    noButton.setPosition(0 + P1.sprite.getPosition().x,
                         -10 + P1.sprite.getPosition().y);
  } else {
    dialougeBox.setPosition(-285, -75);
    yesButton.setPosition(-150, -10);
    noButton.setPosition(0, -10);
  }

  // Checks if mouse is hovering over the yes button
  if (yesButton.getGlobalBounds().contains(sf::Vector2f(
          window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the yes button
    yesButton.setPosition(-150 + P1.sprite.getPosition().x,
                          -9 + P1.sprite.getPosition().y);
    yesButton.setTexture(resourceManager.yesButtonSelectedTex, true);
    // Checks if the yes button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      if (gameState == "gameLoop") {
        saveGame(P1.health, P1.level, P1.currentExp,
                 P1.clock.getElapsedTime().asSeconds());
      }
      window.close();
    }
  }
  // Checks if mouse is hovering over the yes button
  if (noButton.getGlobalBounds().contains(sf::Vector2f(
          window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the yes button
    noButton.setPosition(0 + P1.sprite.getPosition().x,
                         -9 + P1.sprite.getPosition().y);
    noButton.setTexture(resourceManager.noButtonSelectedTex, true);
    // Checks if the no button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      showQuitGameDialouge = false;
    }
  }
  // Draws quit game dialouge UI elements
  window.draw(dialougeBox);
  window.draw(yesButton);
  window.draw(noButton);
}

void gameLoop() {
  // Initialising Objects for Main Game
  SpinningBlade b1(0);
  sf::Clock clk;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;
  sf::Sprite backgroundMap, mapExtras;
  UIManager UI(0, 0, width / 2, height / 2, &window);

  // Setings for Map and Extras
  backgroundMap.scale(2, 2);
  backgroundMap.setTexture(resourceManager.mapImage);
  backgroundMap.setPosition(-3586, -3616);

  mapExtras.scale(2, 2);
  mapExtras.setTexture(resourceManager.extrasImage);
  mapExtras.setPosition(-2048, -2048);

  isGameChanging = false;

  // Main game Loop
  while (window.isOpen()) {
    // Initialising Enemies & Player Arrows
    Enemies a1;
    PlayerArrowSpawner pA(&a1);

    // While the Window is open
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }

    // Sets Background Map, Spawns Obstacles and EXP
    backgroundMap.setOrigin(-512, -512);

    for (int i = 0; i <= 237; i++) {
      og.spawnNewObstacle();
    }
    for (int i = 0; i < 25; i++) {
      E1.spawnNewExp();
    }
    cout << P1.health << endl;
    // While the Player is Alive and the window is still open
    while (P1.isAlive() && window.isOpen()) {
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
      }

      sf::Time dt = clk.restart();
      timeSinceLastUpdate += dt;
      while (P1.isAlive() && window.isOpen() &&
             timeSinceLastUpdate > TimePerFrame) {
        while (window.pollEvent(event)) {
          if (event.type == sf::Event::Closed) window.close();
        }

        if (shouldLoadGame) {
          loadGame();
          shouldLoadGame = false;
        }

        timeSinceLastUpdate -= TimePerFrame;

        // Draws Background Map
        window.draw(backgroundMap);
        og.updateObstacles();

        // Adding Collision to Objects
        for (int i = 0; i < og.obstacleCounter; i++) {
          float playerX = P1.sprite.getPosition().x + 20;
          float playerY = P1.sprite.getPosition().y + 20;
          float obstacleX = og.obstacles[i]->sprite.getPosition().x - 1888;
          float obstacleY = og.obstacles[i]->sprite.getPosition().y - 1888;
          if (abs(playerX - obstacleX) <= 50 &&
              abs(playerY - obstacleY) <= 50) {
            xpos = P1.oldXpos;
            ypos = P1.oldYpos;
          }
        }

        // Drawing Foliage on the Map
        window.draw(mapExtras);

        // update command for enemies
        a1.updateEnemies();

        // Spawning Player Arrows, firing them at enemies
        pA.fireCounter = pA.fireCounter + 2;
        // Temp test to see how fire rate affects gameplay
        if (pA.fireCounter >= (1 / P1.clock.getElapsedTime().asSeconds() +
                               (100 - P1.clock.getElapsedTime().asSeconds()))) {
          pA.attack();
          pA.fireCounter = 0;
        }
        pA.drawArrows();

        // Drawing Player on the map
        P1.DrawPlayer(&window);

        // update command for Abilities
        b1.updateAbility();
        b1.hitEnemy(&a1);

        // Updating Exp, UI and Map
        E1.updateExps();
        UI.DrawUIManager(&window);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
          showQuitGameDialouge = true;
        }

        // Checks if the quit button has been clicked
        if (showQuitGameDialouge == true) {
          quitGameDialouge();
        }

        window.display();
        window.clear(sf::Color::White);
      }
    }

    // Death Screen if Player runs out of health
    P1.resetPlayer();
    UI.resetUI();
    E1.deleteExpBalls();
    a1.deleteEnemies();
    window.clear(sf::Color::Green);
    sf::RectangleShape deathScreen;
    sf::Texture deathText;
    deathText.loadFromFile("deathScreen.png");
    deathScreen.setSize(sf::Vector2f(1920 / 2, 1080 / 2));
    deathScreen.setPosition(sf::Vector2f(-1920 / 4, -1080 / 4));
    deathScreen.setTexture(&deathText);
    window.draw(deathScreen);
    window.display();

    // Waiting for Player Response on Death Screen
    waiting = true;
    while (waiting == true && window.isOpen()) {
      sf::Event eventInner;
      while (window.pollEvent(eventInner)) {
        if (eventInner.type == sf::Event::Closed) window.close();
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
        waiting = 0;
        gameLoop();
      }
    }
  }
}

void mainMenu() {
  // Setings for Main Menu UI
  sf::Sprite playButton, loadButton, settingsButton, quitButton, background,
      menuTitle, settingsPage, exitButton;
  menuTitle.setTexture(resourceManager.menuTitleTex);
  background.setTexture(resourceManager.backgroundTex);
  playButton.setTexture(resourceManager.playButtonTex, true);
  loadButton.setTexture(resourceManager.loadButtonTex);
  settingsButton.setTexture(resourceManager.settingsButtonTex);
  quitButton.setTexture(resourceManager.quitButtonTex);

  settingsPage.setTexture(resourceManager.settingsPageTex);
  settingsPage.setScale(3, 3);
  settingsPage.setPosition(-325, -215);
  exitButton.setScale(2, 2);
  exitButton.setPosition(-330, -220);
  exitButton.setTexture(resourceManager.quitButtonTex);

  background.setScale(6, 6);
  background.setPosition(-480, -270);
  menuTitle.setScale(6, 6);
  menuTitle.setPosition(-240, -190);
  playButton.setScale(6, 6);
  playButton.setPosition(-190, 20);
  loadButton.setScale(6, 6);
  loadButton.setPosition(-90, 20);
  settingsButton.setScale(6, 6);
  settingsButton.setPosition(10, 20);
  quitButton.setScale(6, 6);
  quitButton.setPosition(110, 20);

  // std::cout << window.getSize().x / 3024 << std::endl;

  // Checks if mouse is hovering over the play button

  if (playButton.getGlobalBounds().contains(sf::Vector2f(
          window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the play button
    playButton.setPosition(-196, 14);
    playButton.setTexture(resourceManager.playButtonSelectedTex, true);

    // Checks if the play button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      gameState = "gameLoop";
      isGameChanging = true;
    }
  }
  // Checks if mouse is hovering over the quit button
  else if (quitButton.getGlobalBounds().contains(sf::Vector2f(
               window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the quit button
    quitButton.setPosition(104, 14);
    quitButton.setTexture(resourceManager.quitButtonSelectedTex, true);
    // Checks if the quit button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      showQuitGameDialouge = true;
    }
  }
  // Checks if mouse is hovering over the settings button
  else if (settingsButton.getGlobalBounds().contains(sf::Vector2f(
               window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the settings button
    settingsButton.setPosition(4, 14);
    settingsButton.setTexture(resourceManager.settingsButtonSelectedTex, true);
    // Checks if the settings button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      showSettingsPage = true;
    }

  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    showQuitGameDialouge = true;
  }
  // Checks if mouse is hovering over the load button
  else if (loadButton.getGlobalBounds().contains(sf::Vector2f(
               window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
    // Sets selected texture for the load button
    loadButton.setPosition(-96, 14);
    loadButton.setTexture(resourceManager.loadButtonSelectedTex, true);
    // Checks if the load button is clicked
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      shouldLoadGame = true;
      gameState = "gameLoop";
      isGameChanging = true;
    }

  } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
    showQuitGameDialouge = true;
  }

  // Draws UI elements
  window.draw(background);
  window.draw(menuTitle);
  window.draw(playButton);
  window.draw(loadButton);
  window.draw(settingsButton);
  window.draw(quitButton);

  // Checks if the settings button has been clicked
  if (showSettingsPage == true) {
    // Checks if mouse is hovering over the exit button
    if (exitButton.getGlobalBounds().contains(sf::Vector2f(
            window.mapPixelToCoords(sf::Mouse::getPosition(window))))) {
      // Sets selected texture for the exit button
      exitButton.setPosition(-332, -222);
      exitButton.setTexture(resourceManager.quitButtonSelectedTex, true);
      // Checks if the exit button is clicked
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        showSettingsPage = false;
      }
    }
    // Draws settings page UI elements
    window.draw(settingsPage);
    window.draw(exitButton);
  }

  // Checks if the quit button has been clicked
  if (showQuitGameDialouge == true) {
    quitGameDialouge();
  }

  window.setFramerateLimit(120);
  window.display();
}

int main() {
  // Sets inital game states
  showQuitGameDialouge = false;
  showSettingsPage = false;
  isGameChanging = true;
  waiting = false;
  shouldLoadGame = false;
  gameState = "mainMenu";

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      /*       if (event.type == sf::Event::Resized) {
              sf::FloatRect view(1920, -1080, event.size.width,
         event.size.height); window.setView(sf::View(view));
            } */
    }
    // Checks if on Main Menu Screen
    if (gameState == "mainMenu") {
      // Checks if the game state changed
      if (isGameChanging) {
        // Load assets for main menu
        resourceManager.loadMainMenu();
        isGameChanging = false;
      }
      mainMenu();
    }
    // Checks if on Main Game Screen
    if (gameState == "gameLoop") {
      // Checks if the game state changed
      if (isGameChanging) {
        // Load assets for main game
        resourceManager.loadGame();
        isGameChanging = false;
      }
      gameLoop();
    }
  }

  return 0;
}