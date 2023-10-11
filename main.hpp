#ifndef PARTICLEPHYSICS_MAIN_HPP
#define PARTICLEPHYSICS_MAIN_HPP

#endif //PARTICLEPHYSICS_MAIN_HPP

#define null true

#include <SDL2/SDL.h>

enum particleType {
    none, sand, water, steam, wall
};

enum Direction{
    up, down, left, right, upLeft, upRight, downLeft, downRight
};

struct Vector2 {
    int x;
    int y;

    Vector2(int i, int j) {
        x = i;
        y = j;
    }

    Vector2() {
        x = 0;
        y = 0;
    }

};

struct Particle {
    particleType type;
    bool isNull;
    bool justMoved;

    double density;

    Particle(){
        type = none;
        isNull = true;
        justMoved = false;
        density = 0.0d;
    }

    Particle(particleType pType){
        type = pType;
        isNull = false;
        justMoved = false;
        switch(type){
            case none:
                density = 0;
                break;
            case sand:
                density = 1.52;
                break;
            case water:
                density = 1;
                break;
            case steam:
                density = 0.000598;
                break;
        }
    }
};

int countParticles();

bool initializeSDL();

void closeSDL();

void renderParticle(Vector2 pos, Particle particle);

void handleMouse(int mouseX, int mouseY);

void initializeGrid();

void updateGrid();

void removeParticle(Vector2 pos);

void addParticle(Vector2 pos, Particle particle);

void moveParticle(Vector2 pos1, Vector2 pos2, Particle particle);

void swapParticle(Vector2 pos1, Vector2 pos2);

Vector2 sandBehavior(Vector2 pos);

Vector2 waterBehavior(Vector2 pos);

Vector2 steamBehavior(Vector2 pos);

Vector2 wallBehavior(Vector2 pos);

bool canMove(Direction direction, Vector2 pos);

bool canSwap(Direction direction, Vector2 pos);

bool pointInsideRect(Vector2 point, SDL_Rect rectangle);