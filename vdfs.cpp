#include <utility>
#include <vector>

using namespace std;

struct world {
    int width;
    int height;
    int depth;
    const vector<vector<vector<int>>>& data;
    vector<vector<vector<char>>> used;
    vector<vector<vector<char>>> used_cur;
    vector<vector<vector<char>>> used0;
    world(const vector<vector<vector<int>>>& data) : data(data)
    {
        width = data.size();
        height = data[0].size();
        depth = data[0][0].size();
        used = vector<vector<vector<char>>>(width,
                                            vector<vector<char>>(height, vector<char>(depth)));
        used_cur = used;
        used0 = used;
    }
};

static inline void vdfs(world& w, int x, int y, int z, int mask)
{
    if (x < 0 || y < 0 || z < 0 || x >= w.width || y >= w.height || z >= w.depth
        || w.used_cur[x][y][z])
        return;
    w.used_cur[x][y][z] = true;
    w.used[x][y][z] = true;
    if (w.data[x][y][z] >= 0)
        return;
    if (mask & 1)
        vdfs(w, x + 1, y, z, mask);
    else
        vdfs(w, x - 1, y, z, mask);
    if (mask & 2)
        vdfs(w, x, y + 1, z, mask);
    else
        vdfs(w, x, y - 1, z, mask);
    if (mask & 4)
        vdfs(w, x, y, z + 1, mask);
    else
        vdfs(w, x, y, z - 1, mask);
}

vector<vector<vector<char>>> vdfs_main(const vector<vector<vector<int>>>& data, int x, int y, int z)
{
    world w(data);
    for (int i = 0; i < 8; i++) {
        vdfs(w, x, y, z, i);
        w.used_cur = w.used0;
    }
    return w.used;
}
