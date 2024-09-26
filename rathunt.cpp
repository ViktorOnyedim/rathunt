#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <cmath>
#include<vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// const int MAP_WIDTH = 8;
// const int MAP_HEIGHT = 8;
const double FOV = 60 * M_PI / 180;
const int NUM_RAYS = SCREEN_WIDTH;

// Rotation constants
const double ROTATION_SPEED = 0.05;
const double MOUSE_SENSITIVITY = 0.002;

// Movement constant
const double MOVEMENT_SPEED = 0.05;
const double COLLISION_MARGIN = 0.1;

const int TEXTURE_WIDTH = 64;
const int TEXTURE_HEIGHT = 64;
const int NUM_TEXTURES = 4;

// Define map
// std::vector<std::vector<int>> map = {
//     {1, 1, 1, 1, 1, 1, 1, 1},
// 	{1, 0, 0, 0, 0, 0, 0, 1},
// 	{1, 0, 1, 1, 0, 1, 0, 1},
// 	{1, 0, 1, 1, 1, 0, 0, 1},
// 	{1, 0, 1, 0, 1, 0, 0, 1},
// 	{1, 0, 1, 1, 1, 0, 0, 1},
// 	{1, 0, 0, 0, 0, 0, 0, 1},
// 	{1, 1, 1, 1, 1, 1, 1, 1}
// };

// player's position on the map
struct Player {
    double x = 1.5;
    double y = 1.5;
    double angle = 0.0; // camera angle
    // Direction vectors
    double dirX = 1.0;
    double dirY = 0.0;
    double planeX = 0.0;
    double planeY = 0.66;
};

class Map {
public:
    std::vector<std::vector<int>> data;
    int width = 0;
    int height = 0;

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '3') continue; // Skip empty lines and comments

            std::vector<int> row;
            for (char c : line) {
                if (c == '#') {
                    row.push_back(1); // Wall
                } else if (c == '.') {
                    row.push_back(0); // Empty space
                } else {
                    throw std::runtime_error("Invalid character in map file: " + std::string(1, c));
                }
            }

            if (!data.empty() && row.size() != data[0].size()) {
                throw std::runtime_error("Inconsistent row length in map file");
            }

            data.push_back(row);
        }

        if (data.empty()) {
            throw std::runtime_error("Empty map file");
        }

        height = data.size();
        width = data[0].size();
    }

    bool isWall(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return true;  // Treat out of bounds as walls
        }
        return data[y][x] != 0;
    }
};


Map map;

void castRay(SDL_Renderer* renderer, const Player& player, double rayAngle, int rayNum) {
    double rayDirX = cos(rayAngle);
	double rayDirY = sin(rayAngle);

    int mapX = static_cast<int>(player.x);
	int mapY = static_cast<int>(player.y);

    // Distance ray has to travel from start pos to the 1st x-side & y-side
    double sideDistX;
	double sideDistY;

    // distance ray has to travel to go from 1 x-side to the next, or for y
    double deltaDistX = std::abs(1 / rayDirX);
	double deltaDistY = std::abs(1 / rayDirY);
	double perpWallDist; // used to calculate length of the ray

    // What direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    int hitWall = 0;
    int side; // was a NS or a EW wall hit?

    // calculate step and initial sideDist
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

    // perform DDA
    while (hitWall == 0) {
        // jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }
        // Check if ray has hit a wall
        if (map.isWall(mapX, mapY)) hitWall = 1;
    }

    // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    if (side == 0) {
        perpWallDist = (mapX - player.x + (1 - stepX) / 2) / rayDirX;
    } else {
        perpWallDist = (mapY - player.y + (1 - stepY) / 2) / rayDirY;
    }

    // Calculate height of line to draw on screen
    int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist);

    // Calculate lowest and highest pixel to fill in current stripe
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



void move_player(Player& player, double dirX, double dirY) {
    double newX = player.x + dirX * MOVEMENT_SPEED;
    double newY = player.y + dirY * MOVEMENT_SPEED;

    /* Detect collision */

    /* check if the space the player is trying to move to 
    along the y-axis) is not blocked */


    if (!map.isWall(static_cast<int>(newX), static_cast<int>(player.y))) {
        player.x = newX;
    }
    if (!map.isWall(static_cast<int>(player.x), static_cast<int>(newY))) {
        player.y = newY;
    }
}

SDL_Texture* textures[NUM_TEXTURES];
SDL_Texture* weaponTexture;

