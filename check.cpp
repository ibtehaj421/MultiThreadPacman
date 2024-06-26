#include<iostream>
#include<string>
#include<pthread.h>
#include<math.h>
#include <semaphore.h>
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<unistd.h>
#include<time.h>
//#include "array.h"
//#include "setup.h"
#include "shortestpath.h"
using namespace std;
using namespace sf;


//the ghost1 thread
void* GHOST1(void* input){
    
    sem_wait(&keyPermit);
        sem_wait(&exitPermit);
            sleep(1);
        sem_post(&keyPermit);
    sem_post(&exitPermit);

    Queue<int> ghostPath;

    while(1){
        sem_wait(&reader); //let full.
        int* Estart = new int[2] {(int)(ghost1.getPosition().y / CELL_SIZE), (int)(ghost1.getPosition().x / CELL_SIZE)};
        sem_wait(&mute);
        int* Eend = new int[2] {(int)(pacman.getPosition().y / CELL_SIZE), (int)(pacman.getPosition().x / CELL_SIZE)};
        if(pacman.getPosition().x == ghost1.getPosition().x && pacman.getPosition().y == ghost1.getPosition().y){
            if(lifeTrue){
            life--;
            lifeTrue = false; //to allow only one decrement at a time.
            }
        }
        sem_post(&mute);
        sem_getvalue(&pacWrite,&value);
        if(value < numReaders)
        sem_post(&pacWrite);
        ghostPath = FindShortestPath(Estart, Eend);
        move_enemy(ghostPath, ghost1, c1);
    }
    pthread_exit(0);
}
//the ghost2 thread
void* GHOST2(void* input){

    sem_wait(&keyPermit);
        sem_wait(&exitPermit);
            sleep(3);
        sem_post(&keyPermit);
    sem_post(&exitPermit);


    float timer;
    int direction;
    //sem_post(&reader);
    
    while(1){
        
        timer += c2.getElapsedTime().asMilliseconds();
        c2.restart();

        
         //for reading whatever position pacman is at.
        sem_wait(&reader3);
        if(timer >= 200){
            sem_wait(&reader);
         if(g2Movement == "R"){
            if(Grid[(int)ghost2.getPosition().y/20][((int)ghost2.getPosition().x/20)+1] == 1){
            
            direction= rand()%2;

            if(direction)
                g2Movement = "U";
            else
                g2Movement = "D";

            goto exitg2;
            }
            ghost2.move(20,0);
           
            }
        else if(g2Movement == "L"){
           
            if(Grid[(int)ghost2.getPosition().y/20][(((int)ghost2.getPosition().x-1)/20)] == 1){
            direction= rand()%2;

            if(direction)
                g2Movement = "U";
            else
                g2Movement = "D";
               
                goto exitg2;
            }
            ghost2.move(-20,0);
           
        }
        else if(g2Movement == "U"){
            
            if(Grid[((int)ghost2.getPosition().y/20)-1][((int)ghost2.getPosition().x/20)] == 1){
            direction= rand()%2;

            if(direction)
                g2Movement = "R";
            else
                g2Movement = "L";
           
            goto exitg2;
            }
            ghost2.move(0,-20);
             
        }
        else if(g2Movement == "D"){
            
            if(Grid[((int)ghost2.getPosition().y/20)+1][((int)ghost2.getPosition().x/20)] == 1){
            direction= rand()%2;

            if(direction)
                g2Movement = "U";
            else
                g2Movement = "D";
           
            goto exitg2;
            }
            ghost2.move(0,20);
           
        }
            timer=0;
        }     //sample critical section.
        //check for wall colision based on current movement direction..
        exitg2:
        sem_post(&reader3);
        readCount = 1;   
        //write
    }  
    
    //exit the thread
    pthread_exit(0);
}
//the ghost3 thread
void* GHOST3(void* input){
    
    sem_wait(&keyPermit);
        sem_wait(&exitPermit);
            sleep(6);
        sem_post(&keyPermit);
    sem_post(&exitPermit);

    Queue<int> ghostPath;

    while(1){
        sem_wait(&reader);
        int* Estart = new int[2] {(int)(ghost3.getPosition().y / CELL_SIZE), (int)(ghost3.getPosition().x / CELL_SIZE)};
        sem_wait(&mute);
        int* Eend = new int[2] {(int)(pacman.getPosition().y / CELL_SIZE), (int)(pacman.getPosition().x / CELL_SIZE)};
        sem_post(&mute);
        sem_getvalue(&pacWrite,&value);
        if(value < numReaders)
        sem_post(&pacWrite);
        ghostPath = FindShortestPath(Estart, Eend);
        move_enemy(ghostPath, ghost3, c3);
    }

    pthread_exit(0);
}
//the ghost4 thread
void* GHOST4(void* input){
    
    sem_wait(&keyPermit);
        sem_wait(&exitPermit);
            sleep(7);
        sem_post(&keyPermit);
    sem_post(&exitPermit);

    Queue<int> ghostPath;

    while(1){
        sem_wait(&reader);
        int* Estart = new int[2] {(int)(ghost4.getPosition().y / CELL_SIZE), (int)(ghost4.getPosition().x / CELL_SIZE)};
        sem_wait(&mute);
        int* Eend = new int[2] {(int)(pacman.getPosition().y / CELL_SIZE), (int)(pacman.getPosition().x / CELL_SIZE)};
        sem_post(&mute);
        sem_getvalue(&pacWrite,&value);
        if(value < numReaders)
        sem_post(&pacWrite);
        ghostPath = FindShortestPath(Estart, Eend);
        move_enemy(ghostPath, ghost4, c4);
    }

    pthread_exit(0);
}

