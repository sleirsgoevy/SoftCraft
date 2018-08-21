#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

#include "config.h"

using namespace std;

int zmax(int n)
{
    return n >= 0 ? n : 0;
}

int count_neighbors(const vector<vector<vector<char>>>& world, int x, int y, int z)
{
    int ans = 27;
    for (int i = zmax(x - 1); i < WORLD_SIZE && i <= x + 1; i++)
        for (int j = zmax(y - 1); j < WORLD_SIZE && j <= y + 1; j++)
            for (int k = zmax(z - 1); k < WORLD_SIZE && k <= z + 1; k++)
                if (!world[i][j][k])
                    ans--;
    return ans;
}

vector<vector<vector<char>>> generate_caves()
{
    vector<vector<vector<char>>> world(WORLD_SIZE,
                                       vector<vector<char>>(WORLD_SIZE, vector<char>(WORLD_SIZE)));
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++)
            for (int k = 0; k < WORLD_SIZE; k++)
                world[i][j][k] = rand() < 0.5 * RAND_MAX;
    while (true) {
        int diff = 0;
        vector<vector<vector<char>>> world2 = world;
        for (int i = 0; i < WORLD_SIZE; i++)
            for (int j = 0; j < WORLD_SIZE; j++)
                for (int k = 0; k < WORLD_SIZE; k++) {
                    int c = count_neighbors(world, i, j, k);
                    if (c < 9)
                        world2[i][j][k] = false;
                    if (c >= 18)
                        world2[i][j][k] = true;
                }
        for (int i = 0; i < WORLD_SIZE; i++)
            for (int j = 0; j < WORLD_SIZE; j++)
                for (int k = 0; k < WORLD_SIZE; k++)
                    if (world[i][j][k] != world2[i][j][k]) {
                        world[i][j][k] = world2[i][j][k];
                        diff++;
                    }
        cout << "life3d diff " << diff << endl;
        if (diff == 0)
            break;
    }
    return world;
}
