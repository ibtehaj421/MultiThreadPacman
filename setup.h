#include<iostream>
#include<string>
#include<pthread.h>
#include <SFML/Graphics.hpp>
using namespace sf;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sf::RenderWindow window(sf::VideoMode(500,600), "PACMAN");
Sprite background;
Texture bgt;
sf::CircleShape shape(100.f);
Sprite pacman;
Texture pc;
float pacAnim;
bool GAME = true;
std::string pacMovement;
pthread_attr_t unlinker;
bool allThreadKiller = false;
Clock pacClock;