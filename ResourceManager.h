#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <SFML/Graphics.hpp>

class ResourceManager {
 public:
  sf::Texture backgroundTex, menuTitleTex, playButtonTex, loadButtonTex,
      settingsButtonTex, quitButtonTex, playButtonSelectedTex,
      loadButtonSelectedTex, quitButtonSelectedTex, quitGameDialougeBoxTex,
      mapImage, extrasImage;
  sf::Font deathFont;
  ResourceManager();
  void loadMainMenu();
  void loadGame();
  ~ResourceManager();
};
#endif  // RESOURCEMANAGER_H
