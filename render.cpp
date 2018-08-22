#include <algorithm>
#include <atomic>
#include <cmath>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include "config.h"
#include "segtree.h"

using namespace std;

struct worker_shared;

struct screen {
    int* data;
    int width;
    int height;
    void** predata;
    worker_shared* ws;
};

struct player_pos;

struct coords {
    double x;
    double y;
    double z;
    coords(double x, double y, double z) : x(x), y(y), z(z){};
    coords operator/(const player_pos& pos);
};

struct player_pos {
    coords c;
    double yaw;
    double pitch;
};

coords coords::operator/(const player_pos& pos)
{
    coords ans(x - pos.c.x, y - pos.c.y, z - pos.c.z);
    ans = coords(ans.x * cos(pos.yaw) - ans.z * sin(pos.yaw),
                 ans.y,
                 ans.x * sin(pos.yaw) + ans.z * cos(pos.yaw));
    ans = coords(ans.x,
                 ans.z * sin(pos.pitch) + ans.y * cos(pos.pitch),
                 ans.z * cos(pos.pitch) - ans.y * sin(pos.pitch));
    return ans;
}

template <class T>
struct iteration_order {
    const vector<T> v;
    int base_index;
    struct iterator {
        const iteration_order* io;
        int i;
        iterator(const iteration_order& io, int i) : io(&io), i(i){};
        iterator& operator++()
        {
            i++;
            return *this;
        }
        const pair<int, const T&> operator*()
        {
            int idx = io->get(i);
            return pair<int, const T&>(idx, io->v[idx]);
        }
        bool operator!=(const iterator& other)
        {
            return other.i != i;
        }
    };
    iteration_order(const vector<T>& v, int base_index) : v(v), base_index(max(base_index, 0)){};
    const int get(int i) const
    {
        if (i < base_index)
            return i;
        else
            return v.size() - 1 - i + base_index;
    }
    iterator begin() const
    {
        return iterator(*this, 0);
    }
    iterator end() const
    {
        return iterator(*this, v.size());
    }
};

void preDrawRect(screen& canvas,
                 const coords& p1,
                 const coords& p2,
                 const coords& p3,
                 const coords& p4,
                 void* color);

void drawTexture(screen& canvas,
                 const coords& p1,
                 const coords& p2,
                 const coords& p3,
                 const coords& p4,
                 int color);

template <class T>
void drawRectSomehow(screen& canvas,
                     const coords& p1,
                     const coords& p2,
                     const coords& p3,
                     const coords& p4,
                     T color)
{
}

template <>
void drawRectSomehow<int>(screen& canvas,
                          const coords& p1,
                          const coords& p2,
                          const coords& p3,
                          const coords& p4,
                          int color)
{
    return drawTexture(canvas, p1, p2, p3, p4, color);
}

template <>
void drawRectSomehow<void*>(screen& canvas,
                            const coords& p1,
                            const coords& p2,
                            const coords& p3,
                            const coords& p4,
                            void* color)
{
    return preDrawRect(canvas, p1, p2, p3, p4, color);
}

