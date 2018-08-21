#include "config.h"
#include <utility>
#include <algorithm>
//#include "segtree.h"
#include <unordered_map>

using namespace std;

struct screen
{
    int* data;
    int width; 
    int height;
    void** predata;
    operator int*()
    {
        return data;
    }
    operator void**()
    {
        return predata;
    }
    template<class T>
    void putpixel(int x, int y, T color)
    {
        ((T*)*this)[width * y + x] = color;
    }
};

struct coords
{
    double x;
    double y;
    double z;
    pair<int, int> project(const screen& scr) const
    {
        return make_pair(scr.width / 2.0 + (scr.height * SCALE * x) / z, scr.height / 2.0 - (scr.height * SCALE * y) / z);
    }
    bool operator<(const coords& other) const
    {
        return z < other.z;
    }
    static coords texture_pos(const coords& p1, const coords& p2, const coords& p3, const coords& p4, double x, double y)
    {
        coords ans;
        ans.x = p1.x + (p2.x - p1.x) * x + (p4.x - p1.x) * y;
        ans.y = p1.y + (p2.y - p1.y) * x + (p4.y - p1.y) * y;
        ans.z = p1.z + (p2.z - p1.z) * x + (p4.z - p1.z) * y;
        return ans;
    }
};

template<class T>
void drawLine(screen& scr, pair<int, int> pos1, pair<int, int> pos2, T color)
{
    if(pos1 == pos2)
    {
        if(pos1.first >= 0 && pos1.first < scr.width && pos2.second >= 0 && pos2.second < scr.height)
            scr.putpixel(pos1.first, pos2.second, color);
        return;
    }
    if(abs(pos1.first - pos2.first) < abs(pos1.second - pos2.second))
    {
        if(pos1.second > pos2.second)
            swap(pos1, pos2);
        for(int i = max(pos1.second, 0); i < scr.height && i <= pos2.second; i++)
        {
            int j = pos1.first + (pos2.first - pos1.first) * (long long)(i - pos1.second) / (pos2.second - pos1.second);
            if(j >= 0 && j < scr.width)
                scr.putpixel(j, i, color);
        }
    }
    else
    {
        if(pos1.first > pos2.first)
            swap(pos1, pos2);
        for(int i = max(pos1.first, 0); i < scr.width && i <= pos2.first; i++)
        {
            int j = pos1.second + (pos2.second - pos1.second) * (long long)(i - pos1.first) / (pos2.first - pos1.first);
            if(j >= 0 && j < scr.height)
                scr.putpixel(i, j, color);
        }
    }
}

template<class T>
void drawLine3d(screen& scr, coords pos1, coords pos2, T color)
{
    if(pos1.z <= EPS && pos2.z <= EPS)
        return; 
    if(pos1.z <= EPS)
    {
        pos1.x += (pos2.x - pos1.x) * (EPS - pos1.z) / (pos2.z - pos1.z);
        pos1.y += (pos2.y - pos1.y) * (EPS - pos1.z) / (pos2.z - pos1.z);
        pos1.z = EPS;
    }
    if(pos2.z <= EPS)
    {
        pos2.x += (pos1.x - pos2.x) * (EPS - pos2.z) / (pos1.z - pos2.z);
        pos2.y += (pos1.y - pos2.y) * (EPS - pos2.z) / (pos1.z - pos2.z);
        pos2.z = EPS;
    }
    drawLine(scr, pos1.project(scr), pos2.project(scr), color);
}

bool second_comp(const pair<int, int>& a, const pair<int, int>& b)
{
    return a.second < b.second || (a.second == b.second && a.first < b.first);
}

template<class T>
void drawTriangle(screen& scr, pair<int, int> pos1, pair<int, int> pos2, pair<int, int> pos3, T color)
{
    pair<int, int> arr[] = {pos1, pos2, pos3};
    sort(arr, arr + 3, second_comp);
    if(arr[0].second == arr[2].second)
        return;
    for(int y = max(arr[0].second, 0); y <= arr[2].second && y < scr.height; y++)
    {
        int x1 = arr[0].first + (arr[2].first - arr[0].first) * (long long)(y - arr[0].second) / (arr[2].second - arr[0].second);
        int x2;
        if(y < arr[1].second || arr[1].second == arr[2].second)
            x2 = arr[0].first + (arr[1].first - arr[0].first) * (long long)(y - arr[0].second) / (arr[1].second - arr[0].second);
        else
            x2 = arr[1].first + (arr[2].first - arr[1].first) * (long long)(y - arr[1].second) / (arr[2].second - arr[1].second);
        if(x1 > x2)
            swap(x1, x2);
        /*for(int x = max(x1, 0); x <= x2 && x < scr.width; x++)
            scr.putpixel(x, y, color);*/
        for(int x = max(x1, 0); x <= x2 && x < scr.width; x++)
            scr.putpixel(x, y, color);
    }
}

