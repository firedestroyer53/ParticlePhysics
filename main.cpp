#define SDL_MAIN_HANDLED
#define particleToBeUsed steam

#include "main.hpp"

const int GRID_SIZE = 512;
Particle grid[GRID_SIZE][GRID_SIZE];
SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;
const int SCREEN_WIDTH = GRID_SIZE;
const int SCREEN_HEIGHT = GRID_SIZE;
const int UPDATE_INTERVAL_MS = 0;
const bool DEBUG = false;



int main(int argc, char* argv[]) {
    if (!initializeSDL()) {
        return 1;
    }

    initializeGrid();
    std::vector<double> fpsList;
    const char* fpsFilePath = "fps_log.txt";
    const char* particleFilePath = "particle_log.txt";
    std::vector<int> particleCountList;  // Store particle counts
    int particleCount = 0;  // Initialize particle count


    SDL_Event e;
    bool quit = false;
    Uint32 lastUpdateTime = SDL_GetTicks();
    Uint32 frameCount = 0;
    Uint32 lastFPSTime = lastUpdateTime;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastUpdateTime >= UPDATE_INTERVAL_MS) {
            lastUpdateTime = currentTime;
            updateGrid();
            renderGrid();
            frameCount++;
        }

        Uint32 elapsedFPSTime = currentTime - lastFPSTime;
        if (elapsedFPSTime >= 1000) {
            double fps = static_cast<double>(frameCount) / (elapsedFPSTime / 1000.0);
            std::cout << "FPS: " << fps << std::endl;
            if(DEBUG){
                fpsList.push_back(fps);
                // Add particle count to the list
                particleCountList.push_back(particleCount);
                particleCount = 0;  // Reset particle count
            }
          
            lastFPSTime = currentTime;
            frameCount = 0;
        }

        // Update particle count every frame
        if(DEBUG){
            int currentParticleCount = countParticles();
            if(currentParticleCount >= 262144){
                break;
            }
            if (currentParticleCount > particleCount) {
                particleCount = currentParticleCount;
            }
        }
    }
    // Save FPS values to fps_log.txt
    if(DEBUG){
        std::ofstream fpsFile(fpsFilePath);
        if (fpsFile.is_open()) {
            for (double fps : fpsList) {
                fpsFile << fps << std::endl;
            }
            fpsFile.close();
            std::cout << "FPS values saved to " << fpsFilePath << std::endl;
        } else {
            std::cerr << "Unable to open " << fpsFilePath << " for writing." << std::endl;
        }

        // Save particle counts to particle_log.txt
        std::ofstream particleFile(particleFilePath);
        if (particleFile.is_open()) {
            for (int count : particleCountList) {
                particleFile << count << std::endl;
            }
            particleFile.close();
            std::cout << "Particle counts saved to " << particleFilePath << std::endl;
        } else {
            std::cerr << "Unable to open " << particleFilePath << " for writing." << std::endl;
        }
    }


    closeSDL();
    return 0;
}

bool initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Particle Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderGrid() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render each particle as a rectangle
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j].isNull) continue; 
            SDL_Rect particleRect;
            particleRect.x = i * (SCREEN_WIDTH / GRID_SIZE);
            particleRect.y = j * (SCREEN_HEIGHT / GRID_SIZE);
            particleRect.w = SCREEN_WIDTH / GRID_SIZE;
            particleRect.h = SCREEN_HEIGHT / GRID_SIZE;

            // Set color based on particle type
            SDL_Color color;
            switch (grid[i][j].type) {
                case none:
                    color = { 0, 0, 0, 255 }; // Black for none
                    break;
                case sand:
                    color = { 255, 255, 0, 255 }; // Yellow for sand
                    break;
                case water:
                    color = { 0, 0, 255, 255 }; // Blue for water
                    break;
                case steam:
                    color = {255, 0, 0, 255};
                    break;
                case wall:
                    color = {255, 0, 0, 255};
            }

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &particleRect);
        }
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}

void initializeGrid() {
    Particle defaultParticle;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            Coordinate pos(i, j);
            addParticle(pos, defaultParticle);
        }
    }
    Particle part(wall);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++){
            if (rand() % 6 == 0) { 
                //addParticle(Coordinate(i, j), part);
            }
        }
    }
}

void updateGrid() {
    Coordinate pos2;
    for (int i = GRID_SIZE-1; i >= 0; i--) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            Coordinate pos(i, j);
            if (grid[i][j].isNull) continue;
            switch (grid[i][j].type) {
                case none:
                    break;
                case sand:
                    pos2 = sandBehavior(pos);
                    break;
                case water:
                    pos2 = waterBehavior(pos);
                    break;
                case steam:
                    pos2 = steamBehavior(pos);
                    break;
                case wall:
                    pos2 = pos;
                    break;
            }
            if (grid[pos.x][pos.y].justMoved) continue;
            moveParticle(pos, pos2, grid[pos.x][pos.y]);
            grid[pos2.x][pos2.y].justMoved = true;
        }
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            Coordinate pos(i, j);
            if (grid[i][j].isNull) continue;
            grid[i][j].justMoved = false;
        }
    }
    Particle testParticle(particleToBeUsed);
    for(int i = 0; i < GRID_SIZE; i+=GRID_SIZE/25){
        addParticle(Coordinate(i,GRID_SIZE-2), testParticle);
    }
}