template <class T>
void drawBlock(screen& canvas,
               const vector<vector<vector<int>>>& w,
               int x,
               int y,
               int z,
               const player_pos& pos,
               T color)
{
    if (pos.c.x < x && x > 0 && w[x - 1][y][z] < 0)
        drawRectSomehow(canvas,
                        coords(x, y, z) / pos,
                        coords(x, y + 1, z) / pos,
                        coords(x, y + 1, z + 1) / pos,
                        coords(x, y, z + 1) / pos,
                        color);
    if (pos.c.x > x + 1 && x < w.size() - 1 && w[x + 1][y][z] < 0)
        drawRectSomehow(canvas,
                        coords(x + 1, y, z) / pos,
                        coords(x + 1, y + 1, z) / pos,
                        coords(x + 1, y + 1, z + 1) / pos,
                        coords(x + 1, y, z + 1) / pos,
                        color);
    if (pos.c.y < y && y > 0 && w[x][y - 1][z] < 0)
        drawRectSomehow(canvas,
                        coords(x, y, z) / pos,
                        coords(x + 1, y, z) / pos,
                        coords(x + 1, y, z + 1) / pos,
                        coords(x, y, z + 1) / pos,
                        color);
    if (pos.c.y > y + 1 && y < w[x].size() - 1 && w[x][y + 1][z] < 0)
        drawRectSomehow(canvas,
                        coords(x, y + 1, z) / pos,
                        coords(x + 1, y + 1, z) / pos,
                        coords(x + 1, y + 1, z + 1) / pos,
                        coords(x, y + 1, z + 1) / pos,
                        color);
    if (pos.c.z < z && z > 0 && w[x][y][z - 1] < 0)
        drawRectSomehow(canvas,
                        coords(x, y, z) / pos,
                        coords(x + 1, y, z) / pos,
                        coords(x + 1, y + 1, z) / pos,
                        coords(x, y + 1, z) / pos,
                        color);
    if (pos.c.z > z + 1 && z < w[x][y].size() - 1 && w[x][y][z + 1] < 0)
        drawRectSomehow(canvas,
                        coords(x, y, z + 1) / pos,
                        coords(x + 1, y, z + 1) / pos,
                        coords(x + 1, y + 1, z + 1) / pos,
                        coords(x, y + 1, z + 1) / pos,
                        color);
}

vector<vector<vector<char>>> vdfs_main(const vector<vector<vector<int>>>&, int, int, int);

struct block_pos {
    int x;
    int y;
    int z;
};

struct worker_shared {
    int num_workers = 1;
    const vector<vector<block_pos>>* layers = NULL;
    const vector<vector<vector<int>>>* world = NULL;
    screen* canvas = NULL;
    const player_pos* pos = NULL;
    int work_type = 0;
    atomic<int> work_done;
    worker_shared()
    {
        work_done = -1;
    }
};

/*struct worker
{
    int idx;
    worker_shared* ws;
    worker(int idx, worker_shared* ws) : idx(idx), ws(ws){};
};*/

void worker(worker_shared* ws, int idx, bool once)
{
    do {
        while (ws->work_done && ws->work_done >= ws->num_workers)
            ;
        int work_type = ws->work_type;
        screen& canvas = *ws->canvas;
        const player_pos& pos = *ws->pos;
        const vector<vector<vector<int>>>& world = *ws->world;
        for (int i = 0; i < ws->layers->size(); i++) {
            while (ws->work_done < (ws->num_workers * i))
                std::this_thread::yield();
            for (int j = idx; j < ws->layers[0][i].size(); j += max(ws->num_workers, 1)) {
                const block_pos& bp = ws->layers[0][i][j];
                if (world[bp.x][bp.y][bp.z] >= 0)
                    if (work_type == 0)
                        drawBlock(canvas, world, bp.x, bp.y, bp.z, pos, (void*)&bp);
                    else
                        drawBlock(canvas, world, bp.x, bp.y, bp.z, pos, world[bp.x][bp.y][bp.z]);
            }
            ++ws->work_done;
        }
    } while (!once);
}

worker_shared* start_workers(screen& canvas)
{
    worker_shared* ws = new worker_shared;
    ws->num_workers = WORKERS;
    ws->canvas = &canvas;
    ws->work_done = ws->num_workers;
    for (int i = 0; i < WORKERS; i++)
        new thread(worker, ws, i, false);
    return ws;
}

vector<vector<block_pos>> split_layers(const vector<vector<vector<int>>>& world,
                                       const player_pos& pos,
                                       const vector<vector<vector<char>>>& visible)
{
    int x0 = floor(pos.c.x);
    int y0 = floor(pos.c.y);
    int z0 = floor(pos.c.z);
    vector<vector<block_pos>> ans;
    for (int x = 0; x < world.size(); x++)
        for (int y = 0; y < world[x].size(); y++)
            for (int z = 0; z < world[x][y].size(); z++) {
                block_pos bp;
                bp.x = x;
                bp.y = y;
                bp.z = z;
                int level = abs(x - x0) + abs(y - y0) + abs(z - z0);
                if (visible[x][y][z] && level) {
                    if (ans.size() < level)
                        ans.resize(level);
                    ans[level - 1].push_back(bp);
                }
            }
    reverse(ans.begin(), ans.end());
    return ans;
}

