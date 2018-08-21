#include <cstdlib>
#include <utility>
#include <vector>

#include "config.h"

using namespace std;

vector<vector<int>> do_dsq(int, int, int, int);

vector<vector<vector<char>>> generate_caves();

vector<vector<vector<int>>> generate()
{
    vector<vector<int>> dsq1 = do_dsq(WORLD_SIZE, WORLD_SIZE / 4, 0, 1);
    vector<vector<int>> dsq2 = do_dsq(WORLD_SIZE, 0, 0, 1);
    vector<vector<vector<int>>> ans(WORLD_SIZE,
                                    vector<vector<int>>(WORLD_SIZE, vector<int>(WORLD_SIZE, -1)));
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++) {
            int height = min(dsq1[i][j] / 5, WORLD_SIZE);
            int dirt_height = dsq2[i][j] * 10 / WORLD_SIZE;
            int dirt_start = height - dirt_height;
            for (int k = 0; k < dirt_start; k++)
                ans[i][k][j] = STONE;
            for (int k = max(dirt_start, 0); k < height; k++)
                ans[i][k][j] = DIRT;
        }
    vector<vector<vector<char>>> caves = generate_caves();
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 1; j < WORLD_SIZE; j++)
            for (int k = 0; k < WORLD_SIZE; k++)
                if (!caves[i][j][k])
                    ans[i][j][k] = -1;
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 1; j < 11; j++)
            for (int k = 0; k < WORLD_SIZE; k++)
                if (ans[i][j][k] < 0)
                    ans[i][j][k] = LAVA;
    for (int i = 0; i < WORLD_SIZE; i++)
        for (int j = 0; j < WORLD_SIZE; j++)
            for (int k = WORLD_SIZE - 1; k >= 0; k--)
                if (ans[i][k][j] == DIRT) {
                    ans[i][k][j] = GRASS;
                    break;
                }
    for (int i = 0; i < WORLD_SIZE * WORLD_SIZE / 100; i++) {
        int x = rand() % (WORLD_SIZE - 2) + 1;
        int z = rand() % (WORLD_SIZE - 2) + 1;
        int y = min(dsq1[x][z] / 5, WORLD_SIZE);
        for (int i = 0; i < 4; i++)
            ans[x][y + i][z] = WOOD;
        for (int i = 2; i < 4; i++)
            for (int j = -1; j <= 1; j++)
                for (int k = -1; k <= 1; k++)
                    if (ans[x + j][y + i][z + k] < 0)
                        ans[x + j][y + i][z + k] = LEAVES;
        ans[x][y + 4][z] = LEAVES;
    }
    return ans;
}
