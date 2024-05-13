#include<iostream>
#include<string>
#include<pthread.h>
#include<math.h>
#include <semaphore.h>
#include <SFML/Graphics.hpp>
#include "array.h"
#include "setup.h"
using namespace std;
using namespace sf;
//the pacman main thread
void* PACMAN(void* input){
    float timer;
    while(1){
        //do something
        
        timer += pacClock.getElapsedTime().asMilliseconds();
        pacClock.restart();
        //all movement is locked under a critical section
        sem_wait(&reader);
        if(timer >= 200){
         if(pacMovement == "R"){
            if(Grid[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)+1] == 1){
            pacMovement = " ";
             
            goto exitPACMAN;
            }
            pacman.move(20,0);
           
            }
        else if(pacMovement == "L"){
           
            if(Grid[(int)pacman.getPosition().y/20][(((int)pacman.getPosition().x-1)/20)] == 1){
                //pacman.move(1,0);
                pacMovement = " ";
                
                goto exitPACMAN;
            }
            pacman.move(-20,0);
           
        }
        else if(pacMovement == "U"){
            
            if(Grid[((int)pacman.getPosition().y/20)-1][((int)pacman.getPosition().x/20)] == 1){
            pacMovement = " ";
           
            goto exitPACMAN;
            }
            pacman.move(0,-20);
             
        }
        else if(pacMovement == "D"){
            
            if(Grid[((int)pacman.getPosition().y/20)+1][((int)pacman.getPosition().x/20)] == 1){
            pacMovement = " ";
           
            goto exitPACMAN;
            }
            pacman.move(0,20);
           
        }
            timer=0;
        }     //sample critical section.
        //check for wall colision based on current movement direction..
        exitPACMAN:
        sem_post(&reader);
        readCount = 1;
        //write
    }  
    
    //exit the thread
    pthread_exit(0);
}
void* GameEngine(void* input){
    window.setFramerateLimit(30);
    //the window will create a pacman thread that will initialize the pacman movement mechanics and collision detection with items and ghosts.
    pthread_t pacThread;
    pthread_create(&pacThread,0,PACMAN,0);
    //pacman thread calling ends here.
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                GAME = false;
                window.close();
            }
        }
        //input thread is also the gameEngine thread.
        if(Keyboard::isKeyPressed(Keyboard::A)){
                pacMovement = "L";
            }
        if(Keyboard::isKeyPressed(Keyboard::D)){
                pacMovement = "R";
            }
        if(Keyboard::isKeyPressed(Keyboard::W)){
                pacMovement = "U";
            }
        if(Keyboard::isKeyPressed(Keyboard::S)){
                pacMovement = "D";
            }
        sem_wait(&reader);
        pthread_mutex_lock(&lock);
        window.clear();
        window.draw(background); //read function
        window.draw(pacman); //read function
        window.display();
        window.setActive(false);
        sem_post(&reader);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(0);
}
void* UserInterface(void* input){
        Clock clock;
        int frame = 0;
        //ui initializer
        bgt.loadFromFile("/home/ibtehaj/projTest/pacMap1.png");
        background.setTexture(bgt);
        pc.loadFromFile("pset.png");
        pacman.setTexture(pc);
        pacman.setPosition(20,20);
        //ui initializer
        while(1){
            if(GAME == false){
                break;
            }
            //used to set the ui elements and their feature where the gameEngine will render and handle all game logic
            float animator = clock.getElapsedTime().asMilliseconds();
            pacAnim+=animator;
            clock.restart();
            
            if(pacAnim >=150){
            if(frame >=2){
                frame = 0;
            }
            else
            frame = (frame + 1)%3;
            pacAnim = 0;
        }
         pthread_mutex_lock(&lock);
            pacman.setTextureRect(IntRect(0,frame*20,20,20)); //sample critical section.
         pthread_mutex_unlock(&lock);
        }
    pthread_exit(0);
}

int main(){
    pthread_t win,ui;
    window.setActive(false);
    sem_init(&reader,0,1);
    pthread_create(&win,0,GameEngine,(void*)500);
    pthread_create(&ui,0,UserInterface,0);
    pthread_exit(0);
    return 0;   
}
