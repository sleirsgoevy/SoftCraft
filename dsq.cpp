#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

#define mp(a, b) ((((long long)(a)) << 32) | (((long long)(b)) & 0xffffffffll))

int dsq(unordered_map<long long, int>& data, int size, int down, int up, int x, int y)
{
    x %= size;
    y %= size;
    if (data.find(mp(x, y)) != data.end())
        return data[mp(x, y)];
    int base1 = (((x ^ x - 1) & 0x3fffffff) + 1) / 2;
    int base2 = (((y ^ y - 1) & 0x3fffffff) + 1) / 2;
    int base = min(base1, base2);
    int ans;
    if (base1 == base2)
        ans = dsq(data, size, down, up, x - base, y - base)
              + dsq(data, size, down, up, x - base, y + base)
              + dsq(data, size, down, up, x + base, y - base)
              + dsq(data, size, down, up, x + base, y + base);
    else
        ans = dsq(data, size, down, up, x - base, y) + dsq(data, size, down, up, x + base, y)
              + dsq(data, size, down, up, x, y - base) + dsq(data, size, down, up, x, y + base);
    ans /= 4;
    ans += rand() % ((up + down + 1) * base) - down;
    return data[mp(x, y)] = ans;
}

vector<vector<int>> _do_dsq(int size, int base_val, int down, int up)
{
    unordered_map<long long, int> data;
    data[0ll] = 0;
    vector<vector<int>> ans(size, vector<int>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            ans[i][j] = dsq(data, size, down, up, i, j) + base_val;
    return ans;
}

vector<vector<int>> do_dsq(int size, int base_val, int down, int up)
{
    int next2 = 1 << (32 - __builtin_clz(size - 1));
    vector<vector<int>> it = _do_dsq(next2, base_val, down, up);
    vector<vector<int>> ans(size, vector<int>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            ans[i][j] = it[i * next2 / size][j * next2 / size];
    return ans;
}