void render(const vector<vector<vector<int>>>& world, const player_pos& pos, screen& canvas)
{
    if (canvas.ws == NULL)
        canvas.ws = start_workers(canvas);
    /*  if(canvas.tr == NULL)
        {
            canvas.tr = new segtree[canvas.height];
            for(int i = 0; i < canvas.height; i++)
                canvas.tr[i] = segtree(0, canvas.width);
        }*/
    for (int i = 0; i < canvas.height; i++)
        for (int j = 0; j < canvas.width; j++)
            canvas.data[i * canvas.width + j] = 0xffffff;
    for (int i = 0; i < canvas.height; i++)
        for (int j = 0; j < canvas.width; j++)
            canvas.predata[i * canvas.width + j] = NULL;
    vector<vector<vector<char>>> is_visible = vdfs_main(
            world, floor(pos.c.x), floor(pos.c.y), floor(pos.c.z));
    int cnt = 0;
    // vector<void*> blockdata;
    /*for(auto& i : iteration_order<vector<vector<int> > >(world, floor(pos.c.x)))
        for(auto& j : iteration_order<vector<int> >(i.second, floor(pos.c.y)))
            for(auto& k : iteration_order<int>(j.second, floor(pos.c.z)))
                if(k.second >= 0 && is_visible[i.first][j.first][k.first])
                {
                    block_pos* bp = new block_pos;
                    bp->x = i.first;
                    bp->y = j.first;
                    bp->z = k.first;
                    blockdata.push_back((void*)bp);
                    drawBlock(canvas, world, i.first, j.first, k.first, pos,
       (void*)bp); cnt++;
                }*/
    vector<vector<block_pos>> layers = split_layers(world, pos, is_visible);
    if (layers.size() == 0)
        return;
    canvas.ws->layers = &layers;
    canvas.ws->world = &world;
    canvas.ws->pos = &pos;
    canvas.ws->work_type = 0;
    asm volatile("" ::: "memory");
    canvas.ws->work_done = 0;
#if WORKERS == 0
    worker(canvas.ws, 0, true);
#endif
    while (canvas.ws->work_done < layers.size() * canvas.ws->num_workers)
        std::this_thread::yield();
    for (vector<vector<char>>& i : is_visible)
        for (vector<char>& j : i)
            for (int k = 0; k < j.size(); k++)
                j[k] = false;
    for (int i = 0; i < canvas.height; i++)
        for (int j = 0; j < canvas.width; j++) {
            block_pos* cur = (block_pos*)canvas.predata[i * canvas.width + j];
            if (cur != NULL)
                is_visible[cur->x][cur->y][cur->z] = true;
        }
    layers = split_layers(world, pos, is_visible);
    if (layers.size() == 0)
        return;
    canvas.ws->layers = &layers;
    canvas.ws->world = &world;
    canvas.ws->pos = &pos;
    canvas.ws->work_type = 1;
    asm volatile("" ::: "memory");
    canvas.ws->work_done = 0;
#if WORKERS == 0
    worker(canvas.ws, 0, true);
#endif
    while (canvas.ws->work_done < layers.size() * canvas.ws->num_workers)
        ;
    /*for(auto& i : iteration_order<vector<vector<int> > >(world, floor(pos.c.x)))
        for(auto& j : iteration_order<vector<int> >(i.second, floor(pos.c.y)))
            for(auto& k : iteration_order<int>(j.second, floor(pos.c.z)))
                if(k.second >= 0 && is_visible[i.first][j.first][k.first])
                {
                    drawBlock(canvas, world, i.first, j.first, k.first, pos,
       k.second); cnt++;
                }*/
    // cout << cnt << " blocks rendered." << endl;
}
