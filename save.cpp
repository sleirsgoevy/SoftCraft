#include <vector>
#include <cstdlib>
#include <cstdio>
#include "config.h"

using namespace std;

int log2_bp(int n, int low, int high)
{
    if(high - low <= 1)
        return high;
    int mid = (high + low) / 2;
    if((((__int128)1) << mid) < n)
        return log2_bp(n, mid, high);
    else
        return log2_bp(n, low, mid);
}

int log2(int n)
{
    return log2_bp(n, -1, 127);
}

int coords_len()
{
    return (log2(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE) + 7) / 8;
}

vector<vector<vector<int> > > generate();

vector<vector<vector<int> > > read_world(const char* path, int& seed, vector<vector<vector<int> > >& world0)
{
    FILE* fd = fopen(path, "r");
    if(fd != NULL)
        fread(&seed, sizeof(int), 1, fd);
    srand(seed);
    vector<vector<vector<int> > > ans = generate();
    world0 = ans;
    if(fd == NULL)
        return ans;
    __int128 coords = 0;
    while(fread(&coords, 1, coords_len(), fd) == coords_len())
    {
        int blockdata;
        fread(&blockdata, sizeof(int), 1, fd);
        ans[coords % WORLD_SIZE][(coords / WORLD_SIZE) % WORLD_SIZE][(coords / WORLD_SIZE / WORLD_SIZE) % WORLD_SIZE] = blockdata;
    }
    fclose(fd);
    return ans;
}

void save_world(const char* path, int seed, const vector<vector<vector<int> > >& world0, const vector<vector<vector<int> > >& world)
{
    FILE* fd = fopen(path, "w");
    fwrite(&seed, sizeof(int), 1, fd);
    for(int i = 0; i < WORLD_SIZE; i++)
        for(int j = 0; j < WORLD_SIZE; j++)
            for(int k = 0; k < WORLD_SIZE; k++)
                if(world[i][j][k] != world0[i][j][k])
                {
                    __int128 coords = k;
                    coords = coords * WORLD_SIZE + j;
                    coords = coords * WORLD_SIZE + i;
                    fwrite(&coords, coords_len(), 1, fd);
                    fwrite(&world[i][j][k], sizeof(int), 1, fd);
                }
    fclose(fd);
}
