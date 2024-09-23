#include <SDL2/SDL.h>
#include <stdio.h>
#include<vector>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;
const double FOV = 60 * M_PI / 180;
const int NUM_RAYS = SCREEN_WIDTH;

// Define map
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 1, 1, 0, 0, 1},
	{1, 0, 1, 0, 1, 0, 0, 1},
	{1, 0, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1}
};

// player's position on the map
double playerX = 1.5;
double playerY = 1.5;
double angle = 0; // camera angle

void castRay(SDL_Renderer* renderer, double rayAngle, int rayNum) {
    double rayDirX = cos(rayAngle);
	double rayDirY = sin(rayAngle);

    int mapX = floor(playerX);
	int mapY = floor(playerY);

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
		sideDistX = (playerX - mapX) * deltaDistX;
	} else {
		stepX = 1;
		sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
	}

	if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (playerY - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
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
        perpWallDist = (mapX - playerX + (1 - stepX) / 2) / rayDirX;
    } else {
        perpWallDist = (mapY - playerY + (1 - stepY) / 2) / rayDirY;
    }

    int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist);
    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawStart < 0) drawStart = 0;
    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

    // Drawing wall
    Uint8 wallColor = static_cast<Uint8>(255 / (1 + perpWallDist * perpWallDist * 0.1));
    
    Uint8 r = 3; 
    Uint8 g = 3; 
    Uint8 b = 3; 
    int y1 = 0;
    int y2 = drawStart;

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, rayNum, y1, rayNum, y2);

    
}



int main( int argc, char* args[] )
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;
    
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

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
                    double rayAngle = angle - FOV / 2 + FOV * x / static_cast<double>(NUM_RAYS);
                    castRay(renderer, rayAngle, x);
                }
            }
        }
    }
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