//the pacman main thread
void* PACMAN(void* input){
    float timer;
    //sem_post(&reader);
    float lifeTimer;
    while(1){
        //do something
        
        timer += pacClock.getElapsedTime().asMilliseconds();
        lifeTimer += lifeClock.getElapsedTime().asSeconds();
        pacClock.restart();
        lifeClock.restart();
        //all movement is locked under a critical section

        if(pacMovement== " "){
            //sem_wait(&reader);
            goto exitPACMAN;
        }
        sem_wait(&reader2); //game engine gives access to pacman for write
        //game grid reading semaphore -> 4 ghosts and 1 pacman grid access.
        sem_getvalue(&pacWrite,&value);
        if(timer >= 200 && value == numReaders){ //timer checks if current access is viable to write
            //pacman writes here.
            for(int i=0;i<numReaders;i++){
                sem_wait(&pacWrite);
            }
            sem_wait(&mute);
            if(lifeTimer >= 2 && !lifeTrue){
                lifeTimer = 0;
                lifeTrue = true;
            }
         if(pacMovement == "R"){
            if(Grid[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)+1] == 1){
            pacMovement = " "; //made a semaphore to give access to the written values for movement by the game engine.             
           chomp.pause();
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++;
            }
            goto exitPACMAN;
            }
            if(pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status){
                pellets[(int)pacman.getPosition().y/20][((int)pacman.getPosition().x/20)].status = false;
                points++; //ui
            }
            pacman.move(20,0);
           
            }
        else if(pacMovement == "L"){
           
            if(Grid[(int)pacman.getPosition().y/20][(((int)pacman.getPosition().x-1)/20)] == 1){
                //pacman.move(1,0);
                pacMovement = " ";
                chomp.pause();
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
            chomp.pause();
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
            chomp.pause();
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
        sem_post(&mute);
        for(int i=0;i<numReaders;i++){
            sem_post(&reader);
        }
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
    
    //this will create all the ghost threads
    pthread_t ghost1Thread, ghost2Thread, ghost3Thread, ghost4Thread;
    pthread_create(&ghost1Thread,0,GHOST1,0);
    //pthread_create(&ghost2Thread,0,GHOST2,0);
    //pthread_create(&ghost3Thread,0,GHOST3,0);
    //pthread_create(&ghost4Thread,0,GHOST4,0);
    //ghost threads calling ends here.
    
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
        
         //lock for writing
         // will probably go the value 5.

        //input thread is also the gameEngine thread.
        if(Keyboard::isKeyPressed(Keyboard::A)){
                chomp.play();
                sheetcol = 2;
                pacMovement = "L";
            }
        if(Keyboard::isKeyPressed(Keyboard::D)){
                chomp.play();
                sheetcol = 0;
                pacMovement = "R";
            }
        if(Keyboard::isKeyPressed(Keyboard::W)){
                chomp.play();
                sheetcol = 3;
                pacMovement = "U";
            }
        if(Keyboard::isKeyPressed(Keyboard::S)){
                chomp.play();
                sheetcol = 1;
                pacMovement = "D";
            }

        pthread_mutex_lock(&lock);
        sem_wait(&reader);
        window.clear();
        window.draw(background); //read function
        //window.draw(pellets[1][1].coin);
        for(int i=1;i<=29;i++){
            for(int j=1;j<=24;j++){
                if(pellets[i][j].status){ //written by pacman.
                    window.draw(pellets[i][j].coin);
                }
            }
        }
        sem_wait(&mute);
        window.draw(pacman); //written by pacman for x and y position.
        window.draw(pointNum);
        window.draw(lifeNum);
        sem_post(&mute);
        window.draw(score);
        window.draw(lives);
        sem_getvalue(&pacWrite,&value);
        if(value < numReaders)
        sem_post(&pacWrite);
        window.draw(ghost1);
        //window.draw(ghost3);
        //window.draw(ghost4);
        // if(sem_trywait(&reader3)){
        // window.draw(ghost2);
        // sem_post(&reader3);
        // }
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
        pc.loadFromFile("/home/ibtehaj/projTest/pacmanspritesheet.png");
        pacman.setTexture(pc);
        pacman.setPosition(12*20,17*20);
        Coin.loadFromFile("/home/ibtehaj/projTest/pelletp.png");
        retro.loadFromFile("Emulogic-zrEw.ttf");
        score.setFont(retro);
        score.setString("SCORE:");
        score.setPosition(170,370);
        score.setCharacterSize(16);
        score.setFillColor(sf::Color::Yellow);
        pointNum.setFont(retro);
        pointNum.setString("0");
        pointNum.setPosition(280,370);
        pointNum.setCharacterSize(16);
        pointNum.setFillColor(sf::Color::Yellow);
        lives.setFont(retro);
        lives.setString("LIVES");
        lives.setPosition(50,370);
        lives.setCharacterSize(12);
        lives.setFillColor(sf::Color::Red);
        lifeNum.setFont(retro);
        lifeNum.setString(to_string(life));
        lifeNum.setPosition(70,390);
        lifeNum.setCharacterSize(12);
        lifeNum.setFillColor(sf::Color::Red);
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

        //ghosts
        g1.loadFromFile("/home/ibtehaj/projTest/g1.png");
        ghost1.setTexture(g1);
        ghost1.setPosition(210,290);
        g2.loadFromFile("/home/ibtehaj/projTest/g2.png");
        ghost2.setTexture(g2);
        ghost2.setPosition(245,300);
        g3.loadFromFile("/home/ibtehaj/projTest/g3.png");
        ghost3.setTexture(g3);
        ghost3.setPosition(255,290);
        g4.loadFromFile("/home/ibtehaj/projTest/g4.png");
        ghost4.setTexture(g4);
        ghost4.setPosition(275,290);
        base.openFromFile("/home/ibtehaj/projTest/level2.wav");
        base.setVolume(50.f);
        base.setLoop(true);
        base.play();
        chomp.openFromFile("/home/ibtehaj/projTest/pacman_chomp.wav");
        chomp.setVolume(70.f);
        chomp.setLoop(true);
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
            pacman.setTextureRect(IntRect(sheetcol*20,frame*20,20,20)); //sample critical section.
         pthread_mutex_unlock(&lock);

            sem_wait(&reader);
            sem_wait(&mute);
            pointNum.setString(to_string(points)); //currently receiving a little too late. not entirely synced with the pacman write need to cater that.
            lifeNum.setString(to_string(life));
            sem_post(&mute);
            sem_getvalue(&pacWrite,&value);
            if(value < numReaders)
            sem_post(&pacWrite);

        }
    pthread_exit(0);
}

int main(){
    pthread_t win,ui;
    window.setActive(false);
    numReaders = 3;
    srand(time(0));

    sem_init(&reader,0,numReaders);
    for(int i=0;i<numReaders;i++){
        sem_wait(&reader);
    }
    sem_init(&reader2,0,0);
    sem_init(&reader3,0,0); // *under experiment* for ghost 2 random movement 
    //sem_init(&readCounter,0,2);
    sem_init(&pacWrite,0,numReaders); //for now
    sem_init(&mute,0,1);
    sem_init(&keyPermit,0,2); //For ghost Leaving logic
    sem_init(&exitPermit,0,2); 

    pthread_create(&win,0,GameEngine,(void*)500);
    pthread_create(&ui,0,UserInterface,0);
    pthread_exit(0);
    return 0;   
}
