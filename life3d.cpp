#include "config.h"
#include <vector>
#include <utility>
#include <cstdlib>
#include <iostream>

using namespace std;

int count_neighbors(const vector<vector<vector<bool> > >& world, int x, int y, int z)
{
    int ans = 27;
    for(int i = max(x - 1, 0); i < WORLD_SIZE && i <= x + 1; i++)
        for(int j = max(y - 1, 0); j < WORLD_SIZE && j <= y + 1; j++)
            for(int k = max(z - 1, 0); k < WORLD_SIZE && k <= z + 1; k++)
                if(!world[i][j][k])
                    ans--;
    return ans;
}

vector<vector<vector<bool> > > generate_caves()
{
    vector<vector<vector<bool> > > world(WORLD_SIZE, vector<vector<bool> >(WORLD_SIZE, vector<bool>(WORLD_SIZE)));
    for(int i = 0; i < WORLD_SIZE; i++)
        for(int j = 0; j < WORLD_SIZE; j++)
            for(int k = 0; k < WORLD_SIZE; k++)
                world[i][j][k] = rand() < 0.5 * RAND_MAX;
    while(true)
    {
        int diff = 0;
        vector<vector<vector<bool> > > world2 = world;
        for(int i = 0; i < WORLD_SIZE; i++)
            for(int j = 0; j < WORLD_SIZE; j++)
                for(int k = 0; k < WORLD_SIZE; k++)
                {
                    int c = count_neighbors(world, i, j, k);
                    if(c < 9)
                        world2[i][j][k] = false;
                    if(c >= 18)
                        world2[i][j][k] = true;
                }
        for(int i = 0; i < WORLD_SIZE; i++)
            for(int j = 0; j < WORLD_SIZE; j++)
                for(int k = 0; k < WORLD_SIZE; k++)
                    if(world[i][j][k] != world2[i][j][k])
                    {
                        world[i][j][k] = world2[i][j][k];
                        diff++;
                    }
        cout << "life3d diff " << diff << endl;
        if(diff == 0)
            break;
    }
    return world;
}
