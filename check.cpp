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
    //sem_post(&reader);
    while(1){
        //do something

        timer += pacClock.getElapsedTime().asMilliseconds();
        pacClock.restart();
        //all movement is locked under a critical section
        if(pacMovement== " "){
            //sem_wait(&reader);
            goto exitPACMAN;
        }
        sem_wait(&reader2);
        //game grid reading semaphore -> 4 ghosts and 1 pacman grid access.
        if(timer >= 200){

         if(pacMovement == "R"){
            if(Grid[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)+1] == 1){
            pacMovement = " "; //made a semaphore to give access to the written values for movement by the game engine.
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
               
            }
            goto exitPACMAN;
            }
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
            pacman.move(20,0);

            }
        else if(pacMovement == "L"){

            if(Grid[(int)pacman.getPosition().y/20][(((int)pacman.getPosition().x-1)/20)] == 1){
                //pacman.move(1,0);
                pacMovement = " ";
                if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
                goto exitPACMAN;
            }
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
            pacman.move(-20,0);

        }
        else if(pacMovement == "U"){

            if(Grid[((int)pacman.getPosition().y/20)-1][((int)pacman.getPosition().x/20)] == 1){
            pacMovement = " ";
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
               
            }
            goto exitPACMAN;
            }
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
            pacman.move(0,-20);
        }
        else if(pacMovement == "D"){
            if(Grid[((int)pacman.getPosition().y/20)+1][((int)pacman.getPosition().x/20)] == 1){
            pacMovement = " ";
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
            goto exitPACMAN;
            }
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
                
            }
            pacman.move(0,20);
        }
            timer=0;
        }     //sample critical section.
        //check for wall colision based on current movement direction..
        exitPACMAN:
        for(int i=0;i<2;i++){
        sem_post(&reader);
        }
        readCount = 1;
        //write.
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
        
        sem_wait(&reader); //lock for writing
         // will probably go the value 5.
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
        
        pthread_mutex_lock(&lock);
        window.clear();
        window.draw(background); //read function
        for(int i=1;i<=29;i++){
            for(int j=1;j<=24;j++){
                if(pellets[i][j].status){ //written by pacman.
                    window.draw(pellets[i][j].coin);
                }
            }
        }
        window.draw(pacman); //written by pacman for x and y position.
        window.draw(score);
        window.draw(pointNum);
       //read function
        window.display();
        window.setActive(false);
        pthread_mutex_unlock(&lock);
        sem_post(&reader2);
        
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
        pacman.setPosition(12*20,17*20);
        Coin.loadFromFile("/home/ibtehaj/projTest/pelletp.png");
        retro.loadFromFile("/home/ibtehaj/projTest/Emulogic-zrEw.ttf");
        score.setFont(retro);
        score.setString("SCORE:");
        score.setPosition(150,370);
        score.setCharacterSize(16);
        score.setFillColor(sf::Color::Yellow);
        pointNum.setFont(retro);
        pointNum.setString("0");
        pointNum.setPosition(260,370);
        pointNum.setCharacterSize(16);
        pointNum.setFillColor(sf::Color::Yellow);
        //load pellets and theri initial positions according to places on the grid.
        for(int i=0;i<30;i++){
            for(int j=0;j<25;j++){
                if(Grid[i][j] == 0){
                    pellets[i][j].coin.setTexture(Coin);
                    pellets[i][j].coin.setPosition(j*20,i*20);
                }
                else{
                pellets[i][j].status = false;
                }
            }
        }
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
            //score will also be set inside this thread.
            sem_wait(&reader);
            pointNum.setString(to_string(points)); //currently receiving a little too late. not entirely synced with the pacman write need to cater that.
         
        }
    pthread_exit(0);
}

int main(){
    pthread_t win,ui;
    window.setActive(false);
    sem_init(&reader,0,2);
    sem_init(&reader2,0,0);
    sem_init(&readCounter,0,2);
    pthread_create(&win,0,GameEngine,(void*)500);
    pthread_create(&ui,0,UserInterface,0);
    pthread_exit(0);
    return 0;
}

//main -> ui thread, (game engine-> pacman).
//pacman thread updates position x and y rendered by game-engine(correct draw position). game engine(movement direction)
//ui thread only has read access and change access pacman.x pacman.y (pacman.Texture).