void loadTextures(SDL_Renderer* renderer) {
    
    // Load weapon texture
    SDL_Surface* weaponSurface = IMG_Load("weapon.png");
    if (!weaponSurface) {
        printf("Failed to load weapon texture: %s\n", SDL_GetError());
        exit(1);
    }
    weaponTexture = SDL_CreateTextureFromSurface(renderer, weaponSurface);
    SDL_FreeSurface(weaponSurface);


}

void renderWeapon(SDL_Renderer* renderer) {
    int weaponWidth = (SCREEN_WIDTH / 2) - 40;
    int weaponHeight = (SCREEN_HEIGHT / 2) - 40;
    int weaponX = (SCREEN_WIDTH - weaponWidth) / 2;
    int weaponY = SCREEN_HEIGHT - weaponHeight;

    SDL_Rect dstRect = {weaponX, weaponY, weaponWidth, weaponHeight};
    SDL_RenderCopy(renderer, weaponTexture, NULL, &dstRect);
}


void render(SDL_Renderer* renderer, const Player& player) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    for (int x = 0; x < NUM_RAYS; x++) {
        // Calculate ray position and direction
        double cameraX = 2 * x / static_cast<double>(SCREEN_WIDTH) - 1; // x-coordinate in camea space
        double rayDirX = player.dirX + player.planeX * cameraX;
        double rayDirY = player.dirY + player.planeY * cameraX;
        double rayAngle = atan2(rayDirY, rayDirX);
        castRay(renderer, player, rayAngle, x);
    }

    renderWeapon(renderer);
    

    SDL_RenderPresent(renderer);
}


int main( int argc, char* args[] ) {
    if (argc != 2) {
        std::cerr << "Usage: " << args[0] << " <map_file_path>" << std::endl;
        return 1;
    }

    
    try {
        map.loadFromFile(args[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error loading map: " << e.what() << std::endl;
        return 1;
    }

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
            
            loadTextures(renderer);

            Player player;


            // get relative motion data in SDL_MOUSEMOTION events.
            SDL_SetRelativeMouseMode(SDL_TRUE);

            //
            const Uint8* key_state = SDL_GetKeyboardState(NULL);

            //Hack to get window to stay up
            SDL_Event e;
        
            bool quit = false;
            while( quit == false ) { 
                while( SDL_PollEvent( &e ) ) { 
                    if( e.type == SDL_QUIT ) {
                        quit = true; 
                    } else if (e.type == SDL_MOUSEMOTION) {
                        player.angle += e.motion.xrel * MOUSE_SENSITIVITY;
                    } else if (e.type == SDL_KEYDOWN) {
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_LEFT:
                            player.angle -= ROTATION_SPEED;
                            break;
                        case SDLK_RIGHT:
                            player.angle += ROTATION_SPEED;
                        default:
                            break;
                        }
                    }
                } 

                // Update player direction and camera plane
                player.dirX = cos(player.angle);
                player.dirY = sin(player.angle);
                player.planeX = -player.dirY * 0.66;
                player.planeY = player.dirX * 0.66;

                // Handle player movement
                double moveX = 0;
                double moveY = 0;
                if (key_state[SDL_SCANCODE_W]) {
                    moveX += player.dirX;
                    moveY += player.dirY;
                    // move_player(player, player.dirX, player.dirY);
                }
                if (key_state[SDL_SCANCODE_S]) {
                    moveX -= player.dirX;
                    moveY -= player.dirY;
                    // move_player(player, -player.dirX, -player.dirY);
                }
                if (key_state[SDL_SCANCODE_D]) {
                    moveX -= player.dirY;
                    moveY += player.dirX;

                    
                    // move_player(player, -player.dirY, player.dirX);
                }
                if (key_state[SDL_SCANCODE_A]) {
                    moveX += player.dirY;
                    moveY -= player.dirX;
                    // move_player(player, player.dirY, -player.dirX);
                }

                if (moveX != 0 || moveY != 0) {
                    double length = sqrt(moveX * moveX + moveY * moveY);
                    moveX /= length;
                    moveY /= length;
                    move_player(player, moveX, moveY);
                }

                render(renderer, player);
                
                
            }

            SDL_DestroyTexture(weaponTexture);
            for (int i = 0; i < NUM_TEXTURES; i++) {
                SDL_DestroyTexture(textures[i]);
            }   
            SDL_DestroyRenderer(renderer);
        }
    }
    

    //Destroy window
    SDL_DestroyWindow( window );
    IMG_Quit();
    SDL_Quit();

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
