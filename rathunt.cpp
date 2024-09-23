#include <SDL2/SDL.h>
#include <stdio.h>
#include <cmath>
#include<vector>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;
const double FOV = 60 * M_PI / 180;
const int NUM_RAYS = SCREEN_WIDTH;

// Define map
std::vector<std::vector<int>> map = {
    {1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 1},
	{1, 0, 1, 1, 1, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1}
};

// player's position on the map
struct Player {
    double x = 1.5;
    double y = 1.5;
    double angle = 0.0; // camera angle
};



void castRay(SDL_Renderer* renderer, const Player& player, double rayAngle, int rayNum) {
    double rayDirX = cos(rayAngle);
	double rayDirY = sin(rayAngle);

    int mapX = static_cast<int>(player.x);
	int mapY = static_cast<int>(player.y);

    double sideDistX;
	double sideDistY;

    double deltaDistX = std::abs(1 / rayDirX);
	double deltaDistY = std::abs(1 / rayDirY);
	double perpWallDist;

    int stepX, stepY;
    int hitWall = 0;
    int side;

    if (rayDirX < 0) {
		stepX = -1;
		sideDistX = (player.x - mapX) * deltaDistX;
	} else {
		stepX = 1;
		sideDistX = (mapX + 1.0 - player.x) * deltaDistX;
	}

	if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (player.y - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - player.y) * deltaDistY;
    }

    while (hitWall == 0) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }
        if (map[mapY][mapX] > 0) hitWall = 1;
    }

    if (side == 0) {
        perpWallDist = (mapX - player.x + (1 - stepX) / 2) / rayDirX;
    } else {
        perpWallDist = (mapY - player.y + (1 - stepY) / 2) / rayDirY;
    }

    int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist);
    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawStart < 0) drawStart = 0;
    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

    
    int x = rayNum;
    int y1 = drawStart;
    int y2 = drawEnd;

    
    // Drawing wall - add color based on orientation of wall

    // EAST & WEST walls
    if (side == 0) {
        SDL_SetRenderDrawColor(renderer, 0, 10, 0, 255);
        SDL_RenderDrawLine(renderer, x, y1, x, y2);
    } else {
    // NORTH & SOUTH
        SDL_SetRenderDrawColor(renderer, 152, 133, 88, 255);
        SDL_RenderDrawLine(renderer, x, y1, x, y2);
    }
    
    

    // Drawing ceiling
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderDrawLine(renderer, x, 0, x, y1);
    
    // Drawing floor
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawLine(renderer, x, y2, x, SCREEN_HEIGHT);
}



int main( int argc, char* args[] ) {
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }     else
    {
        //Create window
        window = SDL_CreateWindow( "Rat Hunt", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        } else
        {
        
            SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            
            Player player;

            //Hack to get window to stay up
            SDL_Event e;
            bool quit = false;
            while( quit == false ) { 
                while( SDL_PollEvent( &e ) ) { 
                    if( e.type == SDL_QUIT ) quit = true; 
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                for (int x = 0; x < NUM_RAYS; x++) {
                    double rayAngle = player.angle - FOV / 2 + FOV * x / static_cast<double>(NUM_RAYS);
                    castRay(renderer, player, rayAngle, x);
                }
                SDL_RenderPresent(renderer);
            }
            SDL_DestroyRenderer(renderer);
        }
    }
    

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