Coordinate sandBehavior(Coordinate pos) {
    Coordinate pos2 = pos;

    auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    bool b = gen();
    switch(b){
        case true:
            if(canMove(down, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y + 1;
            } else if(canMove(downLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y + 1;
            } else if(canMove(downRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y + 1;
            }
            break;
        case false:
            if(canMove(down, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y + 1;
            } else if(canMove(downRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y + 1;
            } else if(canMove(downLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y + 1;
            }
            break;
    }
    
    return pos2;
}

Coordinate waterBehavior(Coordinate pos) {
    Coordinate pos2 = pos;

    auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    bool b = gen();
    switch(b){
        case true:
            if(canMove(down, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y + 1;
            } else if(canMove(downLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y + 1;
            } else if(canMove(downRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y + 1;
            } else if(canMove(left, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y;
            } else if(canMove(right, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y;
            }
            break;
        case false:
            if(canMove(down, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y + 1;
            } else if(canMove(downRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y + 1;
            } else if(canMove(downLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y + 1;
            } else if(canMove(right, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y;
            } else if(canMove(left, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y;
            }
            break;
    }

    return pos2;
}

Coordinate steamBehavior(Coordinate pos) {

    Coordinate pos2 = pos;

    auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    bool b = gen();
    switch(b){
        case true:
            if(canMove(up, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y - 1;
            } else if(canMove(upLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y - 1;
            } else if(canMove(upRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y - 1;
            } else if(canMove(left, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y;
            } else if(canMove(right, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y;
            }
            break;
        case false:
            if(canMove(up, pos)){
                pos2.x = pos.x;
                pos2.y = pos.y - 1;
            } else if(canMove(upRight, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y - 1;
            } else if(canMove(upLeft, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y - 1;
            } else if(canMove(right, pos)){
                pos2.x = pos.x + 1;
                pos2.y = pos.y;
            } else if(canMove(left, pos)){
                pos2.x = pos.x - 1;
                pos2.y = pos.y;
            }
            break;
    }

    return pos2;
}

void removeParticle(Coordinate pos) {
    grid[pos.x][pos.y].isNull = true;
}

void addParticle(Coordinate pos, Particle particle) {
    if (pos.x >= 0 && pos.x < GRID_SIZE && pos.y >= 0 && pos.y < GRID_SIZE) {
        grid[pos.x][pos.y] = particle;
    }
}

void moveParticle(Coordinate pos1, Coordinate pos2, Particle particle) {
    removeParticle(pos1);
    addParticle(pos2, particle);
}

void swapParticle(Coordinate pos1, Coordinate pos2){
    Particle p1 = grid[pos1.x][pos1.y];
    Particle p2 = grid[pos2.x][pos2.y];

    grid[pos1.x][pos1.y] = p2;
    grid[pos2.x][pos2.y] = p1;
}

bool canMove(Direction direction, Coordinate pos){
    switch(direction){
        case up:
            return(pos.y - 1 >= 0 && grid[pos.x][pos.y - 1].isNull);
        case down:
            return(pos.y + 1 < GRID_SIZE && grid[pos.x][pos.y + 1].isNull);
        case left:
            return(pos.x - 1 >= 0 && grid[pos.x - 1][pos.y].isNull);
        case right:
            return(pos.x + 1 < GRID_SIZE && grid[pos.x + 1][pos.y].isNull);
        case upLeft:
            return(pos.x - 1 >= 0 && pos.y - 1 >= 0 && grid[pos.x -1][pos.y - 1].isNull);
        case upRight:
            return(pos.x + 1 < GRID_SIZE && pos.y - 1 >= 0 && grid[pos.x + 1][pos.y - 1].isNull);
        case downLeft:
            return(pos.x - 1 >= 0 && pos.y + 1 < GRID_SIZE && grid[pos.x - 1][pos.y + 1].isNull);
        case downRight:
            return(pos.x + 1 < GRID_SIZE && pos.y + 1 < GRID_SIZE && grid[pos.x + 1][pos.y + 1].isNull);
    }
    return 0;
}

bool canSwap(Direction direction, Coordinate pos){
    switch(direction){
        case down:
            return(pos.y + 1 < GRID_SIZE && grid[pos.x][pos.y + 1].type == water);
        case downLeft:
            return(pos.y + 1 < GRID_SIZE && pos.x - 1 > 0 && grid[pos.x - 1][pos.y + 1].type == water);
        case downRight:
            return(pos.y + 1 < GRID_SIZE && pos.x + 1 > 0 && grid[pos.x + 1][pos.y + 1].type == water);
    }
}

bool pointInsideRect(Coordinate point, SDL_Rect rectangle){
    return(rectangle.x - rectangle.w/2 < point.x < rectangle.x + rectangle.w/2 && rectangle.y - rectangle.h/2 < point.y < rectangle.y + rectangle.h/2);
}

int countParticles() {
    int count = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (!grid[i][j].isNull) {
                count++;
            }
        }
    }
    return count;
}
