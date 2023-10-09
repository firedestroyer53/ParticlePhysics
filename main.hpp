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

struct Coordinate {
    int x;
    int y;

    Coordinate(int i, int j) {
        x = i;
        y = j;
    }

    Coordinate() {
        x = 0;
        y = 0;
    }

};

struct Particle {
    particleType type;
    bool isNull;
    bool justMoved;

    Particle(){
        type = none;
        isNull = true;
        justMoved = false;
    }

    Particle(particleType pType){
        type = pType;
        isNull = false;
        justMoved = false;
    }
};

int countParticles();

bool initializeSDL();

void closeSDL();

void renderGrid();

void handleMouse();

void initializeGrid();

void updateGrid();

void removeParticle(Coordinate pos);

void addParticle(Coordinate pos, Particle particle);

void moveParticle(Coordinate pos1, Coordinate pos2, Particle particle);

void swapParticle(Coordinate pos1, Coordinate pos2);

Coordinate sandBehavior(Coordinate pos);

Coordinate waterBehavior(Coordinate pos);

Coordinate steamBehavior(Coordinate pos);

Coordinate wallBehavior(Coordinate pos);

bool canMove(Direction direction, Coordinate pos);

bool canSwap(Direction direction, Coordinate pos);

bool pointInsideRect(Coordinate point, SDL_Rect rectangle);