#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "config.h"

using namespace std;

struct rgb24 {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    int val()
    {
        return r * 65536 + g * 256 + b;
    }
} __attribute__((packed));

int* load_texture(const string& file)
{
    ifstream fin("textures/" + file + ".bmp");
    vector<char> buf = vector<char>(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
    char* data = &(buf[0]);
    int* src = (int*)(data + *((int*)(data + 10)));
    int* ans = new int[256];
    for (int i = 0; i < 256; i++)
        ans[i] = src[i];
    return ans;
}

unordered_map<int, int*> textures;

void init_textures()
{
    textures[STONE] = load_texture("stone");
    textures[DIRT] = load_texture("dirt");
    textures[WOOD] = load_texture("wood");
    textures[GRASS] = load_texture("grass");
    textures[LEAVES] = load_texture("leaves");
    textures[LAVA] = load_texture("lava");
}
