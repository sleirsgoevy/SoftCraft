#include <chrono>
#include <cmath>
#include <cstdlib>
#include <vector>

#include <SDL2/SDL.h>

#include "config.h"

using namespace std;

struct player_pos {
    double x;
    double y;
    double z;
    double yaw;
    double pitch;
    player_pos(double x, double y, double z, double yaw = 0, double pitch = 0) :
            x(x),
            y(y),
            z(z),
            yaw(yaw),
            pitch(pitch){};
};

struct block_pos {
    int x;
    int y;
    int z;
    int side;
};

struct screen {
    int* data;
    int width;
    int height;
    block_pos* predata;
    void* extra = NULL;
    screen(int* data, int width, int height) : data(data), width(width), height(height)
    {
        predata = NULL;
    }
    block_pos pre_middle()
    {
        return predata[width * (height / 2) + width / 2];
    }
};

/*template<class T>
T generate()
{
    T ans;
    for(int i = 0; i < WORLD_SIZE; i++)
        ans.push_back(generate<typename T::value_type>());
    return ans;
}

template<>
int generate<int>()
{
    int colors[] = {-1, -1, -1, -1, -1, -1, RED, GREEN, BLUE};
    return colors[rand() % 9];
}*/

//vector<vector<vector<int>>> generate();

vector<vector<vector<int> > > read_world(const char* path, int& seed, vector<vector<vector<int> > >& world0);

void save_world(const char* path, int seed, const vector<vector<vector<int> > >& world0, const vector<vector<vector<int> > >& world);

void render(const vector<vector<vector<int>>>&, const player_pos&, screen&);

void collide_block(player_pos& pos, int x, int y, int z);
void collide(player_pos& pos, const vector<vector<vector<int>>>& world);

void init_textures();

const int inventory[] = {STONE, DIRT, WOOD, GRASS, LEAVES, LAVA};

int main(int argc, char** argv)
{
    if(argc < 2 || argc > 3)
    {
        fprintf(stderr, "usage: %s <save> [seed]\n", argv[0]);
        return 1;
    }
    int seed = 1;
    if (argc == 3)
        seed = atoi(argv[2]);
    const char* save_path = argv[1];
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);
    SDL_Window* window = SDL_CreateWindow(
            "SoftCraft", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    init_textures();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    if (!window) {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
    int* buffer = (int*)malloc(640 * 480 * 4);
    if (!buffer) {
        printf("Unable to allocate frame buffer\n");
        return 1;
    }
    screen scr(buffer, 640, 480);
    vector<vector<vector<int> > > world0;
    vector<vector<vector<int>>> world = read_world(save_path, seed, world0);//generate /*<vector<vector<vector<int> > > >*/ ();
    player_pos pos(WORLD_SIZE / 2, WORLD_SIZE / 2, WORLD_SIZE / 2);
    int movement_ws = 0;
    int movement_ad = 0;
    int movement_y = 0;
    auto start_time = chrono::high_resolution_clock::now();
    long long ticks = 0;
    double yspeed = 0;
    int curr_block = STONE;
    while (true) {
        render(world, pos, scr);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: goto save_world;
            case SDL_KEYDOWN: {
                if (event.key.repeat == 0)
                {
                    switch (event.key.keysym.sym) {
                    case SDLK_w: movement_ws++; break;
                    case SDLK_s: movement_ws--; break;
                    case SDLK_a: movement_ad--; break;
                    case SDLK_d: movement_ad++; break;
                    case SDLK_SPACE:
                        yspeed = JUMP_SPEED;
                        break;
                        //                      case SDLK_SPACE: movement_y++; break;
                        //                      case SDLK_LSHIFT: movement_y--; break;
                    }
                    if(event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_6)
                        curr_block = inventory[event.key.keysym.sym - SDLK_1];
                }
                break;
            }
            case SDL_KEYUP: {
                if (event.key.repeat == 0)
                    switch (event.key.keysym.sym) {
                    case SDLK_w: movement_ws--; break;
                    case SDLK_s: movement_ws++; break;
                    case SDLK_a: movement_ad++; break;
                    case SDLK_d:
                        movement_ad--;
                        break;
                        //                      case SDLK_SPACE: movement_y--; break;
                        //                      case SDLK_LSHIFT: movement_y++; break;
                    }
                break;
            }
            case SDL_MOUSEMOTION: {
                pos.yaw += event.motion.xrel * MOUSE_SENSIVITY;
                pos.pitch += event.motion.yrel * MOUSE_SENSIVITY;
                if (pos.yaw < M_PI)
                    pos.yaw += 2 * M_PI;
                if (pos.yaw > M_PI)
                    pos.yaw -= 2 * M_PI;
                pos.pitch = min(M_PI / 2, max(-M_PI / 2, pos.pitch));
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                block_pos pointed = scr.pre_middle();
                if(pointed.side >= 0)
                    if(event.button.button == SDL_BUTTON_LEFT)
                        world[pointed.x][pointed.y][pointed.z] = -1;
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                    {
                        switch(pointed.side)
                        {
                            case 0: pointed.x--; break;
                            case 1: pointed.x++; break;
                            case 2: pointed.y--; break;
                            case 3: pointed.y++; break;
                            case 4: pointed.z--; break;
                            case 5: pointed.z++; break;
                        }
                        if(pointed.x >= 0 && pointed.y >= 0 && pointed.z >= 0 && pointed.x < WORLD_SIZE && pointed.y < WORLD_SIZE && pointed.z < WORLD_SIZE)
                        {
                            player_pos pos2 = pos;
                            collide_block(pos2, pointed.x, pointed.y, pointed.z);
                            if(pos2.x == pos.x && pos2.y == pos.y && pos2.z == pos.z)
                                world[pointed.x][pointed.y][pointed.z] = curr_block;\
                        }
                    }
                break;
                }
            }
        }
        SDL_UpdateTexture(texture, NULL, buffer, 640 * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        auto timer2 = chrono::high_resolution_clock::now();
        long long ticks2 = chrono::duration_cast<chrono::microseconds>(
                                   chrono::high_resolution_clock::now() - start_time)
                                   .count()
                           / TICK_DURATION;
        for (long long i = ticks; i < ticks2; i++) {
            yspeed -= GRAVITY;
            player_pos pos2 = pos;
            pos2.z += (movement_ws * cos(pos.yaw) - movement_ad * sin(pos.yaw)) * SPEED;
            pos2.x += (movement_ws * sin(pos.yaw) + movement_ad * cos(pos.yaw)) * SPEED;
            collide(pos2, world);
            pos2.y += yspeed;
            collide(pos2, world);
            if (pos2.y == pos.y)
                yspeed = 0;
            pos = pos2;
        }
        ticks = ticks2;
    }
save_world:
    save_world(save_path, seed, world0, world);
    return 0;
}