template<class T>
void drawRectangle(screen& scr, pair<int, int> pos1, pair<int, int> pos2, pair<int, int> pos3, pair<int, int> pos4, T color)
{
/*  pair<int, int> arr[] = {pos1, pos2, pos3, pos4};
    sort(arr, arr + 4);
    if(arr[0].first == arr[3].first)
        return;
    for(int x = max(arr[0].first, 0); x <= arr[3].first && x < scr.width; x++)
    {
        int y1;
        if(x < arr[1].first || arr[1].first == arr[3].first)
            y1 = arr[0].second + (arr[1].second - arr[0].second) * (long long)(x - arr[0].first) / (arr[1].first - arr[0].first);
        else
            y1 = arr[1].second + (arr[3].second - arr[1].second) * (long long)(x - arr[1].first) / (arr[3].first - arr[1].first);
        int y2;
        if(x < arr[2].first || arr[2].first == arr[3].first)
            y2 = arr[0].second + (arr[2].second - arr[0].second) * (long long)(x - arr[0].first) / (arr[2].first - arr[0].first);
        else
            y2 = arr[2].second + (arr[3].second - arr[2].second) * (long long)(x - arr[2].first) / (arr[3].first - arr[2].first);
        if(y1 > y2)
            swap(y1, y2);
        for(int y = max(y1, 0); y <= y2 && y < scr.height; y++)
            scr.putpixel(x, y, color);
    }*/
    drawTriangle(scr, pos2, pos3, pos4, color);
    drawTriangle(scr, pos1, pos3, pos4, color);
    drawTriangle(scr, pos1, pos2, pos4, color);
    drawTriangle(scr, pos1, pos2, pos3, color);
}

template<class T>
void drawTriangle3d(screen& scr, const coords& pos1, const coords& pos2, const coords& pos3, T color)
{
    coords arr[] = {pos1, pos2, pos3};
    sort(arr, arr + 3);
    if(arr[2].z <= EPS)
        return;
    if(arr[0].z > EPS)
        drawTriangle(scr, pos1.project(scr), pos2.project(scr), pos3.project(scr), color);
    else if(arr[1].z <= EPS)
    {
        arr[0].x += (arr[2].x - arr[0].x) * (EPS - arr[0].z) / (arr[2].z - arr[0].z);
        arr[0].y += (arr[2].y - arr[0].y) * (EPS - arr[0].z) / (arr[2].z - arr[0].z);
        arr[0].z = EPS;
        arr[1].x += (arr[2].x - arr[1].x) * (EPS - arr[1].z) / (arr[2].z - arr[1].z);
        arr[1].y += (arr[2].y - arr[1].y) * (EPS - arr[1].z) / (arr[2].z - arr[1].z);
        arr[1].z = EPS;
        drawTriangle(scr, arr[0].project(scr), arr[1].project(scr), arr[2].project(scr), color);
    }
    else
    {
        coords p1a = arr[0];
        p1a.x += (arr[1].x - p1a.x) * (EPS - p1a.z) / (arr[1].z - p1a.z);
        p1a.y += (arr[1].y - p1a.y) * (EPS - p1a.z) / (arr[1].z - p1a.z);
        p1a.z = EPS;
        coords p1b = arr[0];
        p1b.x += (arr[2].x - p1b.x) * (EPS - p1b.z) / (arr[2].z - p1b.z);
        p1b.y += (arr[2].y - p1b.y) * (EPS - p1b.z) / (arr[2].z - p1b.z);
        p1b.z = EPS;
        drawRectangle(scr, p1a.project(scr), p1b.project(scr), arr[1].project(scr), arr[2].project(scr), color);
    }
}

extern unordered_map<int, int*> textures;

void preDrawRect(screen& scr, const coords& p1, const coords& p2, const coords& p3, const coords& p4, void* color)
{
    drawTriangle3d(scr, p1, p3, p2, color);
    drawTriangle3d(scr, p1, p3, p4, color);
}

void drawTexture(screen& scr, const coords& p1, const coords& p2, const coords& p3, const coords& p4, int color)
{
/*  drawTriangle3d(scr, p1, p3, p2, color);
    drawTriangle3d(scr, p1, p3, p4, color);
    drawLine3d(scr, p1, p2, 0);
    drawLine3d(scr, p2, p3, 0);
    drawLine3d(scr, p3, p4, 0);
    drawLine3d(scr, p4, p1, 0);*/
    const int* texture = textures[color];
    for(int i = 0; i < 16; i++)
        for(int j = 0; j < 16; j++)
        {
#define tp(x, y) coords::texture_pos(p1, p2, p3, p4, x, y)
            coords p1_new = tp(i / 16., j / 16.);
            coords p2_new = tp((i + 1) / 16., j / 16.);
            coords p3_new = tp((i + 1) / 16., (j + 1) / 16.);
            coords p4_new = tp(i / 16., (j + 1) / 16.);
#undef tp
            drawTriangle3d(scr, p1_new, p3_new, p2_new, texture[16*j+i]);
            drawTriangle3d(scr, p1_new, p3_new, p4_new, texture[16*j+i]);
        }
}
