#include<iostream>
#include<string>
#include<pthread.h>
#include <semaphore.h>
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
using namespace sf;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sf::RenderWindow window(sf::VideoMode(500,600), "PACMAN");
Sprite background;
Texture bgt;
sf::CircleShape shape(100.f);
Sprite pacman;
Texture pc;
Texture Coin;
float pacAnim;
bool GAME = true;
std::string pacMovement = " ";
pthread_attr_t unlinker;
bool allThreadKiller = false;
Clock pacClock;
int readCount = 0; //initially set the read count to zero to basically restrict the write lock mechanism.
sem_t reader; //full
sem_t reader2;
sem_t reader3;

int readCounter; //allow upto 3 threads for now.
Text score;
Text pointNum;
Font retro;
int points;
int sheetcol = 0;
struct pellet
{
    Sprite coin;
    bool status = true; //initially set as true as each pellet can be spawned and eaten will be set to false once a pellet gets eaten until re spawning of pellets.
};
struct pellet pellets[30][25];
sem_t pacWrite; //empty
int value;
sem_t keyPermit, exitPermit; 
Clock c1,c2,c3,c4;   //For each ghost
Sprite ghost1,ghost2,ghost3,ghost4;
Texture g1,g2,g3,g4;
std::string g2Movement = "U";  //The random moving ghosts alwasy move up first
std::string g3Movement = "U";
Music base;
Music chomp;
sem_t mute;
