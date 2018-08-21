#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include "config.h"

using namespace std;

struct player_pos {
    double x;
    double y;
    double z;
};

void collide_block(player_pos& pos, int x, int y, int z)
{
    double xmin = x - PLAYER_EPS;
    double xmax = x + 1 + PLAYER_EPS;
    double ymin = y - PLAYER_EPS;
    double ymax = y + 1 + PLAYER_HEIGHT;
    double zmin = z - PLAYER_EPS;
    double zmax = z + 1 + PLAYER_EPS;
    if (pos.x < xmin || pos.x > xmax || pos.y < ymin || pos.y > ymax || pos.z < zmin
        || pos.z > zmax)
        return;
    double d1 = pos.x - xmin;
    double d2 = xmax - pos.x;
    double d3 = pos.y - ymin;
    double d4 = ymax - pos.y;
    double d5 = pos.z - zmin;
    double d6 = zmax - pos.z;
    double dmin = min({d1, d2, d3, d4, d5, d6});
    if (dmin == d1)
        pos.x = xmin;
    else if (dmin == d2)
        pos.x = xmax;
    else if (dmin == d3)
        pos.y = ymin;
    else if (dmin == d4)
        pos.y = ymax;
    else if (dmin == d5)
        pos.z = zmin;
    else if (dmin == d6)
        pos.z = zmax;
}

void collide(player_pos& pos, const vector<vector<vector<int>>>& world)
{
    int x = floor(pos.x);
    int y = floor(pos.y);
    int z = floor(pos.z);
    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 2; j <= y; j++)
            for (int k = z - 1; k <= z + 1; k++)
                if (i < 0 || i >= world.size() || j < 0 || j >= world[i].size() || k < 0
                    || k >= world[i][j].size() || world[i][j][k] >= 0)
                    collide_block(pos, i, j, k);
}
