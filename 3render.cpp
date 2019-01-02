#include <vector>
#include <cmath>

template<class color_t>
struct Framebuffer
{
    int width;
    int height;
    color_t* buf;
    color_t& pixel_at(int x, int y)
    {
        return buf[width * y + x];
    }
};

template<class color_t>
struct Triangle
{
    double x1, y1, x2, y2, x3, y3;
    color_t c;
    Triangle(double x1, double y1, double x2, double y2, double x3, double y3, color_t c) : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), c(c){};
    static double area(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        double a = std::hypot(x2 - x1, y2 - y1);
        double b = std::hypot(x3 - x2, y3 - y2);
        double c = std::hypot(x3 - x1, y3 - y1);
        double p = (a + b + c) / 2;
        return std::sqrt(p * (p - a) * (p - b) * (p - c));
    }
    static bool crosses0(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        return ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) * ((x2 - x1) * (y4 - y1) - (y2 - y1) * (x4 - x1)) < 0;
    }
    static bool crosses(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        return crosses0(x1, y1, x2, y2, x3, y3, x4, y4) && crosses0(x3, y3, x4, y4, x1, y1, x2, y2);
    }
    static bool crosses1(double x1, double y1, double x2, double y2, double l, double r, double u, double d)
    {
        return crosses(x1, y1, x2, y2, l, u, r, u) || crosses(x1, y1, x2, y2, l, d, r, d) || crosses(x1, y1, x2, y2, l, u, l, d) || crosses(x1, y1, x2, y2, r, u, r, d);
    }
    bool crosses(double l, double r, double u, double d)
    {
        return crosses1(x1, y1, x2, y2, l, r, u, d) || crosses1(x2, y2, x3, y3, l, r, u, d) || crosses1(x1, y1, x3, y3, l, r, u, d);
    }
    bool is_inside(double x, double y)
    {
        return std::abs(area(x, y, x2, y2, x3, y3) + area(x1, y1, x, y, x3, y3) + area(x1, y1, x2, y2, x, y) - area(x1, y1, x2, y2, x3, y3)) <= 1e-7;
    }
};

template<class color_t>
struct BigPixel
{
    Framebuffer<color_t>* fb;
    int x, y, w, h;
    std::vector<Triangle<color_t> > tris;
    bool filled = false;
    BigPixel(){};
    BigPixel(Framebuffer<color_t>& fb, int x, int y, int w, int h) : fb(&fb), x(x), y(y), w(w), h(h){};
    bool is_single()
    {
        return w == 1 && h == 1;
    }
    bool is_inside(double x, double y)
    {
        return x >= this->x && x <= this->x+this->w && y >= this->y && y <= this->y+this->w;
    }
    int get_subpixels(BigPixel (&arr)[4])
    {
        if(w == 1)
        {
            arr[0] = BigPixel(*fb, x, y, 1, h / 2);
            arr[1] = BigPixel(*fb, x, y + h / 2, 1, (h + 1) / 2);
            return 2;
        }
        else if(h == 1)
        {
            arr[0] = BigPixel(*fb, x, y, w / 2, 1);
            arr[1] = BigPixel(*fb, x + w / 2, y, (w + 1) / 2, 1);
            return 2;
        }
        else
        {
            arr[0] = BigPixel(*fb, x, y, w / 2, h / 2);
            arr[1] = BigPixel(*fb, x + w / 2, y, (w + 1) / 2, h / 2);
            arr[2] = BigPixel(*fb, x, y + h / 2, w / 2, (h + 1) / 2);
            arr[3] = BigPixel(*fb, x + w / 2, y + h / 2, (w + 1) / 2, (h + 1) / 2);
            return 4;
        }
    }
    void add_tri(Triangle<color_t> tri)
    {
        int cnt = tri.is_inside(x, y) + tri.is_inside(x + w, y) + tri.is_inside(x, y + h) + tri.is_inside(x + w, y + h);
        if(cnt == 4)
        {
            tris.clear();
            filled = true;
        }
        if(cnt || is_inside(tri.x1, tri.y1) || is_inside(tri.x2, tri.y2) || is_inside(tri.x3, tri.y3) || tri.crosses(x, x + w, y, y + h))
            tris.push_back(tri);
    }
    void fill(color_t c)
    {
        for(int i = x; i < x + w; i++)
            for(int j = y; j < y + h; j++)
                fb->pixel_at(i, j) = c;
    }
    void push(color_t sky)
    {
        if(tris.empty())
        {
            fill(sky);
            return;
        }
        if(is_single() || (tris.size() == 1 && filled))
        {
            fill(tris.back().c);
            return;
        }
        BigPixel arr[4];
        int arr_len = get_subpixels(arr);
        for(int i = 0; i < arr_len; i++)
        {
            for(Triangle<color_t>& j : tris)
                arr[i].add_tri(j);
            arr[i].push(sky);
        }
    }
};

template<class color_t>
void render_3(Framebuffer<color_t>& fb, std::vector<Triangle<color_t> >& tris, color_t sky)
{
    BigPixel<color_t> bp(fb, 0, 0, fb.width, fb.height);
    for(Triangle<color_t>& tri : tris)
        bp.add_tri(tri);
    bp.push(sky);
}
